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

#समावेश "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

व्योम bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

	क्रम (पूर्णांक i = 0; i < y_width; i++)
	{
		अगर (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा अगर (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

व्योम bitwise_द्विआधारी_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

	अगर (cell->type == ID($and) && !is_signed) {
		अगर (a_width > b_width)
			a_width = b_width;
		अन्यथा
			b_width = a_width;
	}

	क्रम (पूर्णांक i = 0; i < y_width; i++)
	{
		अगर (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा अगर (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		अगर (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		अन्यथा अगर (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

व्योम arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

	अगर (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	क्रम (पूर्णांक i = 0; i < y_width; i++)
	क्रम (पूर्णांक k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

व्योम arith_द्विआधारी_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

	अगर (!is_signed && cell->type != ID($sub)) {
		पूर्णांक ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	क्रम (पूर्णांक i = 0; i < y_width; i++)
	{
		क्रम (पूर्णांक k = 0; k <= i; k++)
		{
			अगर (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			अगर (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

व्योम reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));

	क्रम (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

व्योम logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));

	क्रम (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	क्रम (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

व्योम concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));

	क्रम (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	क्रम (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

व्योम slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक offset = cell->getParam(ID::OFFSET).as_int();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

	क्रम (पूर्णांक i = 0; i < y_width; i++) {
		पूर्णांक a_bit = offset + i;
		अगर (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

व्योम compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));

	क्रम (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	क्रम (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

व्योम mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक s_width = Get_आकार(cell->getPort(ID::S));

	क्रम (पूर्णांक i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		क्रम (पूर्णांक k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		क्रम (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

व्योम bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक s_width = Get_आकार(cell->getPort(ID::S));

	क्रम (पूर्णांक i = 0; i < width; i++)
	{
		क्रम (पूर्णांक k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		क्रम (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

व्योम demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक s_width = Get_आकार(cell->getPort(ID::S));

	क्रम (पूर्णांक i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		क्रम (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

व्योम shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	bool is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक y_width = Get_आकार(cell->getPort(ID::Y));

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

	पूर्णांक b_ऊपरी, b_निचला;
	अगर (!is_b_signed) {
		b_ऊपरी = (1 << b_width_capped) - 1;
		b_निचला = 0;
	} अन्यथा {
		b_ऊपरी = (1 << (b_width_capped - 1)) - 1;
		b_निचला = -(1 << (b_width_capped - 1));
	}

	क्रम (पूर्णांक i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		पूर्णांक b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		पूर्णांक a_range_upper;
		// lowest position of A that can be moved to Y[i]
		पूर्णांक a_range_lower;

		अगर (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_ऊपरी;
			अगर (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_ऊपरी);
			a_range_upper = min(i+1, a_width);
		} अन्यथा अगर (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_ऊपरी, a_width);
		} अन्यथा अगर (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_निचला;
			a_range_lower = max(0, i + b_निचला);
			अगर (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_ऊपरी, a_width);
		} अन्यथा {
			log_assert(false && "unreachable");
		}

		अगर (i < b_range_upper) {
			क्रम (पूर्णांक k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} अन्यथा {
			// only influence is through sign extension
			अगर (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		क्रम (पूर्णांक k = 0; k < b_width_capped; k++) {
			// left shifts
			अगर (cell->type.in(ID($shl), ID($sshl))) {
				अगर (a_width == 1 && is_signed) {
					पूर्णांक skip = 1 << (k + 1);
					पूर्णांक base = skip -1;
					अगर (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा अगर (is_signed) {
					अगर (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					अगर (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} अन्यथा अगर (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				अगर (is_signed) {
					bool shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					bool zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					अगर (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					अगर (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} अन्यथा अगर (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				अगर (is_signed) {
					अगर (k != b_width_capped - 1) {
						bool r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						bool r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_ऊपरी));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						bool l = a_width - 2 - i >= b_निचला;
						अगर (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						अगर (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} अन्यथा {
						अगर (y_width - i <= b_ऊपरी || a_width - 2 - i >= b_निचला)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} अन्यथा {
					अगर (a_width - 1 - i >= b_निचला)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} अन्यथा {
				log_assert(false && "unreachable");
			}
		}
	}
}

व्योम packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	पूर्णांक n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	पूर्णांक abits = cell->getParam(ID::ABITS).as_int();
	पूर्णांक width = cell->getParam(ID::WIDTH).as_int();

	क्रम (पूर्णांक i = 0; i < n_rd_ports; i++) {
		अगर (rd_clk_enable[i] != State::S0) {
			क्रम (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_तथ्य, i * width + k, -1);
			जारी;
		}

		क्रम (पूर्णांक j = 0; j < abits; j++)
			क्रम (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_तथ्य, i * width + k, -1);
	}
}

व्योम memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	पूर्णांक abits = cell->getParam(ID::ABITS).as_int();
	पूर्णांक width = cell->getParam(ID::WIDTH).as_int();

	अगर (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		अगर (cell->type == ID($memrd_v2)) {
			क्रम (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::तथ्य_2, k, -1);
		}
		वापस;
	}

	क्रम (पूर्णांक j = 0; j < abits; j++)
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::तथ्य_2, k, -1);
}

व्योम mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	अगर (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	अन्यथा अगर (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	अन्यथा अगर (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		वापस; /* no edges here */
	अन्यथा
		log_abort();
}

व्योम ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = cell->getPort(ID::Q).आकार();

	अगर (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		क्रम (पूर्णांक k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	अगर (cell->hasPort(ID::CLR))
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	अगर (cell->hasPort(ID::SET))
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	अगर (cell->hasPort(ID::ALOAD))
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	अगर (cell->hasPort(ID::AD))
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	अगर (cell->hasPort(ID::ARST))
		क्रम (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

व्योम full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> निवेश_ports;
	std::vector<RTLIL::IdString> output_ports;

	क्रम (स्वतः &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		अगर (cell->निवेश(port) || dir == RTLIL::PortDir::PD_INOUT)
			निवेश_ports.धकेलो_back(port);
		अगर (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.धकेलो_back(port);
	}

	क्रम (स्वतः out_port : output_ports)
	{
		पूर्णांक out_width = Get_आकार(cell->getPort(out_port));
		क्रम (पूर्णांक out_bit = 0; out_bit < out_width; out_bit++)
		{
			क्रम (स्वतः in_port : निवेश_ports)
			{
				पूर्णांक in_width = Get_आकार(cell->getPort(in_port));
				क्रम (पूर्णांक in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

व्योम bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक max_width = std::min(width, std::min(a_width, b_width));

	क्रम (पूर्णांक i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

व्योम bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
	पूर्णांक a_width = Get_आकार(cell->getPort(ID::A));
	पूर्णांक b_width = Get_आकार(cell->getPort(ID::B));
	पूर्णांक s_width = Get_आकार(cell->getPort(ID::S));
	पूर्णांक max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	क्रम (पूर्णांक i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

bool YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	अगर (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_द्विआधारी_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($neg)) {
		arith_neg_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($add), ID($sub))) {
		arith_द्विआधारी_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($slice)) {
		slice_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($concat)) {
		concat_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($bmux)) {
		bmux_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($demux)) {
		demux_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($bweqx)) {
		bweqx_op(यह, cell);
		वापस true;
	}

	अगर (cell->type == ID($bwmux)) {
		bwmux_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(यह, cell);
		वापस true;
	}

	अगर (cell->is_builtin_ff()) {
		ff_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात))) {
		full_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(यह, cell);
		वापस true;
	}

	अगर (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(यह, cell);
		वापस true;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	अगर (cell->type.in(ID($निश्चित), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		वापस true; // no-op: these have either no inputs or no outputs

	वापस false;
}
