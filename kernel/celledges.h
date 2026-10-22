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

#अगर_अघोषित CELLEDGES_H
#घोषणा CELLEDGES_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा AbstractCellEdgesDatabase
{
	भव ~AbstractCellEdgesDatabase() { }
	भव व्योम add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, पूर्णांक from_bit, RTLIL::IdString to_port, पूर्णांक to_bit, पूर्णांक delay) = 0;
	bool add_edges_from_cell(RTLIL::Cell *cell);
};

काष्ठा FwdCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	FwdCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	व्योम add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, पूर्णांक from_bit, RTLIL::IdString to_port, पूर्णांक to_bit, पूर्णांक) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[from_sigbit].जोड़ो(to_sigbit);
	}
};

काष्ठा RevCellEdgesDatabase : AbstractCellEdgesDatabase
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	RevCellEdgesDatabase(SigMap &sigmap) : sigmap(sigmap) { }

	व्योम add_edge(RTLIL::Cell *cell, RTLIL::IdString from_port, पूर्णांक from_bit, RTLIL::IdString to_port, पूर्णांक to_bit, पूर्णांक) override {
		SigBit from_sigbit = sigmap(cell->getPort(from_port)[from_bit]);
		SigBit to_sigbit = sigmap(cell->getPort(to_port)[to_bit]);
		db[to_sigbit].जोड़ो(from_sigbit);
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
