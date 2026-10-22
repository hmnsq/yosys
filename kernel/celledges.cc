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

#ضمن "kernel/celledges.h"

USING_YOSYS_NAMESPACE
شخصي_NAMESPACE_BEGIN

فراغ bitwise_unary_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = !cell->type.دننه(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

	لكل (صحيح i = 0; i < y_پلنوالی; i++)
	{
		إذا (i < a_پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_پلنوالی > 0)
			db->جمع_کړه_څنډه(cell, ID::A, a_پلنوالی-1, ID::Y, i, -1);
	}
}

فراغ bitwise_دوګونی_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

	إذا (cell->type == ID($and) && !is_signed) {
		إذا (a_پلنوالی > b_پلنوالی)
			a_پلنوالی = b_پلنوالی;
		وإلا
			b_پلنوالی = a_پلنوالی;
	}

	لكل (صحيح i = 0; i < y_پلنوالی; i++)
	{
		إذا (i < a_پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_پلنوالی > 0)
			db->جمع_کړه_څنډه(cell, ID::A, a_پلنوالی-1, ID::Y, i, -1);

		إذا (i < b_پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, i, -1);
		وإلا إذا (is_signed && b_پلنوالی > 0)
			db->جمع_کړه_څنډه(cell, ID::B, b_پلنوالی-1, ID::Y, i, -1);
	}
}

فراغ arith_neg_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

	إذا (is_signed && a_پلنوالی == 1)
		y_پلنوالی = std::min(y_پلنوالی, 1);

	لكل (صحيح i = 0; i < y_پلنوالی; i++)
	لكل (صحيح k = 0; k <= i && k < a_پلنوالی; k++)
		db->جمع_کړه_څنډه(cell, ID::A, k, ID::Y, i, -1);
}

فراغ arith_دوګونی_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

	إذا (!is_signed && cell->type != ID($sub)) {
		صحيح ab_پلنوالی = std::max(a_پلنوالی, b_پلنوالی);
		y_پلنوالی = std::min(y_پلنوالی, ab_پلنوالی+1);
	}

	لكل (صحيح i = 0; i < y_پلنوالی; i++)
	{
		لكل (صحيح k = 0; k <= i; k++)
		{
			إذا (k < a_پلنوالی)
				db->جمع_کړه_څنډه(cell, ID::A, k, ID::Y, i, -1);

			إذا (k < b_پلنوالی)
				db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

فراغ reduce_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));

	لكل (صحيح i = 0; i < a_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, 0, -1);
}

فراغ logic_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));

	لكل (صحيح i = 0; i < a_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, 0, -1);
	لكل (صحيح i = 0; i < b_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ concat_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));

	لكل (صحيح i = 0; i < a_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);
	لكل (صحيح i = 0; i < b_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, a_پلنوالی + i, -1);
}

فراغ ټوټه_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح دننه_واټن = cell->getParam(ID::دننه_واټن_2).as_int();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

	لكل (صحيح i = 0; i < y_پلنوالی; i++) {
		صحيح a_بټ = دننه_واټن + i;
		إذا (a_بټ >= 0 && a_بټ < a_پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::A, a_بټ, ID::Y, i, -1);
	}
}

فراغ compare_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));

	لكل (صحيح i = 0; i < a_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, 0, -1);

	لكل (صحيح i = 0; i < b_پلنوالی; i++)
		db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ mux_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح s_پلنوالی = Get_اندازه(cell->get_پورټ(ID::S));

	لكل (صحيح i = 0; i < a_پلنوالی; i++)
	{
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);

		لكل (صحيح k = i; k < b_پلنوالی; k += a_پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ bmux_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح s_پلنوالی = Get_اندازه(cell->get_پورټ(ID::S));

	لكل (صحيح i = 0; i < پلنوالی; i++)
	{
		لكل (صحيح k = i; k < a_پلنوالی; k += پلنوالی)
			db->جمع_کړه_څنډه(cell, ID::A, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ demux_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح s_پلنوالی = Get_اندازه(cell->get_پورټ(ID::S));

	لكل (صحيح i = 0; i < پلنوالی; i++)
	{
		db->جمع_کړه_څنډه(cell, ID::A, i % a_پلنوالی, ID::Y, i, -1);
		لكل (صحيح k = 0; k < s_پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ ښويدنه_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقي is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح y_پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));

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
	صحيح b_پلنوالی_capped = min(b_پلنوالی, 30);

	صحيح b_لوړ, b_ټیټ;
	إذا (!is_b_signed) {
		b_لوړ = (1 << b_پلنوالی_capped) - 1;
		b_ټیټ = 0;
	} وإلا {
		b_لوړ = (1 << (b_پلنوالی_capped - 1)) - 1;
		b_ټیټ = -(1 << (b_پلنوالی_capped - 1));
	}

	لكل (صحيح i = 0; i < y_پلنوالی; i++){
		// highest position of Y that can change with the value of B
		صحيح b_range_پاسنی = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحيح a_range_پاسنی;
		// lowest position of A that can be moved to Y[i]
		صحيح a_range_ښکتنی;

		إذا (cell->type.دننه(ID($shl), ID($sshl))) {
			b_range_پاسنی = a_پلنوالی + b_لوړ;
			إذا (is_signed) b_range_پاسنی -= 1;
			a_range_ښکتنی = max(0, i - b_لوړ);
			a_range_پاسنی = min(i+1, a_پلنوالی);
		} وإلا إذا (cell->type.دننه(ID($shr), ID($sshr)) || (cell->type.دننه(ID($ښويدنه), ID($shiftx)) && !is_b_signed)) {
			b_range_پاسنی = a_پلنوالی;
			a_range_ښکتنی = min(i, a_پلنوالی - 1);
			a_range_پاسنی = min(i+1 + b_لوړ, a_پلنوالی);
		} وإلا إذا (cell->type.دننه(ID($ښويدنه), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_پاسنی = a_پلنوالی - b_ټیټ;
			a_range_ښکتنی = max(0, i + b_ټیټ);
			إذا (is_signed)
				a_range_ښکتنی = min(a_range_ښکتنی, a_پلنوالی - 1);
			a_range_پاسنی = min(i+1 + b_لوړ, a_پلنوالی);
		} وإلا {
			log_assert(خطأ && "unreachable");
		}

		إذا (i < b_range_پاسنی) {
			لكل (صحيح k = a_range_ښکتنی; k < a_range_پاسنی; k++)
				db->جمع_کړه_څنډه(cell, ID::A, k, ID::Y, i, -1);
		} وإلا {
			// only influence is through sign extension
			إذا (is_signed)
				db->جمع_کړه_څنډه(cell, ID::A, a_پلنوالی - 1, ID::Y, i, -1);
		}

		لكل (صحيح k = 0; k < b_پلنوالی_capped; k++) {
			// left shifts
			إذا (cell->type.دننه(ID($shl), ID($sshl))) {
				إذا (a_پلنوالی == 1 && is_signed) {
					صحيح skip = 1 << (k + 1);
					صحيح بنسټ = skip -1;
					إذا (i % skip != بنسټ && i - a_پلنوالی + 2 < 1 << b_پلنوالی_capped)
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				} وإلا إذا (is_signed) {
					إذا (i - a_پلنوالی + 2 < 1 << b_پلنوالی_capped)
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i - a_پلنوالی + 1 < 1 << b_پلنوالی_capped)
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} وإلا إذا (cell->type.دننه(ID($shr), ID($sshr)) || (cell->type.دننه(ID($ښويدنه), ID($shiftx)) && !is_b_signed)) {
				إذا (is_signed) {
					منطقي ښويدنه_دننه_bulk = i < a_پلنوالی - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقي zpad_jump = (((y_پلنوالی - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_پلنوالی - i) & ~(1 << k)) < (1 << b_پلنوالی_capped)));

					إذا (ښويدنه_دننه_bulk || (cell->type.دننه(ID($shr), ID($ښويدنه), ID($shiftx)) && zpad_jump))
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i < a_پلنوالی)
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} وإلا إذا (cell->type.دننه(ID($ښويدنه), ID($shiftx)) && is_b_signed) {
				إذا (is_signed) {
					إذا (k != b_پلنوالی_capped - 1) {
						منطقي r_ښويدنه_دننه_bulk = i < a_پلنوالی - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقي r_zpad_jump = (((y_پلنوالی - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_پلنوالی - i) & ~(1 << k)) <= b_لوړ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقي l = a_پلنوالی - 2 - i >= b_ټیټ;
						إذا (a_پلنوالی == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_پلنوالی) & ((1 << (k + 1)) - 1)) != 0;
						}
						إذا (r_ښويدنه_دننه_bulk || r_zpad_jump || l)
							db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
					} وإلا {
						إذا (y_پلنوالی - i <= b_لوړ || a_پلنوالی - 2 - i >= b_ټیټ)
							db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
					}
				} وإلا {
					إذا (a_پلنوالی - 1 - i >= b_ټیټ)
						db->جمع_کړه_څنډه(cell, ID::B, k, ID::Y, i, -1);
				}
			} وإلا {
				log_assert(خطأ && "unreachable");
			}
		}
	}
}

فراغ packed_mem_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحيح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح پلنوالی = cell->getParam(ID::پلنوالی_2).as_int();

	لكل (صحيح i = 0; i < n_rd_ports; i++) {
		إذا (rd_clk_enable[i] != State::S0) {
			لكل (صحيح k = 0; k < پلنوالی; k++)
				db->جمع_کړه_څنډه(cell, ID::RD_ARST, i, ID::RD_معلومات, i * پلنوالی + k, -1);
			استمر;
		}

		لكل (صحيح j = 0; j < abits; j++)
			لكل (صحيح k = 0; k < پلنوالی; k++)
				db->جمع_کړه_څنډه(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_معلومات, i * پلنوالی + k, -1);
	}
}

فراغ memrd_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.دننه(ID($memrd), ID($memrd_v2)));
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح پلنوالی = cell->getParam(ID::پلنوالی_2).as_int();

	إذا (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		إذا (cell->type == ID($memrd_v2)) {
			لكل (صحيح k = 0; k < پلنوالی; k++)
				db->جمع_کړه_څنډه(cell, ID::ARST, 0, ID::معلومات_2, k, -1);
		}
		أرجع;
	}

	لكل (صحيح j = 0; j < abits; j++)
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::ADDR, j, ID::معلومات_2, k, -1);
}

فراغ mem_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	إذا (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	وإلا إذا (cell->type.دننه(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	وإلا إذا (cell->type.دننه(ID($memwr), ID($memwr_v2), ID($meminit)))
		أرجع; /* no edges here */
	وإلا
		log_abort();
}

فراغ ff_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح پلنوالی = cell->get_پورټ(ID::Q).اندازه();

	إذا (cell->type.دننه(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		لكل (صحيح k = 0; k < پلنوالی; k++) {
			db->جمع_کړه_څنډه(cell, ID::D, k, ID::Q, k, -1);
			db->جمع_کړه_څنډه(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	إذا (cell->has_پورټ(ID::CLR))
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::CLR, 0, ID::Q, k, -1);
	إذا (cell->has_پورټ(ID::سېټ_2))
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::سېټ_2, 0, ID::Q, k, -1);
	إذا (cell->has_پورټ(ID::ALOAD))
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::ALOAD, 0, ID::Q, k, -1);
	إذا (cell->has_پورټ(ID::AD))
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::AD, k, ID::Q, k, -1);
	إذا (cell->has_پورټ(ID::ARST))
		لكل (صحيح k = 0; k < پلنوالی; k++)
			db->جمع_کړه_څنډه(cell, ID::ARST, 0, ID::Q, k, -1);
}

فراغ ډک_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	std::ویکټور<RTLIL::IdString> ننوت_ports;
	std::ویکټور<RTLIL::IdString> output_ports;

	لكل (خپلکاری &conn : cell->connections())
	{
		RTLIL::IdString پورټ = conn.لومړی;
		RTLIL::پورټ_Dir dir = cell->پورټ_dir(پورټ);
		إذا (cell->ننوت(پورټ) || dir == RTLIL::پورټ_Dir::PD_INOUT)
			ننوت_ports.ټېل_شا(پورټ);
		إذا (cell->output(پورټ) || dir == RTLIL::پورټ_Dir::PD_INOUT)
			output_ports.ټېل_شا(پورټ);
	}

	لكل (خپلکاری بهر_پورټ : output_ports)
	{
		صحيح بهر_پلنوالی = Get_اندازه(cell->get_پورټ(بهر_پورټ));
		لكل (صحيح بهر_بټ = 0; بهر_بټ < بهر_پلنوالی; بهر_بټ++)
		{
			لكل (خپلکاری دننه_پورټ : ننوت_ports)
			{
				صحيح دننه_پلنوالی = Get_اندازه(cell->get_پورټ(دننه_پورټ));
				لكل (صحيح دننه_بټ = 0; دننه_بټ < دننه_پلنوالی; دننه_بټ++)
					db->جمع_کړه_څنډه(cell, دننه_پورټ, دننه_بټ, بهر_پورټ, بهر_بټ, -1);
			}
		}
	}
}

فراغ bweqx_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح max_پلنوالی = std::min(پلنوالی, std::min(a_پلنوالی, b_پلنوالی));

	لكل (صحيح i = 0; i < max_پلنوالی; i++) {
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);
		db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, i, -1);
	}
}

فراغ bwmux_op(Abstract_Cell_څنډې_Database *db, RTLIL::Cell *cell)
{
	صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
	صحيح a_پلنوالی = Get_اندازه(cell->get_پورټ(ID::A));
	صحيح b_پلنوالی = Get_اندازه(cell->get_پورټ(ID::B));
	صحيح s_پلنوالی = Get_اندازه(cell->get_پورټ(ID::S));
	صحيح max_پلنوالی = std::min(پلنوالی, std::min(a_پلنوالی, std::min(b_پلنوالی, s_پلنوالی)));

	لكل (صحيح i = 0; i < max_پلنوالی; i++) {
		db->جمع_کړه_څنډه(cell, ID::A, i, ID::Y, i, -1);
		db->جمع_کړه_څنډه(cell, ID::B, i, ID::Y, i, -1);
		db->جمع_کړه_څنډه(cell, ID::S, i, ID::Y, i, -1);
	}
}

شخصي_NAMESPACE_پای_ټکی

منطقي YOSYS_NAMESPACE_مختاړی Abstract_Cell_څنډې_Database::جمع_کړه_څنډې_from_cell(RTLIL::Cell *cell)
{
	إذا (cell->type.دننه(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_دوګونی_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($neg)) {
		arith_neg_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($جمع_کړه), ID($sub))) {
		arith_دوګونی_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($logic_and), ID($logic_or))) {
		logic_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($ټوټه)) {
		ټوټه_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($concat)) {
		concat_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ښويدنه), ID($shiftx))) {
		ښويدنه_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($mux), ID($pmux))) {
		mux_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($bmux)) {
		bmux_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($demux)) {
		demux_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($bweqx)) {
		bweqx_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($bwmux)) {
		bwmux_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->is_builtin_ff()) {
		ff_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان))) {
		ډک_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		ډک_op(هذا, cell);
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		ډک_op(هذا, cell);
		أرجع ریښتیا;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	إذا (cell->type.دننه(ID($أكد), ID($assume), ID($ژوندی), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		أرجع ریښتیا; // no-op: these have either no inputs or no outputs

	أرجع خطأ;
}
