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
نجی_NAMESPACE_BEGIN

فراغ bitwise_unary_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = !cell->type.وچ(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

	لكل (صحيح i = 0; i < y_چوڑائی; i++)
	{
		إذا (i < a_چوڑائی)
			db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_چوڑائی > 0)
			db->جوڑو_کنارا(cell, ID::A, a_چوڑائی-1, ID::Y, i, -1);
	}
}

فراغ bitwise_ثنائی_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

	إذا (cell->type == ID($and) && !is_signed) {
		إذا (a_چوڑائی > b_چوڑائی)
			a_چوڑائی = b_چوڑائی;
		وإلا
			b_چوڑائی = a_چوڑائی;
	}

	لكل (صحيح i = 0; i < y_چوڑائی; i++)
	{
		إذا (i < a_چوڑائی)
			db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_چوڑائی > 0)
			db->جوڑو_کنارا(cell, ID::A, a_چوڑائی-1, ID::Y, i, -1);

		إذا (i < b_چوڑائی)
			db->جوڑو_کنارا(cell, ID::B, i, ID::Y, i, -1);
		وإلا إذا (is_signed && b_چوڑائی > 0)
			db->جوڑو_کنارا(cell, ID::B, b_چوڑائی-1, ID::Y, i, -1);
	}
}

فراغ arith_neg_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

	إذا (is_signed && a_چوڑائی == 1)
		y_چوڑائی = std::min(y_چوڑائی, 1);

	لكل (صحيح i = 0; i < y_چوڑائی; i++)
	لكل (صحيح k = 0; k <= i && k < a_چوڑائی; k++)
		db->جوڑو_کنارا(cell, ID::A, k, ID::Y, i, -1);
}

فراغ arith_ثنائی_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

	إذا (!is_signed && cell->type != ID($sub)) {
		صحيح ab_چوڑائی = std::max(a_چوڑائی, b_چوڑائی);
		y_چوڑائی = std::min(y_چوڑائی, ab_چوڑائی+1);
	}

	لكل (صحيح i = 0; i < y_چوڑائی; i++)
	{
		لكل (صحيح k = 0; k <= i; k++)
		{
			إذا (k < a_چوڑائی)
				db->جوڑو_کنارا(cell, ID::A, k, ID::Y, i, -1);

			إذا (k < b_چوڑائی)
				db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

فراغ reduce_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));

	لكل (صحيح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, 0, -1);
}

فراغ logic_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));

	لكل (صحيح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, 0, -1);
	لكل (صحيح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ concat_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));

	لكل (صحيح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);
	لكل (صحيح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::B, i, ID::Y, a_چوڑائی + i, -1);
}

فراغ ٹکڑا_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح وتھ = cell->getParam(ID::وتھ_2).as_int();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

	لكل (صحيح i = 0; i < y_چوڑائی; i++) {
		صحيح a_ثنائی_ہندسہ = وتھ + i;
		إذا (a_ثنائی_ہندسہ >= 0 && a_ثنائی_ہندسہ < a_چوڑائی)
			db->جوڑو_کنارا(cell, ID::A, a_ثنائی_ہندسہ, ID::Y, i, -1);
	}
}

فراغ compare_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));

	لكل (صحيح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, 0, -1);

	لكل (صحيح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارا(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ mux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح s_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::S));

	لكل (صحيح i = 0; i < a_چوڑائی; i++)
	{
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);

		لكل (صحيح k = i; k < b_چوڑائی; k += a_چوڑائی)
			db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ bmux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح s_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::S));

	لكل (صحيح i = 0; i < چوڑائی; i++)
	{
		لكل (صحيح k = i; k < a_چوڑائی; k += چوڑائی)
			db->جوڑو_کنارا(cell, ID::A, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ demux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح s_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::S));

	لكل (صحيح i = 0; i < چوڑائی; i++)
	{
		db->جوڑو_کنارا(cell, ID::A, i % a_چوڑائی, ID::Y, i, -1);
		لكل (صحيح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ کھسکاؤ_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقي is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح y_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));

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
	صحيح b_چوڑائی_capped = min(b_چوڑائی, 30);

	صحيح b_بلند, b_کم;
	إذا (!is_b_signed) {
		b_بلند = (1 << b_چوڑائی_capped) - 1;
		b_کم = 0;
	} وإلا {
		b_بلند = (1 << (b_چوڑائی_capped - 1)) - 1;
		b_کم = -(1 << (b_چوڑائی_capped - 1));
	}

	لكل (صحيح i = 0; i < y_چوڑائی; i++){
		// highest position of Y that can change with the value of B
		صحيح b_range_اتلا = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحيح a_range_اتلا;
		// lowest position of A that can be moved to Y[i]
		صحيح a_range_ہیٹھلا;

		إذا (cell->type.وچ(ID($shl), ID($sshl))) {
			b_range_اتلا = a_چوڑائی + b_بلند;
			إذا (is_signed) b_range_اتلا -= 1;
			a_range_ہیٹھلا = max(0, i - b_بلند);
			a_range_اتلا = min(i+1, a_چوڑائی);
		} وإلا إذا (cell->type.وچ(ID($shr), ID($sshr)) || (cell->type.وچ(ID($کھسکاؤ), ID($shiftx)) && !is_b_signed)) {
			b_range_اتلا = a_چوڑائی;
			a_range_ہیٹھلا = min(i, a_چوڑائی - 1);
			a_range_اتلا = min(i+1 + b_بلند, a_چوڑائی);
		} وإلا إذا (cell->type.وچ(ID($کھسکاؤ), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_اتلا = a_چوڑائی - b_کم;
			a_range_ہیٹھلا = max(0, i + b_کم);
			إذا (is_signed)
				a_range_ہیٹھلا = min(a_range_ہیٹھلا, a_چوڑائی - 1);
			a_range_اتلا = min(i+1 + b_بلند, a_چوڑائی);
		} وإلا {
			log_assert(خطأ && "unreachable");
		}

		إذا (i < b_range_اتلا) {
			لكل (صحيح k = a_range_ہیٹھلا; k < a_range_اتلا; k++)
				db->جوڑو_کنارا(cell, ID::A, k, ID::Y, i, -1);
		} وإلا {
			// only influence is through sign extension
			إذا (is_signed)
				db->جوڑو_کنارا(cell, ID::A, a_چوڑائی - 1, ID::Y, i, -1);
		}

		لكل (صحيح k = 0; k < b_چوڑائی_capped; k++) {
			// left shifts
			إذا (cell->type.وچ(ID($shl), ID($sshl))) {
				إذا (a_چوڑائی == 1 && is_signed) {
					صحيح skip = 1 << (k + 1);
					صحيح ادھار = skip -1;
					إذا (i % skip != ادھار && i - a_چوڑائی + 2 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				} وإلا إذا (is_signed) {
					إذا (i - a_چوڑائی + 2 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i - a_چوڑائی + 1 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} وإلا إذا (cell->type.وچ(ID($shr), ID($sshr)) || (cell->type.وچ(ID($کھسکاؤ), ID($shiftx)) && !is_b_signed)) {
				إذا (is_signed) {
					منطقي کھسکاؤ_وچ_bulk = i < a_چوڑائی - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقي zpad_jump = (((y_چوڑائی - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_چوڑائی - i) & ~(1 << k)) < (1 << b_چوڑائی_capped)));

					إذا (کھسکاؤ_وچ_bulk || (cell->type.وچ(ID($shr), ID($کھسکاؤ), ID($shiftx)) && zpad_jump))
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i < a_چوڑائی)
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} وإلا إذا (cell->type.وچ(ID($کھسکاؤ), ID($shiftx)) && is_b_signed) {
				إذا (is_signed) {
					إذا (k != b_چوڑائی_capped - 1) {
						منطقي r_کھسکاؤ_وچ_bulk = i < a_چوڑائی - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقي r_zpad_jump = (((y_چوڑائی - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_چوڑائی - i) & ~(1 << k)) <= b_بلند));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقي l = a_چوڑائی - 2 - i >= b_کم;
						إذا (a_چوڑائی == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_چوڑائی) & ((1 << (k + 1)) - 1)) != 0;
						}
						إذا (r_کھسکاؤ_وچ_bulk || r_zpad_jump || l)
							db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
					} وإلا {
						إذا (y_چوڑائی - i <= b_بلند || a_چوڑائی - 2 - i >= b_کم)
							db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
					}
				} وإلا {
					إذا (a_چوڑائی - 1 - i >= b_کم)
						db->جوڑو_کنارا(cell, ID::B, k, ID::Y, i, -1);
				}
			} وإلا {
				log_assert(خطأ && "unreachable");
			}
		}
	}
}

فراغ packed_mem_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحيح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();

	لكل (صحيح i = 0; i < n_rd_ports; i++) {
		إذا (rd_clk_enable[i] != State::S0) {
			لكل (صحيح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارا(cell, ID::RD_ARST, i, ID::RD_انکڑے, i * چوڑائی + k, -1);
			استمر;
		}

		لكل (صحيح j = 0; j < abits; j++)
			لكل (صحيح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارا(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_انکڑے, i * چوڑائی + k, -1);
	}
}

فراغ memrd_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.وچ(ID($memrd), ID($memrd_v2)));
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();

	إذا (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		إذا (cell->type == ID($memrd_v2)) {
			لكل (صحيح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارا(cell, ID::ARST, 0, ID::انکڑے_2, k, -1);
		}
		أرجع;
	}

	لكل (صحيح j = 0; j < abits; j++)
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::ADDR, j, ID::انکڑے_2, k, -1);
}

فراغ mem_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	إذا (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	وإلا إذا (cell->type.وچ(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	وإلا إذا (cell->type.وچ(ID($memwr), ID($memwr_v2), ID($meminit)))
		أرجع; /* no edges here */
	وإلا
		log_abort();
}

فراغ ff_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح چوڑائی = cell->get_پورٹ(ID::Q).ناپ();

	إذا (cell->type.وچ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		لكل (صحيح k = 0; k < چوڑائی; k++) {
			db->جوڑو_کنارا(cell, ID::D, k, ID::Q, k, -1);
			db->جوڑو_کنارا(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	إذا (cell->has_پورٹ(ID::CLR))
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::CLR, 0, ID::Q, k, -1);
	إذا (cell->has_پورٹ(ID::سیٹ_2))
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::سیٹ_2, 0, ID::Q, k, -1);
	إذا (cell->has_پورٹ(ID::ALOAD))
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::ALOAD, 0, ID::Q, k, -1);
	إذا (cell->has_پورٹ(ID::AD))
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::AD, k, ID::Q, k, -1);
	إذا (cell->has_پورٹ(ID::ARST))
		لكل (صحيح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارا(cell, ID::ARST, 0, ID::Q, k, -1);
}

فراغ بھریا_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	std::سمتیہ<RTLIL::IdString> داخلہ_ports;
	std::سمتیہ<RTLIL::IdString> output_ports;

	لكل (خودکار &conn : cell->connections())
	{
		RTLIL::IdString پورٹ = conn.پہلا;
		RTLIL::پورٹ_Dir dir = cell->پورٹ_dir(پورٹ);
		إذا (cell->داخلہ(پورٹ) || dir == RTLIL::پورٹ_Dir::PD_INOUT)
			داخلہ_ports.ڈالو_پٹھ(پورٹ);
		إذا (cell->output(پورٹ) || dir == RTLIL::پورٹ_Dir::PD_INOUT)
			output_ports.ڈالو_پٹھ(پورٹ);
	}

	لكل (خودکار باہرلا_پورٹ : output_ports)
	{
		صحيح باہرلا_چوڑائی = Get_ناپ(cell->get_پورٹ(باہرلا_پورٹ));
		لكل (صحيح باہرلا_ثنائی_ہندسہ = 0; باہرلا_ثنائی_ہندسہ < باہرلا_چوڑائی; باہرلا_ثنائی_ہندسہ++)
		{
			لكل (خودکار وچ_پورٹ : داخلہ_ports)
			{
				صحيح وچ_چوڑائی = Get_ناپ(cell->get_پورٹ(وچ_پورٹ));
				لكل (صحيح وچ_ثنائی_ہندسہ = 0; وچ_ثنائی_ہندسہ < وچ_چوڑائی; وچ_ثنائی_ہندسہ++)
					db->جوڑو_کنارا(cell, وچ_پورٹ, وچ_ثنائی_ہندسہ, باہرلا_پورٹ, باہرلا_ثنائی_ہندسہ, -1);
			}
		}
	}
}

فراغ bweqx_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح max_چوڑائی = std::min(چوڑائی, std::min(a_چوڑائی, b_چوڑائی));

	لكل (صحيح i = 0; i < max_چوڑائی; i++) {
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);
		db->جوڑو_کنارا(cell, ID::B, i, ID::Y, i, -1);
	}
}

فراغ bwmux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
	صحيح a_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::A));
	صحيح b_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::B));
	صحيح s_چوڑائی = Get_ناپ(cell->get_پورٹ(ID::S));
	صحيح max_چوڑائی = std::min(چوڑائی, std::min(a_چوڑائی, std::min(b_چوڑائی, s_چوڑائی)));

	لكل (صحيح i = 0; i < max_چوڑائی; i++) {
		db->جوڑو_کنارا(cell, ID::A, i, ID::Y, i, -1);
		db->جوڑو_کنارا(cell, ID::B, i, ID::Y, i, -1);
		db->جوڑو_کنارا(cell, ID::S, i, ID::Y, i, -1);
	}
}

نجی_NAMESPACE_سرا

منطقي YOSYS_NAMESPACE_اگیتر Abstract_Cell_کنارے_Database::جوڑو_کنارے_from_cell(RTLIL::Cell *cell)
{
	إذا (cell->type.وچ(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ثنائی_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($neg)) {
		arith_neg_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($جوڑو), ID($sub))) {
		arith_ثنائی_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($logic_and), ID($logic_or))) {
		logic_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($ٹکڑا)) {
		ٹکڑا_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($concat)) {
		concat_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($کھسکاؤ), ID($shiftx))) {
		کھسکاؤ_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($mux), ID($pmux))) {
		mux_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($bmux)) {
		bmux_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($demux)) {
		demux_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($bweqx)) {
		bweqx_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($bwmux)) {
		bwmux_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->is_builtin_ff()) {
		ff_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
		بھریا_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		بھریا_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.وچ(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		بھریا_op(هذا, cell);
		أرجع سچ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	إذا (cell->type.وچ(ID($أكد), ID($assume), ID($جیوندا), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		أرجع سچ; // no-op: these have either no inputs or no outputs

	أرجع خطأ;
}
