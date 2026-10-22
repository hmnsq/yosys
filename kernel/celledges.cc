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
वैयक्तिकम्_NAMESPACE_BEGIN

शून्य bitwise_unary_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = !cell->type.अन्तर्(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++)
	{
		यदि (i < a_विस्तारः)
			db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && a_विस्तारः > 0)
			db->सङ्कलय_edge(cell, ID::A, a_विस्तारः-1, ID::Y, i, -1);
	}
}

शून्य bitwise_द्विमान_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

	यदि (cell->type == ID($and) && !is_signed) {
		यदि (a_विस्तारः > b_विस्तारः)
			a_विस्तारः = b_विस्तारः;
		अन्यथा
			b_विस्तारः = a_विस्तारः;
	}

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++)
	{
		यदि (i < a_विस्तारः)
			db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && a_विस्तारः > 0)
			db->सङ्कलय_edge(cell, ID::A, a_विस्तारः-1, ID::Y, i, -1);

		यदि (i < b_विस्तारः)
			db->सङ्कलय_edge(cell, ID::B, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && b_विस्तारः > 0)
			db->सङ्कलय_edge(cell, ID::B, b_विस्तारः-1, ID::Y, i, -1);
	}
}

शून्य arith_neg_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

	यदि (is_signed && a_विस्तारः == 1)
		y_विस्तारः = std::min(y_विस्तारः, 1);

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++)
	प्रति (पूर्णाङ्क k = 0; k <= i && k < a_विस्तारः; k++)
		db->सङ्कलय_edge(cell, ID::A, k, ID::Y, i, -1);
}

शून्य arith_द्विमान_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

	यदि (!is_signed && cell->type != ID($sub)) {
		पूर्णाङ्क ab_विस्तारः = std::max(a_विस्तारः, b_विस्तारः);
		y_विस्तारः = std::min(y_विस्तारः, ab_विस्तारः+1);
	}

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++)
	{
		प्रति (पूर्णाङ्क k = 0; k <= i; k++)
		{
			यदि (k < a_विस्तारः)
				db->सङ्कलय_edge(cell, ID::A, k, ID::Y, i, -1);

			यदि (k < b_विस्तारः)
				db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

शून्य reduce_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));

	प्रति (पूर्णाङ्क i = 0; i < a_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, 0, -1);
}

शून्य logic_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));

	प्रति (पूर्णाङ्क i = 0; i < a_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, 0, -1);
	प्रति (पूर्णाङ्क i = 0; i < b_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य concat_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));

	प्रति (पूर्णाङ्क i = 0; i < a_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);
	प्रति (पूर्णाङ्क i = 0; i < b_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::B, i, ID::Y, a_विस्तारः + i, -1);
}

शून्य खण्डिका_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्थापनम् = cell->getParam(ID::विस्थापनम्_2).as_int();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++) {
		पूर्णाङ्क a_bit = विस्थापनम् + i;
		यदि (a_bit >= 0 && a_bit < a_विस्तारः)
			db->सङ्कलय_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

शून्य compare_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));

	प्रति (पूर्णाङ्क i = 0; i < a_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, 0, -1);

	प्रति (पूर्णाङ्क i = 0; i < b_विस्तारः; i++)
		db->सङ्कलय_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य mux_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क s_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::S));

	प्रति (पूर्णाङ्क i = 0; i < a_विस्तारः; i++)
	{
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);

		प्रति (पूर्णाङ्क k = i; k < b_विस्तारः; k += a_विस्तारः)
			db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);

		प्रति (पूर्णाङ्क k = 0; k < s_विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य bmux_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क s_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::S));

	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
	{
		प्रति (पूर्णाङ्क k = i; k < a_विस्तारः; k += विस्तारः)
			db->सङ्कलय_edge(cell, ID::A, k, ID::Y, i, -1);

		प्रति (पूर्णाङ्क k = 0; k < s_विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य demux_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क s_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::S));

	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
	{
		db->सङ्कलय_edge(cell, ID::A, i % a_विस्तारः, ID::Y, i, -1);
		प्रति (पूर्णाङ्क k = 0; k < s_विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य सरणम्_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	तार्किक is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क y_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));

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
	पूर्णाङ्क b_विस्तारः_capped = min(b_विस्तारः, 30);

	पूर्णाङ्क b_उच्चः, b_नीचः;
	यदि (!is_b_signed) {
		b_उच्चः = (1 << b_विस्तारः_capped) - 1;
		b_नीचः = 0;
	} अन्यथा {
		b_उच्चः = (1 << (b_विस्तारः_capped - 1)) - 1;
		b_नीचः = -(1 << (b_विस्तारः_capped - 1));
	}

	प्रति (पूर्णाङ्क i = 0; i < y_विस्तारः; i++){
		// highest position of Y that can change with the value of B
		पूर्णाङ्क b_range_ऊर्ध्वस्थम् = 0;
		// 1 + highest position of A that can be moved to Y[i]
		पूर्णाङ्क a_range_ऊर्ध्वस्थम्;
		// lowest position of A that can be moved to Y[i]
		पूर्णाङ्क a_range_अधस्तनम्;

		यदि (cell->type.अन्तर्(ID($shl), ID($sshl))) {
			b_range_ऊर्ध्वस्थम् = a_विस्तारः + b_उच्चः;
			यदि (is_signed) b_range_ऊर्ध्वस्थम् -= 1;
			a_range_अधस्तनम् = max(0, i - b_उच्चः);
			a_range_ऊर्ध्वस्थम् = min(i+1, a_विस्तारः);
		} अन्यथा यदि (cell->type.अन्तर्(ID($shr), ID($sshr)) || (cell->type.अन्तर्(ID($सरणम्), ID($shiftx)) && !is_b_signed)) {
			b_range_ऊर्ध्वस्थम् = a_विस्तारः;
			a_range_अधस्तनम् = min(i, a_विस्तारः - 1);
			a_range_ऊर्ध्वस्थम् = min(i+1 + b_उच्चः, a_विस्तारः);
		} अन्यथा यदि (cell->type.अन्तर्(ID($सरणम्), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_ऊर्ध्वस्थम् = a_विस्तारः - b_नीचः;
			a_range_अधस्तनम् = max(0, i + b_नीचः);
			यदि (is_signed)
				a_range_अधस्तनम् = min(a_range_अधस्तनम्, a_विस्तारः - 1);
			a_range_ऊर्ध्वस्थम् = min(i+1 + b_उच्चः, a_विस्तारः);
		} अन्यथा {
			log_assert(असत्य && "unreachable");
		}

		यदि (i < b_range_ऊर्ध्वस्थम्) {
			प्रति (पूर्णाङ्क k = a_range_अधस्तनम्; k < a_range_ऊर्ध्वस्थम्; k++)
				db->सङ्कलय_edge(cell, ID::A, k, ID::Y, i, -1);
		} अन्यथा {
			// only influence is through sign extension
			यदि (is_signed)
				db->सङ्कलय_edge(cell, ID::A, a_विस्तारः - 1, ID::Y, i, -1);
		}

		प्रति (पूर्णाङ्क k = 0; k < b_विस्तारः_capped; k++) {
			// left shifts
			यदि (cell->type.अन्तर्(ID($shl), ID($sshl))) {
				यदि (a_विस्तारः == 1 && is_signed) {
					पूर्णाङ्क skip = 1 << (k + 1);
					पूर्णाङ्क आधारः = skip -1;
					यदि (i % skip != आधारः && i - a_विस्तारः + 2 < 1 << b_विस्तारः_capped)
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा यदि (is_signed) {
					यदि (i - a_विस्तारः + 2 < 1 << b_विस्तारः_capped)
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					यदि (i - a_विस्तारः + 1 < 1 << b_विस्तारः_capped)
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} अन्यथा यदि (cell->type.अन्तर्(ID($shr), ID($sshr)) || (cell->type.अन्तर्(ID($सरणम्), ID($shiftx)) && !is_b_signed)) {
				यदि (is_signed) {
					तार्किक सरणम्_अन्तर्_bulk = i < a_विस्तारः - 1;
					// can we jump into the zero-padding by toggling B[k]?
					तार्किक zpad_jump = (((y_विस्तारः - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_विस्तारः - i) & ~(1 << k)) < (1 << b_विस्तारः_capped)));

					यदि (सरणम्_अन्तर्_bulk || (cell->type.अन्तर्(ID($shr), ID($सरणम्), ID($shiftx)) && zpad_jump))
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					यदि (i < a_विस्तारः)
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} अन्यथा यदि (cell->type.अन्तर्(ID($सरणम्), ID($shiftx)) && is_b_signed) {
				यदि (is_signed) {
					यदि (k != b_विस्तारः_capped - 1) {
						तार्किक r_सरणम्_अन्तर्_bulk = i < a_विस्तारः - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						तार्किक r_zpad_jump = (((y_विस्तारः - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_विस्तारः - i) & ~(1 << k)) <= b_उच्चः));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						तार्किक l = a_विस्तारः - 2 - i >= b_नीचः;
						यदि (a_विस्तारः == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_विस्तारः) & ((1 << (k + 1)) - 1)) != 0;
						}
						यदि (r_सरणम्_अन्तर्_bulk || r_zpad_jump || l)
							db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
					} अन्यथा {
						यदि (y_विस्तारः - i <= b_उच्चः || a_विस्तारः - 2 - i >= b_नीचः)
							db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} अन्यथा {
					यदि (a_विस्तारः - 1 - i >= b_नीचः)
						db->सङ्कलय_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} अन्यथा {
				log_assert(असत्य && "unreachable");
			}
		}
	}
}

शून्य packed_mem_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	पूर्णाङ्क n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	पूर्णाङ्क abits = cell->getParam(ID::ABITS).as_int();
	पूर्णाङ्क विस्तारः = cell->getParam(ID::विस्तारः_2).as_int();

	प्रति (पूर्णाङ्क i = 0; i < n_rd_ports; i++) {
		यदि (rd_clk_enable[i] != State::S0) {
			प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
				db->सङ्कलय_edge(cell, ID::RD_ARST, i, ID::RD_दत्त, i * विस्तारः + k, -1);
			अनुवर्त;
		}

		प्रति (पूर्णाङ्क j = 0; j < abits; j++)
			प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
				db->सङ्कलय_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_दत्त, i * विस्तारः + k, -1);
	}
}

शून्य memrd_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.अन्तर्(ID($memrd), ID($memrd_v2)));
	पूर्णाङ्क abits = cell->getParam(ID::ABITS).as_int();
	पूर्णाङ्क विस्तारः = cell->getParam(ID::विस्तारः_2).as_int();

	यदि (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		यदि (cell->type == ID($memrd_v2)) {
			प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
				db->सङ्कलय_edge(cell, ID::ARST, 0, ID::दत्त_2, k, -1);
		}
		प्रत्यागम;
	}

	प्रति (पूर्णाङ्क j = 0; j < abits; j++)
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::ADDR, j, ID::दत्त_2, k, -1);
}

शून्य mem_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	यदि (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	अन्यथा यदि (cell->type.अन्तर्(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	अन्यथा यदि (cell->type.अन्तर्(ID($memwr), ID($memwr_v2), ID($meminit)))
		प्रत्यागम; /* no edges here */
	अन्यथा
		log_abort();
}

शून्य ff_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्तारः = cell->get_पत्तनम्(ID::Q).आकार();

	यदि (cell->type.अन्तर्(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++) {
			db->सङ्कलय_edge(cell, ID::D, k, ID::Q, k, -1);
			db->सङ्कलय_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	यदि (cell->has_पत्तनम्(ID::CLR))
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	यदि (cell->has_पत्तनम्(ID::समुच्चयः_2))
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::समुच्चयः_2, 0, ID::Q, k, -1);
	यदि (cell->has_पत्तनम्(ID::ALOAD))
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	यदि (cell->has_पत्तनम्(ID::AD))
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::AD, k, ID::Q, k, -1);
	यदि (cell->has_पत्तनम्(ID::ARST))
		प्रति (पूर्णाङ्क k = 0; k < विस्तारः; k++)
			db->सङ्कलय_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

शून्य पूर्णम्_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	std::सदिशम्<RTLIL::IdString> निवेशः_ports;
	std::सदिशम्<RTLIL::IdString> output_ports;

	प्रति (स्वचालित &conn : cell->connections())
	{
		RTLIL::IdString पत्तनम् = conn.first;
		RTLIL::पत्तनम्_Dir dir = cell->पत्तनम्_dir(पत्तनम्);
		यदि (cell->निवेशः(पत्तनम्) || dir == RTLIL::पत्तनम्_Dir::PD_INOUT)
			निवेशः_ports.प्रक्षिप_पृष्ठभागः(पत्तनम्);
		यदि (cell->output(पत्तनम्) || dir == RTLIL::पत्तनम्_Dir::PD_INOUT)
			output_ports.प्रक्षिप_पृष्ठभागः(पत्तनम्);
	}

	प्रति (स्वचालित बहिर्गमनम्_पत्तनम् : output_ports)
	{
		पूर्णाङ्क बहिर्गमनम्_विस्तारः = Get_आकार(cell->get_पत्तनम्(बहिर्गमनम्_पत्तनम्));
		प्रति (पूर्णाङ्क बहिर्गमनम्_bit = 0; बहिर्गमनम्_bit < बहिर्गमनम्_विस्तारः; बहिर्गमनम्_bit++)
		{
			प्रति (स्वचालित अन्तर्_पत्तनम् : निवेशः_ports)
			{
				पूर्णाङ्क अन्तर्_विस्तारः = Get_आकार(cell->get_पत्तनम्(अन्तर्_पत्तनम्));
				प्रति (पूर्णाङ्क अन्तर्_bit = 0; अन्तर्_bit < अन्तर्_विस्तारः; अन्तर्_bit++)
					db->सङ्कलय_edge(cell, अन्तर्_पत्तनम्, अन्तर्_bit, बहिर्गमनम्_पत्तनम्, बहिर्गमनम्_bit, -1);
			}
		}
	}
}

शून्य bweqx_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क max_विस्तारः = std::min(विस्तारः, std::min(a_विस्तारः, b_विस्तारः));

	प्रति (पूर्णाङ्क i = 0; i < max_विस्तारः; i++) {
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);
		db->सङ्कलय_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

शून्य bwmux_op(Abstract_Cell_धाराः_Database *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
	पूर्णाङ्क a_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::A));
	पूर्णाङ्क b_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::B));
	पूर्णाङ्क s_विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::S));
	पूर्णाङ्क max_विस्तारः = std::min(विस्तारः, std::min(a_विस्तारः, std::min(b_विस्तारः, s_विस्तारः)));

	प्रति (पूर्णाङ्क i = 0; i < max_विस्तारः; i++) {
		db->सङ्कलय_edge(cell, ID::A, i, ID::Y, i, -1);
		db->सङ्कलय_edge(cell, ID::B, i, ID::Y, i, -1);
		db->सङ्कलय_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

वैयक्तिकम्_NAMESPACE_अन्तः

तार्किक YOSYS_NAMESPACE_उपसर्गः Abstract_Cell_धाराः_Database::सङ्कलय_धाराः_from_cell(RTLIL::Cell *cell)
{
	यदि (cell->type.अन्तर्(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_द्विमान_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($neg)) {
		arith_neg_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($सङ्कलय), ID($sub))) {
		arith_द्विमान_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($logic_and), ID($logic_or))) {
		logic_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($खण्डिका)) {
		खण्डिका_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($concat)) {
		concat_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($सरणम्), ID($shiftx))) {
		सरणम्_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($mux), ID($pmux))) {
		mux_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($bmux)) {
		bmux_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($demux)) {
		demux_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($bweqx)) {
		bweqx_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($bwmux)) {
		bwmux_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->is_builtin_ff()) {
		ff_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($mul), ID($भाजय), ID($mod), ID($divfloor), ID($modfloor), ID($घातः))) {
		पूर्णम्_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		पूर्णम्_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		पूर्णम्_op(एतत्, cell);
		प्रत्यागम सत्य;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	यदि (cell->type.अन्तर्(ID($प्रतिज्ञा), ID($assume), ID($जीवितम्), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		प्रत्यागम सत्य; // no-op: these have either no inputs or no outputs

	प्रत्यागम असत्य;
}
