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

#شامل "kernel/celledges.h"

USING_YOSYS_NAMESPACE
نجی_NAMESPACE_BEGIN

خالی bitwise_unary_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقی is_signed = !cell->type.اندر(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

	برائے (صحیح i = 0; i < y_چوڑائی; i++)
	{
		اگر (i < a_چوڑائی)
			db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);
		ورنہ اگر (is_signed && a_چوڑائی > 0)
			db->جوڑو_کنارہ(cell, ID::A, a_چوڑائی-1, ID::Y, i, -1);
	}
}

خالی bitwise_ثنائی_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

	اگر (cell->type == ID($and) && !is_signed) {
		اگر (a_چوڑائی > b_چوڑائی)
			a_چوڑائی = b_چوڑائی;
		ورنہ
			b_چوڑائی = a_چوڑائی;
	}

	برائے (صحیح i = 0; i < y_چوڑائی; i++)
	{
		اگر (i < a_چوڑائی)
			db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);
		ورنہ اگر (is_signed && a_چوڑائی > 0)
			db->جوڑو_کنارہ(cell, ID::A, a_چوڑائی-1, ID::Y, i, -1);

		اگر (i < b_چوڑائی)
			db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, i, -1);
		ورنہ اگر (is_signed && b_چوڑائی > 0)
			db->جوڑو_کنارہ(cell, ID::B, b_چوڑائی-1, ID::Y, i, -1);
	}
}

خالی arith_neg_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

	اگر (is_signed && a_چوڑائی == 1)
		y_چوڑائی = std::min(y_چوڑائی, 1);

	برائے (صحیح i = 0; i < y_چوڑائی; i++)
	برائے (صحیح k = 0; k <= i && k < a_چوڑائی; k++)
		db->جوڑو_کنارہ(cell, ID::A, k, ID::Y, i, -1);
}

خالی arith_ثنائی_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

	اگر (!is_signed && cell->type != ID($sub)) {
		صحیح ab_چوڑائی = std::max(a_چوڑائی, b_چوڑائی);
		y_چوڑائی = std::min(y_چوڑائی, ab_چوڑائی+1);
	}

	برائے (صحیح i = 0; i < y_چوڑائی; i++)
	{
		برائے (صحیح k = 0; k <= i; k++)
		{
			اگر (k < a_چوڑائی)
				db->جوڑو_کنارہ(cell, ID::A, k, ID::Y, i, -1);

			اگر (k < b_چوڑائی)
				db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

خالی reduce_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));

	برائے (صحیح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, 0, -1);
}

خالی logic_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));

	برائے (صحیح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, 0, -1);
	برائے (صحیح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, 0, -1);
}

خالی concat_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));

	برائے (صحیح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);
	برائے (صحیح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, a_چوڑائی + i, -1);
}

خالی قاش_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح انحراف = cell->getParam(ID::انحراف_2).as_int();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

	برائے (صحیح i = 0; i < y_چوڑائی; i++) {
		صحیح a_ثنائی_ہندسہ = انحراف + i;
		اگر (a_ثنائی_ہندسہ >= 0 && a_ثنائی_ہندسہ < a_چوڑائی)
			db->جوڑو_کنارہ(cell, ID::A, a_ثنائی_ہندسہ, ID::Y, i, -1);
	}
}

خالی compare_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));

	برائے (صحیح i = 0; i < a_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, 0, -1);

	برائے (صحیح i = 0; i < b_چوڑائی; i++)
		db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, 0, -1);
}

خالی mux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح s_چوڑائی = Get_سائز(cell->get_پورٹ(ID::S));

	برائے (صحیح i = 0; i < a_چوڑائی; i++)
	{
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);

		برائے (صحیح k = i; k < b_چوڑائی; k += a_چوڑائی)
			db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);

		برائے (صحیح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::S, k, ID::Y, i, -1);
	}
}

خالی bmux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح s_چوڑائی = Get_سائز(cell->get_پورٹ(ID::S));

	برائے (صحیح i = 0; i < چوڑائی; i++)
	{
		برائے (صحیح k = i; k < a_چوڑائی; k += چوڑائی)
			db->جوڑو_کنارہ(cell, ID::A, k, ID::Y, i, -1);

		برائے (صحیح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::S, k, ID::Y, i, -1);
	}
}

خالی demux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح s_چوڑائی = Get_سائز(cell->get_پورٹ(ID::S));

	برائے (صحیح i = 0; i < چوڑائی; i++)
	{
		db->جوڑو_کنارہ(cell, ID::A, i % a_چوڑائی, ID::Y, i, -1);
		برائے (صحیح k = 0; k < s_چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::S, k, ID::Y, i, -1);
	}
}

خالی سرکاؤ_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقی is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح y_چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));

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
	صحیح b_چوڑائی_capped = min(b_چوڑائی, 30);

	صحیح b_بلند, b_کم;
	اگر (!is_b_signed) {
		b_بلند = (1 << b_چوڑائی_capped) - 1;
		b_کم = 0;
	} ورنہ {
		b_بلند = (1 << (b_چوڑائی_capped - 1)) - 1;
		b_کم = -(1 << (b_چوڑائی_capped - 1));
	}

	برائے (صحیح i = 0; i < y_چوڑائی; i++){
		// highest position of Y that can change with the value of B
		صحیح b_range_بالائی = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحیح a_range_بالائی;
		// lowest position of A that can be moved to Y[i]
		صحیح a_range_زیریں;

		اگر (cell->type.اندر(ID($shl), ID($sshl))) {
			b_range_بالائی = a_چوڑائی + b_بلند;
			اگر (is_signed) b_range_بالائی -= 1;
			a_range_زیریں = max(0, i - b_بلند);
			a_range_بالائی = min(i+1, a_چوڑائی);
		} ورنہ اگر (cell->type.اندر(ID($shr), ID($sshr)) || (cell->type.اندر(ID($سرکاؤ), ID($shiftx)) && !is_b_signed)) {
			b_range_بالائی = a_چوڑائی;
			a_range_زیریں = min(i, a_چوڑائی - 1);
			a_range_بالائی = min(i+1 + b_بلند, a_چوڑائی);
		} ورنہ اگر (cell->type.اندر(ID($سرکاؤ), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_بالائی = a_چوڑائی - b_کم;
			a_range_زیریں = max(0, i + b_کم);
			اگر (is_signed)
				a_range_زیریں = min(a_range_زیریں, a_چوڑائی - 1);
			a_range_بالائی = min(i+1 + b_بلند, a_چوڑائی);
		} ورنہ {
			log_assert(جھوٹ && "unreachable");
		}

		اگر (i < b_range_بالائی) {
			برائے (صحیح k = a_range_زیریں; k < a_range_بالائی; k++)
				db->جوڑو_کنارہ(cell, ID::A, k, ID::Y, i, -1);
		} ورنہ {
			// only influence is through sign extension
			اگر (is_signed)
				db->جوڑو_کنارہ(cell, ID::A, a_چوڑائی - 1, ID::Y, i, -1);
		}

		برائے (صحیح k = 0; k < b_چوڑائی_capped; k++) {
			// left shifts
			اگر (cell->type.اندر(ID($shl), ID($sshl))) {
				اگر (a_چوڑائی == 1 && is_signed) {
					صحیح skip = 1 << (k + 1);
					صحیح بنیاد = skip -1;
					اگر (i % skip != بنیاد && i - a_چوڑائی + 2 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				} ورنہ اگر (is_signed) {
					اگر (i - a_چوڑائی + 2 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				} ورنہ {
					اگر (i - a_چوڑائی + 1 < 1 << b_چوڑائی_capped)
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} ورنہ اگر (cell->type.اندر(ID($shr), ID($sshr)) || (cell->type.اندر(ID($سرکاؤ), ID($shiftx)) && !is_b_signed)) {
				اگر (is_signed) {
					منطقی سرکاؤ_اندر_bulk = i < a_چوڑائی - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقی zpad_jump = (((y_چوڑائی - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_چوڑائی - i) & ~(1 << k)) < (1 << b_چوڑائی_capped)));

					اگر (سرکاؤ_اندر_bulk || (cell->type.اندر(ID($shr), ID($سرکاؤ), ID($shiftx)) && zpad_jump))
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				} ورنہ {
					اگر (i < a_چوڑائی)
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} ورنہ اگر (cell->type.اندر(ID($سرکاؤ), ID($shiftx)) && is_b_signed) {
				اگر (is_signed) {
					اگر (k != b_چوڑائی_capped - 1) {
						منطقی r_سرکاؤ_اندر_bulk = i < a_چوڑائی - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقی r_zpad_jump = (((y_چوڑائی - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_چوڑائی - i) & ~(1 << k)) <= b_بلند));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقی l = a_چوڑائی - 2 - i >= b_کم;
						اگر (a_چوڑائی == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_چوڑائی) & ((1 << (k + 1)) - 1)) != 0;
						}
						اگر (r_سرکاؤ_اندر_bulk || r_zpad_jump || l)
							db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
					} ورنہ {
						اگر (y_چوڑائی - i <= b_بلند || a_چوڑائی - 2 - i >= b_کم)
							db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
					}
				} ورنہ {
					اگر (a_چوڑائی - 1 - i >= b_کم)
						db->جوڑو_کنارہ(cell, ID::B, k, ID::Y, i, -1);
				}
			} ورنہ {
				log_assert(جھوٹ && "unreachable");
			}
		}
	}
}

خالی packed_mem_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحیح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحیح abits = cell->getParam(ID::ABITS).as_int();
	صحیح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();

	برائے (صحیح i = 0; i < n_rd_ports; i++) {
		اگر (rd_clk_enable[i] != State::S0) {
			برائے (صحیح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارہ(cell, ID::RD_ARST, i, ID::RD_ڈیٹا, i * چوڑائی + k, -1);
			جاری;
		}

		برائے (صحیح j = 0; j < abits; j++)
			برائے (صحیح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارہ(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_ڈیٹا, i * چوڑائی + k, -1);
	}
}

خالی memrd_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.اندر(ID($memrd), ID($memrd_v2)));
	صحیح abits = cell->getParam(ID::ABITS).as_int();
	صحیح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();

	اگر (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		اگر (cell->type == ID($memrd_v2)) {
			برائے (صحیح k = 0; k < چوڑائی; k++)
				db->جوڑو_کنارہ(cell, ID::ARST, 0, ID::ڈیٹا_2, k, -1);
		}
		واپس;
	}

	برائے (صحیح j = 0; j < abits; j++)
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::ADDR, j, ID::ڈیٹا_2, k, -1);
}

خالی mem_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	اگر (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	ورنہ اگر (cell->type.اندر(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	ورنہ اگر (cell->type.اندر(ID($memwr), ID($memwr_v2), ID($meminit)))
		واپس; /* no edges here */
	ورنہ
		log_abort();
}

خالی ff_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح چوڑائی = cell->get_پورٹ(ID::Q).سائز();

	اگر (cell->type.اندر(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		برائے (صحیح k = 0; k < چوڑائی; k++) {
			db->جوڑو_کنارہ(cell, ID::D, k, ID::Q, k, -1);
			db->جوڑو_کنارہ(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	اگر (cell->has_پورٹ(ID::CLR))
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::CLR, 0, ID::Q, k, -1);
	اگر (cell->has_پورٹ(ID::سیٹ_2))
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::سیٹ_2, 0, ID::Q, k, -1);
	اگر (cell->has_پورٹ(ID::ALOAD))
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::ALOAD, 0, ID::Q, k, -1);
	اگر (cell->has_پورٹ(ID::AD))
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::AD, k, ID::Q, k, -1);
	اگر (cell->has_پورٹ(ID::ARST))
		برائے (صحیح k = 0; k < چوڑائی; k++)
			db->جوڑو_کنارہ(cell, ID::ARST, 0, ID::Q, k, -1);
}

خالی بھرا_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	std::سمتیہ<RTLIL::IdString> داخلہ_ports;
	std::سمتیہ<RTLIL::IdString> output_ports;

	برائے (خودکار &conn : cell->connections())
	{
		RTLIL::IdString پورٹ = conn.پہلا;
		RTLIL::پورٹ_Dir dir = cell->پورٹ_dir(پورٹ);
		اگر (cell->داخلہ(پورٹ) || dir == RTLIL::پورٹ_Dir::PD_INOUT)
			داخلہ_ports.ڈالو_عقب(پورٹ);
		اگر (cell->output(پورٹ) || dir == RTLIL::پورٹ_Dir::PD_INOUT)
			output_ports.ڈالو_عقب(پورٹ);
	}

	برائے (خودکار برآمد_پورٹ : output_ports)
	{
		صحیح برآمد_چوڑائی = Get_سائز(cell->get_پورٹ(برآمد_پورٹ));
		برائے (صحیح برآمد_ثنائی_ہندسہ = 0; برآمد_ثنائی_ہندسہ < برآمد_چوڑائی; برآمد_ثنائی_ہندسہ++)
		{
			برائے (خودکار اندر_پورٹ : داخلہ_ports)
			{
				صحیح اندر_چوڑائی = Get_سائز(cell->get_پورٹ(اندر_پورٹ));
				برائے (صحیح اندر_ثنائی_ہندسہ = 0; اندر_ثنائی_ہندسہ < اندر_چوڑائی; اندر_ثنائی_ہندسہ++)
					db->جوڑو_کنارہ(cell, اندر_پورٹ, اندر_ثنائی_ہندسہ, برآمد_پورٹ, برآمد_ثنائی_ہندسہ, -1);
			}
		}
	}
}

خالی bweqx_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح max_چوڑائی = std::min(چوڑائی, std::min(a_چوڑائی, b_چوڑائی));

	برائے (صحیح i = 0; i < max_چوڑائی; i++) {
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);
		db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, i, -1);
	}
}

خالی bwmux_op(Abstract_Cell_کنارے_Database *db, RTLIL::Cell *cell)
{
	صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
	صحیح a_چوڑائی = Get_سائز(cell->get_پورٹ(ID::A));
	صحیح b_چوڑائی = Get_سائز(cell->get_پورٹ(ID::B));
	صحیح s_چوڑائی = Get_سائز(cell->get_پورٹ(ID::S));
	صحیح max_چوڑائی = std::min(چوڑائی, std::min(a_چوڑائی, std::min(b_چوڑائی, s_چوڑائی)));

	برائے (صحیح i = 0; i < max_چوڑائی; i++) {
		db->جوڑو_کنارہ(cell, ID::A, i, ID::Y, i, -1);
		db->جوڑو_کنارہ(cell, ID::B, i, ID::Y, i, -1);
		db->جوڑو_کنارہ(cell, ID::S, i, ID::Y, i, -1);
	}
}

نجی_NAMESPACE_انتہا

منطقی YOSYS_NAMESPACE_سابقہ Abstract_Cell_کنارے_Database::جوڑو_کنارے_from_cell(RTLIL::Cell *cell)
{
	اگر (cell->type.اندر(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ثنائی_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($neg)) {
		arith_neg_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($جوڑو), ID($sub))) {
		arith_ثنائی_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($logic_and), ID($logic_or))) {
		logic_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($قاش)) {
		قاش_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($concat)) {
		concat_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرکاؤ), ID($shiftx))) {
		سرکاؤ_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($mux), ID($pmux))) {
		mux_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($bmux)) {
		bmux_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($demux)) {
		demux_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($bweqx)) {
		bweqx_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type == ID($bwmux)) {
		bwmux_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->is_builtin_ff()) {
		ff_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
		بھرا_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		بھرا_op(یہ, cell);
		واپس سچ;
	}

	اگر (cell->type.اندر(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		بھرا_op(یہ, cell);
		واپس سچ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	اگر (cell->type.اندر(ID($أكد), ID($assume), ID($زندہ), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		واپس سچ; // no-op: these have either no inputs or no outputs

	واپس جھوٹ;
}
