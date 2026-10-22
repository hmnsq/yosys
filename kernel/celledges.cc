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
ذاتي_NAMESPACE_BEGIN

فراغ bitwise_unary_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = !cell->type.اندريون(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

	لكل (صحيح i = 0; i < y_ويڪر; i++)
	{
		إذا (i < a_ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_ويڪر > 0)
			db->جوڙيو_ڪنارو(cell, ID::A, a_ويڪر-1, ID::Y, i, -1);
	}
}

فراغ bitwise_ٻٽو_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

	إذا (cell->type == ID($and) && !is_signed) {
		إذا (a_ويڪر > b_ويڪر)
			a_ويڪر = b_ويڪر;
		وإلا
			b_ويڪر = a_ويڪر;
	}

	لكل (صحيح i = 0; i < y_ويڪر; i++)
	{
		إذا (i < a_ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);
		وإلا إذا (is_signed && a_ويڪر > 0)
			db->جوڙيو_ڪنارو(cell, ID::A, a_ويڪر-1, ID::Y, i, -1);

		إذا (i < b_ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, i, -1);
		وإلا إذا (is_signed && b_ويڪر > 0)
			db->جوڙيو_ڪنارو(cell, ID::B, b_ويڪر-1, ID::Y, i, -1);
	}
}

فراغ arith_neg_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

	إذا (is_signed && a_ويڪر == 1)
		y_ويڪر = std::min(y_ويڪر, 1);

	لكل (صحيح i = 0; i < y_ويڪر; i++)
	لكل (صحيح k = 0; k <= i && k < a_ويڪر; k++)
		db->جوڙيو_ڪنارو(cell, ID::A, k, ID::Y, i, -1);
}

فراغ arith_ٻٽو_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

	إذا (!is_signed && cell->type != ID($sub)) {
		صحيح ab_ويڪر = std::max(a_ويڪر, b_ويڪر);
		y_ويڪر = std::min(y_ويڪر, ab_ويڪر+1);
	}

	لكل (صحيح i = 0; i < y_ويڪر; i++)
	{
		لكل (صحيح k = 0; k <= i; k++)
		{
			إذا (k < a_ويڪر)
				db->جوڙيو_ڪنارو(cell, ID::A, k, ID::Y, i, -1);

			إذا (k < b_ويڪر)
				db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

فراغ reduce_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));

	لكل (صحيح i = 0; i < a_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, 0, -1);
}

فراغ logic_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));

	لكل (صحيح i = 0; i < a_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, 0, -1);
	لكل (صحيح i = 0; i < b_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ concat_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));

	لكل (صحيح i = 0; i < a_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);
	لكل (صحيح i = 0; i < b_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, a_ويڪر + i, -1);
}

فراغ ٽڪرو_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح وٿي = cell->getParam(ID::وٿي_2).as_int();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

	لكل (صحيح i = 0; i < y_ويڪر; i++) {
		صحيح a_ٻٽو_انگ = وٿي + i;
		إذا (a_ٻٽو_انگ >= 0 && a_ٻٽو_انگ < a_ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::A, a_ٻٽو_انگ, ID::Y, i, -1);
	}
}

فراغ compare_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));

	لكل (صحيح i = 0; i < a_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, 0, -1);

	لكل (صحيح i = 0; i < b_ويڪر; i++)
		db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, 0, -1);
}

فراغ mux_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح s_ويڪر = Get_سائيز(cell->get_پورٽ(ID::S));

	لكل (صحيح i = 0; i < a_ويڪر; i++)
	{
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);

		لكل (صحيح k = i; k < b_ويڪر; k += a_ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ bmux_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح s_ويڪر = Get_سائيز(cell->get_پورٽ(ID::S));

	لكل (صحيح i = 0; i < ويڪر; i++)
	{
		لكل (صحيح k = i; k < a_ويڪر; k += ويڪر)
			db->جوڙيو_ڪنارو(cell, ID::A, k, ID::Y, i, -1);

		لكل (صحيح k = 0; k < s_ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ demux_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح s_ويڪر = Get_سائيز(cell->get_پورٽ(ID::S));

	لكل (صحيح i = 0; i < ويڪر; i++)
	{
		db->جوڙيو_ڪنارو(cell, ID::A, i % a_ويڪر, ID::Y, i, -1);
		لكل (صحيح k = 0; k < s_ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::S, k, ID::Y, i, -1);
	}
}

فراغ سرڪايو_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	منطقي is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقي is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح y_ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));

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
	صحيح b_ويڪر_capped = min(b_ويڪر, 30);

	صحيح b_مٿاهون, b_هيٺيون;
	إذا (!is_b_signed) {
		b_مٿاهون = (1 << b_ويڪر_capped) - 1;
		b_هيٺيون = 0;
	} وإلا {
		b_مٿاهون = (1 << (b_ويڪر_capped - 1)) - 1;
		b_هيٺيون = -(1 << (b_ويڪر_capped - 1));
	}

	لكل (صحيح i = 0; i < y_ويڪر; i++){
		// highest position of Y that can change with the value of B
		صحيح b_range_مٿانهون = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحيح a_range_مٿانهون;
		// lowest position of A that can be moved to Y[i]
		صحيح a_range_هيٺاهون;

		إذا (cell->type.اندريون(ID($shl), ID($sshl))) {
			b_range_مٿانهون = a_ويڪر + b_مٿاهون;
			إذا (is_signed) b_range_مٿانهون -= 1;
			a_range_هيٺاهون = max(0, i - b_مٿاهون);
			a_range_مٿانهون = min(i+1, a_ويڪر);
		} وإلا إذا (cell->type.اندريون(ID($shr), ID($sshr)) || (cell->type.اندريون(ID($سرڪايو), ID($shiftx)) && !is_b_signed)) {
			b_range_مٿانهون = a_ويڪر;
			a_range_هيٺاهون = min(i, a_ويڪر - 1);
			a_range_مٿانهون = min(i+1 + b_مٿاهون, a_ويڪر);
		} وإلا إذا (cell->type.اندريون(ID($سرڪايو), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_مٿانهون = a_ويڪر - b_هيٺيون;
			a_range_هيٺاهون = max(0, i + b_هيٺيون);
			إذا (is_signed)
				a_range_هيٺاهون = min(a_range_هيٺاهون, a_ويڪر - 1);
			a_range_مٿانهون = min(i+1 + b_مٿاهون, a_ويڪر);
		} وإلا {
			log_assert(خطأ && "unreachable");
		}

		إذا (i < b_range_مٿانهون) {
			لكل (صحيح k = a_range_هيٺاهون; k < a_range_مٿانهون; k++)
				db->جوڙيو_ڪنارو(cell, ID::A, k, ID::Y, i, -1);
		} وإلا {
			// only influence is through sign extension
			إذا (is_signed)
				db->جوڙيو_ڪنارو(cell, ID::A, a_ويڪر - 1, ID::Y, i, -1);
		}

		لكل (صحيح k = 0; k < b_ويڪر_capped; k++) {
			// left shifts
			إذا (cell->type.اندريون(ID($shl), ID($sshl))) {
				إذا (a_ويڪر == 1 && is_signed) {
					صحيح skip = 1 << (k + 1);
					صحيح بنياد = skip -1;
					إذا (i % skip != بنياد && i - a_ويڪر + 2 < 1 << b_ويڪر_capped)
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				} وإلا إذا (is_signed) {
					إذا (i - a_ويڪر + 2 < 1 << b_ويڪر_capped)
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i - a_ويڪر + 1 < 1 << b_ويڪر_capped)
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} وإلا إذا (cell->type.اندريون(ID($shr), ID($sshr)) || (cell->type.اندريون(ID($سرڪايو), ID($shiftx)) && !is_b_signed)) {
				إذا (is_signed) {
					منطقي سرڪايو_اندريون_bulk = i < a_ويڪر - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقي zpad_jump = (((y_ويڪر - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_ويڪر - i) & ~(1 << k)) < (1 << b_ويڪر_capped)));

					إذا (سرڪايو_اندريون_bulk || (cell->type.اندريون(ID($shr), ID($سرڪايو), ID($shiftx)) && zpad_jump))
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				} وإلا {
					إذا (i < a_ويڪر)
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} وإلا إذا (cell->type.اندريون(ID($سرڪايو), ID($shiftx)) && is_b_signed) {
				إذا (is_signed) {
					إذا (k != b_ويڪر_capped - 1) {
						منطقي r_سرڪايو_اندريون_bulk = i < a_ويڪر - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقي r_zpad_jump = (((y_ويڪر - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_ويڪر - i) & ~(1 << k)) <= b_مٿاهون));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقي l = a_ويڪر - 2 - i >= b_هيٺيون;
						إذا (a_ويڪر == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_ويڪر) & ((1 << (k + 1)) - 1)) != 0;
						}
						إذا (r_سرڪايو_اندريون_bulk || r_zpad_jump || l)
							db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
					} وإلا {
						إذا (y_ويڪر - i <= b_مٿاهون || a_ويڪر - 2 - i >= b_هيٺيون)
							db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
					}
				} وإلا {
					إذا (a_ويڪر - 1 - i >= b_هيٺيون)
						db->جوڙيو_ڪنارو(cell, ID::B, k, ID::Y, i, -1);
				}
			} وإلا {
				log_assert(خطأ && "unreachable");
			}
		}
	}
}

فراغ packed_mem_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحيح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح ويڪر = cell->getParam(ID::ويڪر_2).as_int();

	لكل (صحيح i = 0; i < n_rd_ports; i++) {
		إذا (rd_clk_enable[i] != State::S0) {
			لكل (صحيح k = 0; k < ويڪر; k++)
				db->جوڙيو_ڪنارو(cell, ID::RD_ARST, i, ID::RD_معلومات, i * ويڪر + k, -1);
			استمر;
		}

		لكل (صحيح j = 0; j < abits; j++)
			لكل (صحيح k = 0; k < ويڪر; k++)
				db->جوڙيو_ڪنارو(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_معلومات, i * ويڪر + k, -1);
	}
}

فراغ memrd_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.اندريون(ID($memrd), ID($memrd_v2)));
	صحيح abits = cell->getParam(ID::ABITS).as_int();
	صحيح ويڪر = cell->getParam(ID::ويڪر_2).as_int();

	إذا (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		إذا (cell->type == ID($memrd_v2)) {
			لكل (صحيح k = 0; k < ويڪر; k++)
				db->جوڙيو_ڪنارو(cell, ID::ARST, 0, ID::معلومات_2, k, -1);
		}
		أرجع;
	}

	لكل (صحيح j = 0; j < abits; j++)
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::ADDR, j, ID::معلومات_2, k, -1);
}

فراغ mem_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	إذا (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	وإلا إذا (cell->type.اندريون(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	وإلا إذا (cell->type.اندريون(ID($memwr), ID($memwr_v2), ID($meminit)))
		أرجع; /* no edges here */
	وإلا
		log_abort();
}

فراغ ff_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح ويڪر = cell->get_پورٽ(ID::Q).سائيز();

	إذا (cell->type.اندريون(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		لكل (صحيح k = 0; k < ويڪر; k++) {
			db->جوڙيو_ڪنارو(cell, ID::D, k, ID::Q, k, -1);
			db->جوڙيو_ڪنارو(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	إذا (cell->has_پورٽ(ID::CLR))
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::CLR, 0, ID::Q, k, -1);
	إذا (cell->has_پورٽ(ID::سيٽ_2))
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::سيٽ_2, 0, ID::Q, k, -1);
	إذا (cell->has_پورٽ(ID::ALOAD))
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::ALOAD, 0, ID::Q, k, -1);
	إذا (cell->has_پورٽ(ID::AD))
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::AD, k, ID::Q, k, -1);
	إذا (cell->has_پورٽ(ID::ARST))
		لكل (صحيح k = 0; k < ويڪر; k++)
			db->جوڙيو_ڪنارو(cell, ID::ARST, 0, ID::Q, k, -1);
}

فراغ ڀريل_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	std::ويڪٽر<RTLIL::IdString> داخلا_ports;
	std::ويڪٽر<RTLIL::IdString> output_ports;

	لكل (خودڪار &conn : cell->connections())
	{
		RTLIL::IdString پورٽ = conn.پهريون;
		RTLIL::پورٽ_Dir dir = cell->پورٽ_dir(پورٽ);
		إذا (cell->داخلا(پورٽ) || dir == RTLIL::پورٽ_Dir::PD_INOUT)
			داخلا_ports.ڌڪو_پٺ(پورٽ);
		إذا (cell->output(پورٽ) || dir == RTLIL::پورٽ_Dir::PD_INOUT)
			output_ports.ڌڪو_پٺ(پورٽ);
	}

	لكل (خودڪار ٻاهريون_پورٽ : output_ports)
	{
		صحيح ٻاهريون_ويڪر = Get_سائيز(cell->get_پورٽ(ٻاهريون_پورٽ));
		لكل (صحيح ٻاهريون_ٻٽو_انگ = 0; ٻاهريون_ٻٽو_انگ < ٻاهريون_ويڪر; ٻاهريون_ٻٽو_انگ++)
		{
			لكل (خودڪار اندريون_پورٽ : داخلا_ports)
			{
				صحيح اندريون_ويڪر = Get_سائيز(cell->get_پورٽ(اندريون_پورٽ));
				لكل (صحيح اندريون_ٻٽو_انگ = 0; اندريون_ٻٽو_انگ < اندريون_ويڪر; اندريون_ٻٽو_انگ++)
					db->جوڙيو_ڪنارو(cell, اندريون_پورٽ, اندريون_ٻٽو_انگ, ٻاهريون_پورٽ, ٻاهريون_ٻٽو_انگ, -1);
			}
		}
	}
}

فراغ bweqx_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح max_ويڪر = std::min(ويڪر, std::min(a_ويڪر, b_ويڪر));

	لكل (صحيح i = 0; i < max_ويڪر; i++) {
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);
		db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, i, -1);
	}
}

فراغ bwmux_op(Abstract_Cell_ڪنارا_Database *db, RTLIL::Cell *cell)
{
	صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
	صحيح a_ويڪر = Get_سائيز(cell->get_پورٽ(ID::A));
	صحيح b_ويڪر = Get_سائيز(cell->get_پورٽ(ID::B));
	صحيح s_ويڪر = Get_سائيز(cell->get_پورٽ(ID::S));
	صحيح max_ويڪر = std::min(ويڪر, std::min(a_ويڪر, std::min(b_ويڪر, s_ويڪر)));

	لكل (صحيح i = 0; i < max_ويڪر; i++) {
		db->جوڙيو_ڪنارو(cell, ID::A, i, ID::Y, i, -1);
		db->جوڙيو_ڪنارو(cell, ID::B, i, ID::Y, i, -1);
		db->جوڙيو_ڪنارو(cell, ID::S, i, ID::Y, i, -1);
	}
}

ذاتي_NAMESPACE_پڇاڙي

منطقي YOSYS_NAMESPACE_اڳياڙي Abstract_Cell_ڪنارا_Database::جوڙيو_ڪنارا_from_cell(RTLIL::Cell *cell)
{
	إذا (cell->type.اندريون(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ٻٽو_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($neg)) {
		arith_neg_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($جوڙيو), ID($sub))) {
		arith_ٻٽو_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($logic_and), ID($logic_or))) {
		logic_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($ٽڪرو)) {
		ٽڪرو_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type == ID($concat)) {
		concat_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرڪايو), ID($shiftx))) {
		سرڪايو_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($mux), ID($pmux))) {
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

	إذا (cell->type.اندريون(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->is_builtin_ff()) {
		ff_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
		ڀريل_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		ڀريل_op(هذا, cell);
		أرجع سچ;
	}

	إذا (cell->type.اندريون(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		ڀريل_op(هذا, cell);
		أرجع سچ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	إذا (cell->type.اندريون(ID($أكد), ID($assume), ID($جيئرو), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		أرجع سچ; // no-op: these have either no inputs or no outputs

	أرجع خطأ;
}
