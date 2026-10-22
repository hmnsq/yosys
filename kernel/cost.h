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

#اگر_غیر_تعریف_شدہ لاگت_H
#تعریف_کرو لاگت_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساخت CellCosts
{

	نجی:
	dict<RTLIL::IdString, صحیح> mod_لاگت_cache;
	Design *design_ = nullptr;

	عوامی:
	CellCosts(RTLIL::Design *design) : design_(design) { }

	ساکن ثابت dict<RTLIL::IdString, صحیح>& default_gate_لاگت() {
		// Default size heuristics for several common PDK standard cells
		// used by abc and stat
		ساکن ثابت dict<RTLIL::IdString, صحیح> db = {
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
		واپس db;
	}

	ساکن ثابت dict<RTLIL::IdString, صحیح>& cmos_gate_لاگت() {
		// Estimated CMOS transistor counts for several common PDK standard cells
		// used by stat and optionally by abc
		ساکن ثابت dict<RTLIL::IdString, صحیح> db = {
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
		واپس db;
	}

	// Get the cell cost for a cell based on its parameters.
	// This cost is an *approximate* upper bound for the number of gates that
	// the cell will get mapped to with "opt -fast; techmap"
	// The intended usage is for flattening heuristics and similar situations
	غیرعلامتی صحیح get(RTLIL::Cell *cell);
	// Sum up the cell costs of all cells in the module
	// and all its submodules recursively
	غیرعلامتی صحیح get(RTLIL::Module *mod);
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
