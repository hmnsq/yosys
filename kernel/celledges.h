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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

સંરચના AbstractCellEdgesDatabase
{
	આભાસી ~AbstractCellEdgesDatabase() { }
	આભાસી શૂન્ય add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, પૂર્ણાંક from_bit, RTLIL::IdString to_port, પૂર્ણાંક to_bit, પૂર્ણાંક delay) = 0;
	bool add_edges_from_cell(RTLIL::Cell *cell);
};

સંરચના FwdCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	FwdCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	શૂન્ય add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, પૂર્ણાંક from_bit, RTLIL::IdString to_port, પૂર્ણાંક to_bit, પૂર્ણાંક) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[from_sigbit].ઉમેરો(to_sigbit);
	}
};

સંરચના RevCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	RevCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	શૂન્ય add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, પૂર્ણાંક from_bit, RTLIL::IdString to_port, પૂર્ણાંક to_bit, પૂર્ણાંક) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[to_sigbit].ઉમેરો(from_sigbit);
	}
};

YOSYS_NAMESPACE_END

#endif
