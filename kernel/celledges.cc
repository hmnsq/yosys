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

#అన్తర్భావ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
వ్యక్తిగత_NAMESPACE_BEGIN

శూన్య bitwise_unary_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	తార్కిక is_signed = !cell->type.లోపల(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++)
	{
		ఒకవేళ (i < a_వెడల్పు)
			db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);
		లేకపోతే ఒకవేళ (is_signed && a_వెడల్పు > 0)
			db->కూడు_అంచు(cell, ID::A, a_వెడల్పు-1, ID::Y, i, -1);
	}
}

శూన్య bitwise_ద్విమాన_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	తార్కిక is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

	ఒకవేళ (cell->type == ID($and) && !is_signed) {
		ఒకవేళ (a_వెడల్పు > b_వెడల్పు)
			a_వెడల్పు = b_వెడల్పు;
		లేకపోతే
			b_వెడల్పు = a_వెడల్పు;
	}

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++)
	{
		ఒకవేళ (i < a_వెడల్పు)
			db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);
		లేకపోతే ఒకవేళ (is_signed && a_వెడల్పు > 0)
			db->కూడు_అంచు(cell, ID::A, a_వెడల్పు-1, ID::Y, i, -1);

		ఒకవేళ (i < b_వెడల్పు)
			db->కూడు_అంచు(cell, ID::B, i, ID::Y, i, -1);
		లేకపోతే ఒకవేళ (is_signed && b_వెడల్పు > 0)
			db->కూడు_అంచు(cell, ID::B, b_వెడల్పు-1, ID::Y, i, -1);
	}
}

శూన్య arith_neg_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	తార్కిక is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

	ఒకవేళ (is_signed && a_వెడల్పు == 1)
		y_వెడల్పు = std::min(y_వెడల్పు, 1);

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++)
	కొరకు (పూర్ణాంకం k = 0; k <= i && k < a_వెడల్పు; k++)
		db->కూడు_అంచు(cell, ID::A, k, ID::Y, i, -1);
}

శూన్య arith_ద్విమాన_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	తార్కిక is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

	ఒకవేళ (!is_signed && cell->type != ID($sub)) {
		పూర్ణాంకం ab_వెడల్పు = std::max(a_వెడల్పు, b_వెడల్పు);
		y_వెడల్పు = std::min(y_వెడల్పు, ab_వెడల్పు+1);
	}

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++)
	{
		కొరకు (పూర్ణాంకం k = 0; k <= i; k++)
		{
			ఒకవేళ (k < a_వెడల్పు)
				db->కూడు_అంచు(cell, ID::A, k, ID::Y, i, -1);

			ఒకవేళ (k < b_వెడల్పు)
				db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

శూన్య reduce_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));

	కొరకు (పూర్ణాంకం i = 0; i < a_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, 0, -1);
}

శూన్య logic_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));

	కొరకు (పూర్ణాంకం i = 0; i < a_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, 0, -1);
	కొరకు (పూర్ణాంకం i = 0; i < b_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::B, i, ID::Y, 0, -1);
}

శూన్య concat_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));

	కొరకు (పూర్ణాంకం i = 0; i < a_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);
	కొరకు (పూర్ణాంకం i = 0; i < b_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::B, i, ID::Y, a_వెడల్పు + i, -1);
}

శూన్య ముక్క_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం స్థానభ్రంశం = cell->getParam(ID::స్థానభ్రంశం_2).as_int();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++) {
		పూర్ణాంకం a_బిట్టు = స్థానభ్రంశం + i;
		ఒకవేళ (a_బిట్టు >= 0 && a_బిట్టు < a_వెడల్పు)
			db->కూడు_అంచు(cell, ID::A, a_బిట్టు, ID::Y, i, -1);
	}
}

శూన్య compare_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));

	కొరకు (పూర్ణాంకం i = 0; i < a_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, 0, -1);

	కొరకు (పూర్ణాంకం i = 0; i < b_వెడల్పు; i++)
		db->కూడు_అంచు(cell, ID::B, i, ID::Y, 0, -1);
}

శూన్య mux_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం s_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::S));

	కొరకు (పూర్ణాంకం i = 0; i < a_వెడల్పు; i++)
	{
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);

		కొరకు (పూర్ణాంకం k = i; k < b_వెడల్పు; k += a_వెడల్పు)
			db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);

		కొరకు (పూర్ణాంకం k = 0; k < s_వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::S, k, ID::Y, i, -1);
	}
}

శూన్య bmux_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం s_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::S));

	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
	{
		కొరకు (పూర్ణాంకం k = i; k < a_వెడల్పు; k += వెడల్పు)
			db->కూడు_అంచు(cell, ID::A, k, ID::Y, i, -1);

		కొరకు (పూర్ణాంకం k = 0; k < s_వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::S, k, ID::Y, i, -1);
	}
}

శూన్య demux_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం s_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::S));

	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
	{
		db->కూడు_అంచు(cell, ID::A, i % a_వెడల్పు, ID::Y, i, -1);
		కొరకు (పూర్ణాంకం k = 0; k < s_వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::S, k, ID::Y, i, -1);
	}
}

శూన్య జరుపు_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	తార్కిక is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	తార్కిక is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం y_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));

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
	పూర్ణాంకం b_వెడల్పు_capped = min(b_వెడల్పు, 30);

	పూర్ణాంకం b_ఎగువ, b_దిగువ;
	ఒకవేళ (!is_b_signed) {
		b_ఎగువ = (1 << b_వెడల్పు_capped) - 1;
		b_దిగువ = 0;
	} లేకపోతే {
		b_ఎగువ = (1 << (b_వెడల్పు_capped - 1)) - 1;
		b_దిగువ = -(1 << (b_వెడల్పు_capped - 1));
	}

	కొరకు (పూర్ణాంకం i = 0; i < y_వెడల్పు; i++){
		// highest position of Y that can change with the value of B
		పూర్ణాంకం b_range_పై = 0;
		// 1 + highest position of A that can be moved to Y[i]
		పూర్ణాంకం a_range_పై;
		// lowest position of A that can be moved to Y[i]
		పూర్ణాంకం a_range_క్రింది;

		ఒకవేళ (cell->type.లోపల(ID($shl), ID($sshl))) {
			b_range_పై = a_వెడల్పు + b_ఎగువ;
			ఒకవేళ (is_signed) b_range_పై -= 1;
			a_range_క్రింది = max(0, i - b_ఎగువ);
			a_range_పై = min(i+1, a_వెడల్పు);
		} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($shr), ID($sshr)) || (cell->type.లోపల(ID($జరుపు), ID($shiftx)) && !is_b_signed)) {
			b_range_పై = a_వెడల్పు;
			a_range_క్రింది = min(i, a_వెడల్పు - 1);
			a_range_పై = min(i+1 + b_ఎగువ, a_వెడల్పు);
		} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($జరుపు), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_పై = a_వెడల్పు - b_దిగువ;
			a_range_క్రింది = max(0, i + b_దిగువ);
			ఒకవేళ (is_signed)
				a_range_క్రింది = min(a_range_క్రింది, a_వెడల్పు - 1);
			a_range_పై = min(i+1 + b_ఎగువ, a_వెడల్పు);
		} లేకపోతే {
			log_assert(అబద్ధం && "unreachable");
		}

		ఒకవేళ (i < b_range_పై) {
			కొరకు (పూర్ణాంకం k = a_range_క్రింది; k < a_range_పై; k++)
				db->కూడు_అంచు(cell, ID::A, k, ID::Y, i, -1);
		} లేకపోతే {
			// only influence is through sign extension
			ఒకవేళ (is_signed)
				db->కూడు_అంచు(cell, ID::A, a_వెడల్పు - 1, ID::Y, i, -1);
		}

		కొరకు (పూర్ణాంకం k = 0; k < b_వెడల్పు_capped; k++) {
			// left shifts
			ఒకవేళ (cell->type.లోపల(ID($shl), ID($sshl))) {
				ఒకవేళ (a_వెడల్పు == 1 && is_signed) {
					పూర్ణాంకం skip = 1 << (k + 1);
					పూర్ణాంకం పునాది = skip -1;
					ఒకవేళ (i % skip != పునాది && i - a_వెడల్పు + 2 < 1 << b_వెడల్పు_capped)
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				} లేకపోతే ఒకవేళ (is_signed) {
					ఒకవేళ (i - a_వెడల్పు + 2 < 1 << b_వెడల్పు_capped)
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				} లేకపోతే {
					ఒకవేళ (i - a_వెడల్పు + 1 < 1 << b_వెడల్పు_capped)
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($shr), ID($sshr)) || (cell->type.లోపల(ID($జరుపు), ID($shiftx)) && !is_b_signed)) {
				ఒకవేళ (is_signed) {
					తార్కిక జరుపు_లోపల_bulk = i < a_వెడల్పు - 1;
					// can we jump into the zero-padding by toggling B[k]?
					తార్కిక zpad_jump = (((y_వెడల్పు - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_వెడల్పు - i) & ~(1 << k)) < (1 << b_వెడల్పు_capped)));

					ఒకవేళ (జరుపు_లోపల_bulk || (cell->type.లోపల(ID($shr), ID($జరుపు), ID($shiftx)) && zpad_jump))
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				} లేకపోతే {
					ఒకవేళ (i < a_వెడల్పు)
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($జరుపు), ID($shiftx)) && is_b_signed) {
				ఒకవేళ (is_signed) {
					ఒకవేళ (k != b_వెడల్పు_capped - 1) {
						తార్కిక r_జరుపు_లోపల_bulk = i < a_వెడల్పు - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						తార్కిక r_zpad_jump = (((y_వెడల్పు - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_వెడల్పు - i) & ~(1 << k)) <= b_ఎగువ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						తార్కిక l = a_వెడల్పు - 2 - i >= b_దిగువ;
						ఒకవేళ (a_వెడల్పు == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_వెడల్పు) & ((1 << (k + 1)) - 1)) != 0;
						}
						ఒకవేళ (r_జరుపు_లోపల_bulk || r_zpad_jump || l)
							db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
					} లేకపోతే {
						ఒకవేళ (y_వెడల్పు - i <= b_ఎగువ || a_వెడల్పు - 2 - i >= b_దిగువ)
							db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
					}
				} లేకపోతే {
					ఒకవేళ (a_వెడల్పు - 1 - i >= b_దిగువ)
						db->కూడు_అంచు(cell, ID::B, k, ID::Y, i, -1);
				}
			} లేకపోతే {
				log_assert(అబద్ధం && "unreachable");
			}
		}
	}
}

శూన్య packed_mem_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	పూర్ణాంకం n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	పూర్ణాంకం abits = cell->getParam(ID::ABITS).as_int();
	పూర్ణాంకం వెడల్పు = cell->getParam(ID::వెడల్పు_2).as_int();

	కొరకు (పూర్ణాంకం i = 0; i < n_rd_ports; i++) {
		ఒకవేళ (rd_clk_enable[i] != State::S0) {
			కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
				db->కూడు_అంచు(cell, ID::RD_ARST, i, ID::RD_దత్తాంశం, i * వెడల్పు + k, -1);
			కొనసాగు;
		}

		కొరకు (పూర్ణాంకం j = 0; j < abits; j++)
			కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
				db->కూడు_అంచు(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_దత్తాంశం, i * వెడల్పు + k, -1);
	}
}

శూన్య memrd_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.లోపల(ID($memrd), ID($memrd_v2)));
	పూర్ణాంకం abits = cell->getParam(ID::ABITS).as_int();
	పూర్ణాంకం వెడల్పు = cell->getParam(ID::వెడల్పు_2).as_int();

	ఒకవేళ (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ఒకవేళ (cell->type == ID($memrd_v2)) {
			కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
				db->కూడు_అంచు(cell, ID::ARST, 0, ID::దత్తాంశం_2, k, -1);
		}
		తిరిగి;
	}

	కొరకు (పూర్ణాంకం j = 0; j < abits; j++)
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::ADDR, j, ID::దత్తాంశం_2, k, -1);
}

శూన్య mem_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	ఒకవేళ (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	లేకపోతే ఒకవేళ (cell->type.లోపల(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	లేకపోతే ఒకవేళ (cell->type.లోపల(ID($memwr), ID($memwr_v2), ID($meminit)))
		తిరిగి; /* no edges here */
	లేకపోతే
		log_abort();
}

శూన్య ff_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం వెడల్పు = cell->get_పోర్టు(ID::Q).కొలత();

	ఒకవేళ (cell->type.లోపల(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++) {
			db->కూడు_అంచు(cell, ID::D, k, ID::Q, k, -1);
			db->కూడు_అంచు(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ఒకవేళ (cell->has_పోర్టు(ID::CLR))
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::CLR, 0, ID::Q, k, -1);
	ఒకవేళ (cell->has_పోర్టు(ID::సమితి_2))
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::సమితి_2, 0, ID::Q, k, -1);
	ఒకవేళ (cell->has_పోర్టు(ID::ALOAD))
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ఒకవేళ (cell->has_పోర్టు(ID::AD))
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::AD, k, ID::Q, k, -1);
	ఒకవేళ (cell->has_పోర్టు(ID::ARST))
		కొరకు (పూర్ణాంకం k = 0; k < వెడల్పు; k++)
			db->కూడు_అంచు(cell, ID::ARST, 0, ID::Q, k, -1);
}

శూన్య నిండిన_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	std::సదిశ<RTLIL::IdString> ఆదానం_ports;
	std::సదిశ<RTLIL::IdString> output_ports;

	కొరకు (స్వయంచాలక &conn : cell->connections())
	{
		RTLIL::IdString పోర్టు = conn.మొదటి;
		RTLIL::పోర్టు_Dir dir = cell->పోర్టు_dir(పోర్టు);
		ఒకవేళ (cell->ఆదానం(పోర్టు) || dir == RTLIL::పోర్టు_Dir::PD_INOUT)
			ఆదానం_ports.నెట్టు_వెనుక(పోర్టు);
		ఒకవేళ (cell->output(పోర్టు) || dir == RTLIL::పోర్టు_Dir::PD_INOUT)
			output_ports.నెట్టు_వెనుక(పోర్టు);
	}

	కొరకు (స్వయంచాలక బహిర్గతం_పోర్టు : output_ports)
	{
		పూర్ణాంకం బహిర్గతం_వెడల్పు = Get_కొలత(cell->get_పోర్టు(బహిర్గతం_పోర్టు));
		కొరకు (పూర్ణాంకం బహిర్గతం_బిట్టు = 0; బహిర్గతం_బిట్టు < బహిర్గతం_వెడల్పు; బహిర్గతం_బిట్టు++)
		{
			కొరకు (స్వయంచాలక లోపల_పోర్టు : ఆదానం_ports)
			{
				పూర్ణాంకం లోపల_వెడల్పు = Get_కొలత(cell->get_పోర్టు(లోపల_పోర్టు));
				కొరకు (పూర్ణాంకం లోపల_బిట్టు = 0; లోపల_బిట్టు < లోపల_వెడల్పు; లోపల_బిట్టు++)
					db->కూడు_అంచు(cell, లోపల_పోర్టు, లోపల_బిట్టు, బహిర్గతం_పోర్టు, బహిర్గతం_బిట్టు, -1);
			}
		}
	}
}

శూన్య bweqx_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం max_వెడల్పు = std::min(వెడల్పు, std::min(a_వెడల్పు, b_వెడల్పు));

	కొరకు (పూర్ణాంకం i = 0; i < max_వెడల్పు; i++) {
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);
		db->కూడు_అంచు(cell, ID::B, i, ID::Y, i, -1);
	}
}

శూన్య bwmux_op(Abstract_Cell_అంచులు_Database *db, RTLIL::Cell *cell)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
	పూర్ణాంకం a_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::A));
	పూర్ణాంకం b_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::B));
	పూర్ణాంకం s_వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::S));
	పూర్ణాంకం max_వెడల్పు = std::min(వెడల్పు, std::min(a_వెడల్పు, std::min(b_వెడల్పు, s_వెడల్పు)));

	కొరకు (పూర్ణాంకం i = 0; i < max_వెడల్పు; i++) {
		db->కూడు_అంచు(cell, ID::A, i, ID::Y, i, -1);
		db->కూడు_అంచు(cell, ID::B, i, ID::Y, i, -1);
		db->కూడు_అంచు(cell, ID::S, i, ID::Y, i, -1);
	}
}

వ్యక్తిగత_NAMESPACE_చివర

తార్కిక YOSYS_NAMESPACE_ఉపసర్గ Abstract_Cell_అంచులు_Database::కూడు_అంచులు_from_cell(RTLIL::Cell *cell)
{
	ఒకవేళ (cell->type.లోపల(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ద్విమాన_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($neg)) {
		arith_neg_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($కూడు), ID($sub))) {
		arith_ద్విమాన_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($logic_and), ID($logic_or))) {
		logic_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($ముక్క)) {
		ముక్క_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($concat)) {
		concat_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($జరుపు), ID($shiftx))) {
		జరుపు_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($mux), ID($pmux))) {
		mux_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($bmux)) {
		bmux_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($demux)) {
		demux_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($bweqx)) {
		bweqx_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($bwmux)) {
		bwmux_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->is_builtin_ff()) {
		ff_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ఘాతం))) {
		నిండిన_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		నిండిన_op(ఇది, cell);
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		నిండిన_op(ఇది, cell);
		తిరిగి నిజం;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ఒకవేళ (cell->type.లోపల(ID($ప్రతిజ్ఞా), ID($assume), ID($సజీవ), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		తిరిగి నిజం; // no-op: these have either no inputs or no outputs

	తిరిగి అబద్ధం;
}
