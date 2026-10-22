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

#جے_بنا_تعریف CELLEDGES_H
#تعریف_کرو CELLEDGES_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

بنية Abstract_Cell_کنارے_Database
{
	مجازی ~Abstract_Cell_کنارے_Database() { }
	مجازی فراغ جوڑو_کنارا(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحيح from_ثنائی_ہندسہ, RTLIL::IdString منزل_پورٹ, صحيح منزل_ثنائی_ہندسہ, صحيح delay) = 0;
	bool جوڑو_کنارے_from_cell(RTLIL::Cell *cell);
};

بنية Fwd_Cell_کنارے_Database : Abstract_Cell_کنارے_Database
{
	SigMap &sigmap;
	dict<Sig_ثنائی_ہندسہ, pool<Sig_ثنائی_ہندسہ>> db;
	Fwd_Cell_کنارے_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جوڑو_کنارا(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحيح from_ثنائی_ہندسہ, RTLIL::IdString منزل_پورٹ, صحيح منزل_ثنائی_ہندسہ, صحيح) override {
		Sig_ثنائی_ہندسہ from_sigbit = sigmap(cell->get_پورٹ(from_پورٹ)[from_ثنائی_ہندسہ]);
		Sig_ثنائی_ہندسہ منزل_sigbit = sigmap(cell->get_پورٹ(منزل_پورٹ)[منزل_ثنائی_ہندسہ]);
		db[from_sigbit].داخل_کرو(منزل_sigbit);
	}
};

بنية Rev_Cell_کنارے_Database : Abstract_Cell_کنارے_Database
{
	SigMap &sigmap;
	dict<Sig_ثنائی_ہندسہ, pool<Sig_ثنائی_ہندسہ>> db;
	Rev_Cell_کنارے_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جوڑو_کنارا(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحيح from_ثنائی_ہندسہ, RTLIL::IdString منزل_پورٹ, صحيح منزل_ثنائی_ہندسہ, صحيح) override {
		Sig_ثنائی_ہندسہ from_sigbit = sigmap(cell->get_پورٹ(from_پورٹ)[from_ثنائی_ہندسہ]);
		Sig_ثنائی_ہندسہ منزل_sigbit = sigmap(cell->get_پورٹ(منزل_پورٹ)[منزل_ثنائی_ہندسہ]);
		db[منزل_sigbit].داخل_کرو(from_sigbit);
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
