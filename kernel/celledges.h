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

#यदि_अपरिभाषितम् CELLEDGES_H
#परिभाषय CELLEDGES_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

संरचना Abstract_Cell_धाराः_Database
{
	आभास ~Abstract_Cell_धाराः_Database() { }
	आभास शून्य सङ्कलय_edge(RTLIL::Cell *cell, RTLIL::IdString from_पत्तनम्, पूर्णाङ्क from_bit, RTLIL::IdString गन्तव्यम्_पत्तनम्, पूर्णाङ्क गन्तव्यम्_bit, पूर्णाङ्क delay) = 0;
	bool सङ्कलय_धाराः_from_cell(RTLIL::Cell *cell);
};

संरचना Fwd_Cell_धाराः_Database : Abstract_Cell_धाराः_Database
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	Fwd_Cell_धाराः_Database(SigMap &sigmap) : sigmap(sigmap) { }

	शून्य सङ्कलय_edge(RTLIL::Cell *cell, RTLIL::IdString from_पत्तनम्, पूर्णाङ्क from_bit, RTLIL::IdString गन्तव्यम्_पत्तनम्, पूर्णाङ्क गन्तव्यम्_bit, पूर्णाङ्क) override {
		SigBit from_sigbit = sigmap(cell->get_पत्तनम्(from_पत्तनम्)[from_bit]);
		SigBit गन्तव्यम्_sigbit = sigmap(cell->get_पत्तनम्(गन्तव्यम्_पत्तनम्)[गन्तव्यम्_bit]);
		db[from_sigbit].निवेशय(गन्तव्यम्_sigbit);
	}
};

संरचना Rev_Cell_धाराः_Database : Abstract_Cell_धाराः_Database
{
	SigMap &sigmap;
	dict<SigBit, pool<SigBit>> db;
	Rev_Cell_धाराः_Database(SigMap &sigmap) : sigmap(sigmap) { }

	शून्य सङ्कलय_edge(RTLIL::Cell *cell, RTLIL::IdString from_पत्तनम्, पूर्णाङ्क from_bit, RTLIL::IdString गन्तव्यम्_पत्तनम्, पूर्णाङ्क गन्तव्यम्_bit, पूर्णाङ्क) override {
		SigBit from_sigbit = sigmap(cell->get_पत्तनम्(from_पत्तनम्)[from_bit]);
		SigBit गन्तव्यम्_sigbit = sigmap(cell->get_पत्तनम्(गन्तव्यम्_पत्तनम्)[गन्तव्यम्_bit]);
		db[गन्तव्यम्_sigbit].निवेशय(from_sigbit);
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
