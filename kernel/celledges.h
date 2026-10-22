/* -*- c++ -*-
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

#ifndef CELLEDGES_H
#define CELLEDGES_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

ساختار AbstractCellEdgesDatabase
{
	مجازی ~AbstractCellEdgesDatabase() { }
	مجازی تهی add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, صحیح from_bit, RTLIL::IdString to_port, صحیح to_bit, صحیح delay) = 0;
	bool add_edges_from_cell(RTLIL::Cell *cell);
};

ساختار FwdCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	FwdCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	تهی add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, صحیح from_bit, RTLIL::IdString to_port, صحیح to_bit, صحیح) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[from_sigbit].درج(to_sigbit);
	}
};

ساختار RevCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	RevCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	تهی add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, صحیح from_bit, RTLIL::IdString to_port, صحیح to_bit, صحیح) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[to_sigbit].درج(from_sigbit);
	}
};

YOSYS_NAMESPACE_END

#endif
