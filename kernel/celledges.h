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

#جيڪڏهن_وصف_نه_ٿيل CELLEDGES_H
#وصف_ڪريو CELLEDGES_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

بنية Abstract_Cell_ڪنارا_Database
{
	مجازي ~Abstract_Cell_ڪنارا_Database() { }
	مجازي فراغ جوڙيو_ڪنارو(RTLIL::Cell *cell, RTLIL::IdString from_پورٽ, صحيح from_ٻٽو_انگ, RTLIL::IdString منزل_پورٽ, صحيح منزل_ٻٽو_انگ, صحيح delay) = 0;
	bool جوڙيو_ڪنارا_from_cell(RTLIL::Cell *cell);
};

بنية Fwd_Cell_ڪنارا_Database : Abstract_Cell_ڪنارا_Database
{
	SigMap &sigmap;
	dict<Sig_ٻٽو_انگ, pool<Sig_ٻٽو_انگ>> db;
	Fwd_Cell_ڪنارا_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جوڙيو_ڪنارو(RTLIL::Cell *cell, RTLIL::IdString from_پورٽ, صحيح from_ٻٽو_انگ, RTLIL::IdString منزل_پورٽ, صحيح منزل_ٻٽو_انگ, صحيح) override {
		Sig_ٻٽو_انگ from_sigbit = sigmap(cell->get_پورٽ(from_پورٽ)[from_ٻٽو_انگ]);
		Sig_ٻٽو_انگ منزل_sigbit = sigmap(cell->get_پورٽ(منزل_پورٽ)[منزل_ٻٽو_انگ]);
		db[from_sigbit].وجهو(منزل_sigbit);
	}
};

بنية Rev_Cell_ڪنارا_Database : Abstract_Cell_ڪنارا_Database
{
	SigMap &sigmap;
	dict<Sig_ٻٽو_انگ, pool<Sig_ٻٽو_انگ>> db;
	Rev_Cell_ڪنارا_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جوڙيو_ڪنارو(RTLIL::Cell *cell, RTLIL::IdString from_پورٽ, صحيح from_ٻٽو_انگ, RTLIL::IdString منزل_پورٽ, صحيح منزل_ٻٽو_انگ, صحيح) override {
		Sig_ٻٽو_انگ from_sigbit = sigmap(cell->get_پورٽ(from_پورٽ)[from_ٻٽو_انگ]);
		Sig_ٻٽو_انگ منزل_sigbit = sigmap(cell->get_پورٽ(منزل_پورٽ)[منزل_ٻٽو_انگ]);
		db[منزل_sigbit].وجهو(from_sigbit);
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
