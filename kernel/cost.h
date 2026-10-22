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

#ifndef COST_H
#define COST_H

#অন্তর্ভাৱ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা CellCosts
{

	গুপ্ত:
	dict<RTLIL::IdString, পূর্ণাঙ্ক> mod_cost_cache_;
	Design *design_ = nullptr;

	সার্ৱজনিক:
	CellCosts(RTLIL::Design *design) : design_(design) { }

	স্থির ধ্রুৱ dict<RTLIL::IdString, পূর্ণাঙ্ক>& default_gate_cost() {
		// Default size heuristics for several common PDK standard cells
		// used by abc and stat
		স্থির ধ্রুৱ dict<RTLIL::IdString, পূর্ণাঙ্ক> db = {
			{ ID($_BUF_),    1 },
			{ ID($_NOT_),    2 },
			{ ID($_AND_),    4 },
			{ ID($_NAND_),   4 },
			{ ID($_OR_),     4 },
			{ ID($_NOR_),    4 },
			{ ID($_ANDNOT_), 4 },
			{ ID($_ORNOT_),  4 },
			{ ID($_XOR_),    5 },
			{ ID($_XNOR_),   5 },
			{ ID($_AOI3_),   6 },
			{ ID($_OAI3_),   6 },
			{ ID($_AOI4_),   7 },
			{ ID($_OAI4_),   7 },
			{ ID($_MUX_),    4 },
			{ ID($_NMUX_),   4 },
		};
		ঘূৰাই db;
	}

	স্থির ধ্রুৱ dict<RTLIL::IdString, পূর্ণাঙ্ক>& cmos_gate_cost() {
		// Estimated CMOS transistor counts for several common PDK standard cells
		// used by stat and optionally by abc
		স্থির ধ্রুৱ dict<RTLIL::IdString, পূর্ণাঙ্ক> db = {
			{ ID($_BUF_),     1 },
			{ ID($_NOT_),     2 },
			{ ID($_AND_),     6 },
			{ ID($_NAND_),    4 },
			{ ID($_OR_),      6 },
			{ ID($_NOR_),     4 },
			{ ID($_ANDNOT_),  6 },
			{ ID($_ORNOT_),   6 },
			{ ID($_XOR_),    12 },
			{ ID($_XNOR_),   12 },
			{ ID($_AOI3_),    6 },
			{ ID($_OAI3_),    6 },
			{ ID($_AOI4_),    8 },
			{ ID($_OAI4_),    8 },
			{ ID($_MUX_),    12 },
			{ ID($_NMUX_),   10 },
			{ ID($_DFF_P_),  16 },
			{ ID($_DFF_N_),  16 },
		};
		ঘূৰাই db;
	}

	// Get the cell cost for a cell based on its parameters.
	// This cost is an *approximate* upper bound for the number of gates that
	// the cell will get mapped to with "opt -fast; techmap"
	// The intended usage is for flattening heuristics and similar situations
	unsigned পূর্ণাঙ্ক get(RTLIL::Cell *cell);
	// Sum up the cell costs of all cells in the module
	// and all its submodules recursively
	unsigned পূর্ণাঙ্ক get(RTLIL::Module *mod);
};

YOSYS_NAMESPACE_END

#endif
