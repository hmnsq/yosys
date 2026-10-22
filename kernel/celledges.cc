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

#અન્તર્ભાવ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

શૂન્ય bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	તાર્કિક is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

	માટે (પૂર્ણાંક i = 0; i < y_width; i++)
	{
		જો (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		નહીંતર જો (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

શૂન્ય bitwise_દ્વિઅંકી_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	તાર્કિક is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

	જો (cell->type == ID($and) && !is_signed) {
		જો (a_width > b_width)
			a_width = b_width;
		નહીંતર
			b_width = a_width;
	}

	માટે (પૂર્ણાંક i = 0; i < y_width; i++)
	{
		જો (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		નહીંતર જો (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		જો (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		નહીંતર જો (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

શૂન્ય arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	તાર્કિક is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

	જો (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	માટે (પૂર્ણાંક i = 0; i < y_width; i++)
	માટે (પૂર્ણાંક k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

શૂન્ય arith_દ્વિઅંકી_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	તાર્કિક is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

	જો (!is_signed && cell->type != ID($sub)) {
		પૂર્ણાંક ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	માટે (પૂર્ણાંક i = 0; i < y_width; i++)
	{
		માટે (પૂર્ણાંક k = 0; k <= i; k++)
		{
			જો (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			જો (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

શૂન્ય reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));

	માટે (પૂર્ણાંક i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

શૂન્ય logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));

	માટે (પૂર્ણાંક i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	માટે (પૂર્ણાંક i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

શૂન્ય concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));

	માટે (પૂર્ણાંક i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	માટે (પૂર્ણાંક i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

શૂન્ય slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક offset = cell->getParam(ID::OFFSET).as_int();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

	માટે (પૂર્ણાંક i = 0; i < y_width; i++) {
		પૂર્ણાંક a_bit = offset + i;
		જો (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

શૂન્ય compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));

	માટે (પૂર્ણાંક i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	માટે (પૂર્ણાંક i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

શૂન્ય mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક s_width = GetSize(cell->getPort(ID::S));

	માટે (પૂર્ણાંક i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		માટે (પૂર્ણાંક k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		માટે (પૂર્ણાંક k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

શૂન્ય bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક s_width = GetSize(cell->getPort(ID::S));

	માટે (પૂર્ણાંક i = 0; i < width; i++)
	{
		માટે (પૂર્ણાંક k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		માટે (પૂર્ણાંક k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

શૂન્ય demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક s_width = GetSize(cell->getPort(ID::S));

	માટે (પૂર્ણાંક i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		માટે (પૂર્ણાંક k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

શૂન્ય shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	તાર્કિક is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	તાર્કિક is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક y_width = GetSize(cell->getPort(ID::Y));

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
	પૂર્ણાંક b_width_capped = min(b_width, 30);

	પૂર્ણાંક b_ઉપલું, b_નીચલું;
	જો (!is_b_signed) {
		b_ઉપલું = (1 << b_width_capped) - 1;
		b_નીચલું = 0;
	} નહીંતર {
		b_ઉપલું = (1 << (b_width_capped - 1)) - 1;
		b_નીચલું = -(1 << (b_width_capped - 1));
	}

	માટે (પૂર્ણાંક i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		પૂર્ણાંક b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		પૂર્ણાંક a_range_upper;
		// lowest position of A that can be moved to Y[i]
		પૂર્ણાંક a_range_lower;

		જો (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_ઉપલું;
			જો (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_ઉપલું);
			a_range_upper = min(i+1, a_width);
		} નહીંતર જો (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_ઉપલું, a_width);
		} નહીંતર જો (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_નીચલું;
			a_range_lower = max(0, i + b_નીચલું);
			જો (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_ઉપલું, a_width);
		} નહીંતર {
			log_assert(ખોટું && "unreachable");
		}

		જો (i < b_range_upper) {
			માટે (પૂર્ણાંક k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} નહીંતર {
			// only influence is through sign extension
			જો (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		માટે (પૂર્ણાંક k = 0; k < b_width_capped; k++) {
			// left shifts
			જો (cell->type.in(ID($shl), ID($sshl))) {
				જો (a_width == 1 && is_signed) {
					પૂર્ણાંક skip = 1 << (k + 1);
					પૂર્ણાંક base = skip -1;
					જો (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} નહીંતર જો (is_signed) {
					જો (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} નહીંતર {
					જો (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} નહીંતર જો (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				જો (is_signed) {
					તાર્કિક shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					તાર્કિક zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					જો (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} નહીંતર {
					જો (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} નહીંતર જો (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				જો (is_signed) {
					જો (k != b_width_capped - 1) {
						તાર્કિક r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						તાર્કિક r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_ઉપલું));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						તાર્કિક l = a_width - 2 - i >= b_નીચલું;
						જો (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						જો (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} નહીંતર {
						જો (y_width - i <= b_ઉપલું || a_width - 2 - i >= b_નીચલું)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} નહીંતર {
					જો (a_width - 1 - i >= b_નીચલું)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} નહીંતર {
				log_assert(ખોટું && "unreachable");
			}
		}
	}
}

શૂન્ય packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	પૂર્ણાંક n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	પૂર્ણાંક abits = cell->getParam(ID::ABITS).as_int();
	પૂર્ણાંક width = cell->getParam(ID::WIDTH).as_int();

	માટે (પૂર્ણાંક i = 0; i < n_rd_ports; i++) {
		જો (rd_clk_enable[i] != State::S0) {
			માટે (પૂર્ણાંક k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			ચાલુ;
		}

		માટે (પૂર્ણાંક j = 0; j < abits; j++)
			માટે (પૂર્ણાંક k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

શૂન્ય memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	પૂર્ણાંક abits = cell->getParam(ID::ABITS).as_int();
	પૂર્ણાંક width = cell->getParam(ID::WIDTH).as_int();

	જો (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		જો (cell->type == ID($memrd_v2)) {
			માટે (પૂર્ણાંક k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		પાછા;
	}

	માટે (પૂર્ણાંક j = 0; j < abits; j++)
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

શૂન્ય mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	જો (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	નહીંતર જો (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	નહીંતર જો (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		પાછા; /* no edges here */
	નહીંતર
		log_abort();
}

શૂન્ય ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક width = cell->getPort(ID::Q).size();

	જો (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		માટે (પૂર્ણાંક k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	જો (cell->hasPort(ID::CLR))
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	જો (cell->hasPort(ID::SET))
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	જો (cell->hasPort(ID::ALOAD))
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	જો (cell->hasPort(ID::AD))
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	જો (cell->hasPort(ID::ARST))
		માટે (પૂર્ણાંક k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

શૂન્ય full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> આગત_ports;
	std::vector<RTLIL::IdString> output_ports;

	માટે (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		જો (cell->આગત(port) || dir == RTLIL::PortDir::PD_INOUT)
			આગત_ports.ધકેલો_back(port);
		જો (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.ધકેલો_back(port);
	}

	માટે (auto out_port : output_ports)
	{
		પૂર્ણાંક out_width = GetSize(cell->getPort(out_port));
		માટે (પૂર્ણાંક out_bit = 0; out_bit < out_width; out_bit++)
		{
			માટે (auto in_port : આગત_ports)
			{
				પૂર્ણાંક in_width = GetSize(cell->getPort(in_port));
				માટે (પૂર્ણાંક in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

શૂન્ય bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક max_width = std::min(width, std::min(a_width, b_width));

	માટે (પૂર્ણાંક i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

શૂન્ય bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
	પૂર્ણાંક a_width = GetSize(cell->getPort(ID::A));
	પૂર્ણાંક b_width = GetSize(cell->getPort(ID::B));
	પૂર્ણાંક s_width = GetSize(cell->getPort(ID::S));
	પૂર્ણાંક max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	માટે (પૂર્ણાંક i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

તાર્કિક YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	જો (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_દ્વિઅંકી_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($neg)) {
		arith_neg_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($add), ID($sub))) {
		arith_દ્વિઅંકી_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($slice)) {
		slice_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($concat)) {
		concat_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($bmux)) {
		bmux_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($demux)) {
		demux_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($bweqx)) {
		bweqx_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type == ID($bwmux)) {
		bwmux_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->is_builtin_ff()) {
		ff_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(આ, cell);
		પાછા સાચું;
	}

	જો (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(આ, cell);
		પાછા સાચું;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	જો (cell->type.in(ID($પ્રતિજ્ઞા), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		પાછા સાચું; // no-op: these have either no inputs or no outputs

	પાછા ખોટું;
}
