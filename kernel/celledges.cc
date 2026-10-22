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
خصوصی_NAMESPACE_BEGIN

فراغ bitwise_unary_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = !cell->type.درون(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

	لكل (صحيح i = 0; i < y_عرض; i++)
	{
		إذا (i < a_عرض)
			db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_عرض > 0)
			db->جمع_کن_یال(cell, ID::A, a_عرض-1, ID::Y, i, -1);
	}
}

فراغ bitwise_دودویی_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

	إذا (cell->type == ID($and) && !is_signed) {
		إذا (a_عرض > b_عرض)
			a_عرض = b_عرض;
		وإلا
			b_عرض = a_عرض;
	}

	لكل (صحيح i = 0; i < y_عرض; i++)
	{
		إذا (i < a_عرض)
			db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_عرض > 0)
			db->جمع_کن_یال(cell, ID::A, a_عرض-1, ID::Y, i, -1);

		إذا (i < b_عرض)
			db->جمع_کن_یال(cell, ID::B, i, ID::Y, i, -1);
		وإلا إذا (is_signed && b_عرض > 0)
			db->جمع_کن_یال(cell, ID::B, b_عرض-1, ID::Y, i, -1);
	}
}

فراغ arith_neg_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

	إذا (is_signed && a_عرض == 1)
		y_عرض = std::min(y_عرض, 1);

	لكل (صحيح i = 0; i < y_عرض; i++)
	لكل (صحيح k = 0; k <= i && k < a_عرض; k++)
		db->جمع_کن_یال(cell, ID::A, k, ID::Y, i, -1);
}

فراغ arith_دودویی_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

	إذا (!is_signed && cell->type != ID($sub)) {
		صحيح ab_عرض = std::max(a_عرض, b_عرض);
		y_عرض = std::min(y_عرض, ab_عرض+1);
	}

	لكل (صحيح i = 0; i < y_عرض; i++)
	{
		لكل (صحيح k = 0; k <= i; k++)
		{
			إذا (k < a_عرض)
				db->جمع_کن_یال(cell, ID::A, k, ID::Y, i, -1);

			إذا (k < b_عرض)
				db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

فراغ reduce_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));

	لكل (صحيح i = 0; i < a_عرض; i++)
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, 0, -1);
}

فراغ logic_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));

	لكل (صحيح i = 0; i < a_عرض; i++)
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, 0, -1);
	لكل (صحيح i = 0; i < b_عرض; i++)
		db->جمع_کن_یال(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ concat_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));

	لكل (صحيح i = 0; i < a_عرض; i++)
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);
	لكل (صحيح i = 0; i < b_عرض; i++)
		db->جمع_کن_یال(cell, ID::B, i, ID::Y, a_عرض + i, -1);
}

فراغ برش_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح جابه_جایی = cell->getParam(ID::جابه_جایی_2).as_int();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

	لكل (صحيح i = 0; i < y_عرض; i++) {
		صحيح a_بیت = جابه_جایی + i;
		إذا (a_بیت >= 0 && a_بیت < a_عرض)
			db->جمع_کن_یال(cell, ID::A, a_بیت, ID::Y, i, -1);
	}
}

فراغ compare_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));

	لكل (صحيح i = 0; i < a_عرض; i++)
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, 0, -1);

	لكل (صحيح i = 0; i < b_عرض; i++)
		db->جمع_کن_یال(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ mux_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح s_عرض = Get_اندازه(cell->get_پورت(ID::S));

	لكل (صحيح i = 0; i < a_عرض; i++)
	{
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);

		لكل (صحيح k = i; k < b_عرض; k += a_عرض)
			db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_عرض; k++)
			db->جمع_کن_یال(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ bmux_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح s_عرض = Get_اندازه(cell->get_پورت(ID::S));

	لكل (صحيح i = 0; i < عرض; i++)
	{
		لكل (صحيح k = i; k < a_عرض; k += عرض)
			db->جمع_کن_یال(cell, ID::A, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_عرض; k++)
			db->جمع_کن_یال(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ demux_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح s_عرض = Get_اندازه(cell->get_پورت(ID::S));

	لكل (صحيح i = 0; i < عرض; i++)
	{
		db->جمع_کن_یال(cell, ID::A, i % a_عرض, ID::Y, i, -1);
		لكل (صحيح k = 0; k < s_عرض; k++)
			db->جمع_کن_یال(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ لغزش_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقي is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح y_عرض = Get_اندازه(cell->get_پورت(ID::Y));

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
	صحيح b_عرض_capped = min(b_عرض, 30);

	صحيح b_بالا, b_کم;
	إذا (!is_b_signed) {
		b_بالا = (1 << b_عرض_capped) - 1;
		b_کم = 0;
	} وإلا {
		b_بالا = (1 << (b_عرض_capped - 1)) - 1;
		b_کم = -(1 << (b_عرض_capped - 1));
	}

	لكل (صحيح i = 0; i < y_عرض; i++){
		// highest position of Y that can change with the value of B
		صحيح b_range_فوقانی = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحيح a_range_فوقانی;
		// lowest position of A that can be moved to Y[i]
		صحيح a_range_پایینی;

		إذا (cell->type.درون(ID($shl), ID($sshl))) {
			b_range_فوقانی = a_عرض + b_بالا;
			إذا (is_signed) b_range_فوقانی -= 1;
			a_range_پایینی = max(0, i - b_بالا);
			a_range_فوقانی = min(i+1, a_عرض);
		} وإلا إذا (cell->type.درون(ID($shr), ID($sshr)) || (cell->type.درون(ID($لغزش), ID($shiftx)) && !is_b_signed)) {
			b_range_فوقانی = a_عرض;
			a_range_پایینی = min(i, a_عرض - 1);
			a_range_فوقانی = min(i+1 + b_بالا, a_عرض);
		} وإلا إذا (cell->type.درون(ID($لغزش), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_فوقانی = a_عرض - b_کم;
			a_range_پایینی = max(0, i + b_کم);
			إذا (is_signed)
				a_range_پایینی = min(a_range_پایینی, a_عرض - 1);
			a_range_فوقانی = min(i+1 + b_بالا, a_عرض);
		} وإلا {
			log_assert(خطأ && "unreachable");
		}

		إذا (i < b_range_فوقانی) {
			لكل (صحيح k = a_range_پایینی; k < a_range_فوقانی; k++)
				db->جمع_کن_یال(cell, ID::A, k, ID::Y, i, -1);
		} وإلا {
			// only influence is through sign extension
			إذا (is_signed)
				db->جمع_کن_یال(cell, ID::A, a_عرض - 1, ID::Y, i, -1);
		}

		لكل (صحيح k = 0; k < b_عرض_capped; k++) {
			// left shifts
			إذا (cell->type.درون(ID($shl), ID($sshl))) {
				إذا (a_عرض == 1 && is_signed) {
					صحيح skip = 1 << (k + 1);
					صحيح پایه = skip -1;
					إذا (i % skip != پایه && i - a_عرض + 2 < 1 << b_عرض_capped)
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				} وإلا إذا (is_signed) {
					إذا (i - a_عرض + 2 < 1 << b_عرض_capped)
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i - a_عرض + 1 < 1 << b_عرض_capped)
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} وإلا إذا (cell->type.درون(ID($shr), ID($sshr)) || (cell->type.درون(ID($لغزش), ID($shiftx)) && !is_b_signed)) {
				إذا (is_signed) {
					منطقي لغزش_درون_bulk = i < a_عرض - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقي zpad_jump = (((y_عرض - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_عرض - i) & ~(1 << k)) < (1 << b_عرض_capped)));

					إذا (لغزش_درون_bulk || (cell->type.درون(ID($shr), ID($لغزش), ID($shiftx)) && zpad_jump))
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i < a_عرض)
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} وإلا إذا (cell->type.درون(ID($لغزش), ID($shiftx)) && is_b_signed) {
				إذا (is_signed) {
					إذا (k != b_عرض_capped - 1) {
						منطقي r_لغزش_درون_bulk = i < a_عرض - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقي r_zpad_jump = (((y_عرض - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_عرض - i) & ~(1 << k)) <= b_بالا));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقي l = a_عرض - 2 - i >= b_کم;
						إذا (a_عرض == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_عرض) & ((1 << (k + 1)) - 1)) != 0;
						}
						إذا (r_لغزش_درون_bulk || r_zpad_jump || l)
							db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
					} وإلا {
						إذا (y_عرض - i <= b_بالا || a_عرض - 2 - i >= b_کم)
							db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
					}
				} وإلا {
					إذا (a_عرض - 1 - i >= b_کم)
						db->جمع_کن_یال(cell, ID::B, k, ID::Y, i, -1);
				}
			} وإلا {
				log_assert(خطأ && "unreachable");
			}
		}
	}
}

فراغ packed_mem_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحيح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح عرض = cell->getParam(ID::عرض_2).as_int();

	لكل (صحيح i = 0; i < n_rd_ports; i++) {
		إذا (rd_clk_enable[i] != State::S0) {
			لكل (صحيح k = 0; k < عرض; k++)
				db->جمع_کن_یال(cell, ID::RD_ARST, i, ID::RD_داده, i * عرض + k, -1);
			استمر;
		}

		لكل (صحيح j = 0; j < abits; j++)
			لكل (صحيح k = 0; k < عرض; k++)
				db->جمع_کن_یال(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_داده, i * عرض + k, -1);
	}
}

فراغ memrd_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.درون(ID($memrd), ID($memrd_v2)));
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح عرض = cell->getParam(ID::عرض_2).as_int();

	إذا (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		إذا (cell->type == ID($memrd_v2)) {
			لكل (صحيح k = 0; k < عرض; k++)
				db->جمع_کن_یال(cell, ID::ARST, 0, ID::داده_2, k, -1);
		}
		أرجع;
	}

	لكل (صحيح j = 0; j < abits; j++)
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::ADDR, j, ID::داده_2, k, -1);
}

فراغ mem_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	إذا (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	وإلا إذا (cell->type.درون(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	وإلا إذا (cell->type.درون(ID($memwr), ID($memwr_v2), ID($meminit)))
		أرجع; /* no edges here */
	وإلا
		log_abort();
}

فراغ ff_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح عرض = cell->get_پورت(ID::Q).اندازه();

	إذا (cell->type.درون(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		لكل (صحيح k = 0; k < عرض; k++) {
			db->جمع_کن_یال(cell, ID::D, k, ID::Q, k, -1);
			db->جمع_کن_یال(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	إذا (cell->has_پورت(ID::CLR))
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::CLR, 0, ID::Q, k, -1);
	إذا (cell->has_پورت(ID::مجموعه_2))
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::مجموعه_2, 0, ID::Q, k, -1);
	إذا (cell->has_پورت(ID::ALOAD))
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::ALOAD, 0, ID::Q, k, -1);
	إذا (cell->has_پورت(ID::AD))
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::AD, k, ID::Q, k, -1);
	إذا (cell->has_پورت(ID::ARST))
		لكل (صحيح k = 0; k < عرض; k++)
			db->جمع_کن_یال(cell, ID::ARST, 0, ID::Q, k, -1);
}

فراغ پر_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	std::وکتور<RTLIL::IdString> درون_داد_ports;
	std::وکتور<RTLIL::IdString> output_ports;

	لكل (خودکار &conn : cell->connections())
	{
		RTLIL::IdString پورت = conn.اولی;
		RTLIL::پورت_Dir dir = cell->پورت_dir(پورت);
		إذا (cell->درون_داد(پورت) || dir == RTLIL::پورت_Dir::PD_INOUT)
			درون_داد_ports.افزودن_پشت(پورت);
		إذا (cell->output(پورت) || dir == RTLIL::پورت_Dir::PD_INOUT)
			output_ports.افزودن_پشت(پورت);
	}

	لكل (خودکار بیرون_پورت : output_ports)
	{
		صحيح بیرون_عرض = Get_اندازه(cell->get_پورت(بیرون_پورت));
		لكل (صحيح بیرون_بیت = 0; بیرون_بیت < بیرون_عرض; بیرون_بیت++)
		{
			لكل (خودکار درون_پورت : درون_داد_ports)
			{
				صحيح درون_عرض = Get_اندازه(cell->get_پورت(درون_پورت));
				لكل (صحيح درون_بیت = 0; درون_بیت < درون_عرض; درون_بیت++)
					db->جمع_کن_یال(cell, درون_پورت, درون_بیت, بیرون_پورت, بیرون_بیت, -1);
			}
		}
	}
}

فراغ bweqx_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح max_عرض = std::min(عرض, std::min(a_عرض, b_عرض));

	لكل (صحيح i = 0; i < max_عرض; i++) {
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);
		db->جمع_کن_یال(cell, ID::B, i, ID::Y, i, -1);
	}
}

فراغ bwmux_op(Abstract_Cell_یالها_Database *db, RTLIL::Cell *cell)
{
	صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
	صحيح a_عرض = Get_اندازه(cell->get_پورت(ID::A));
	صحيح b_عرض = Get_اندازه(cell->get_پورت(ID::B));
	صحيح s_عرض = Get_اندازه(cell->get_پورت(ID::S));
	صحيح max_عرض = std::min(عرض, std::min(a_عرض, std::min(b_عرض, s_عرض)));

	لكل (صحيح i = 0; i < max_عرض; i++) {
		db->جمع_کن_یال(cell, ID::A, i, ID::Y, i, -1);
		db->جمع_کن_یال(cell, ID::B, i, ID::Y, i, -1);
		db->جمع_کن_یال(cell, ID::S, i, ID::Y, i, -1);
	}
}

خصوصی_NAMESPACE_انتها

منطقي YOSYS_NAMESPACE_پیشوند Abstract_Cell_یالها_Database::جمع_کن_یالها_from_cell(RTLIL::Cell *cell)
{
	إذا (cell->type.درون(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_دودویی_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($neg)) {
		arith_neg_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($جمع_کن), ID($sub))) {
		arith_دودویی_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($logic_and), ID($logic_or))) {
		logic_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($برش)) {
		برش_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($concat)) {
		concat_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($لغزش), ID($shiftx))) {
		لغزش_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($mux), ID($pmux))) {
		mux_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($bmux)) {
		bmux_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($demux)) {
		demux_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($bweqx)) {
		bweqx_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type == ID($bwmux)) {
		bwmux_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->is_builtin_ff()) {
		ff_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان))) {
		پر_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		پر_op(هذا, cell);
		أرجع درست;
	}

	إذا (cell->type.درون(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		پر_op(هذا, cell);
		أرجع درست;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	إذا (cell->type.درون(ID($أكد), ID($assume), ID($زنده), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		أرجع درست; // no-op: these have either no inputs or no outputs

	أرجع خطأ;
}
