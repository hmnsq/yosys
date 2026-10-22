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

#اگر_غیر_تعریف_شدہ CELLEDGES_H
#تعریف_کرو CELLEDGES_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

ساخت Abstract_Cell_کنارے_Database
{
	مجازی ~Abstract_Cell_کنارے_Database() { }
	مجازی خالی جوڑو_کنارہ(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحیح from_ثنائی_ہندسہ, RTLIL::IdString مقصد_پورٹ, صحیح مقصد_ثنائی_ہندسہ, صحیح delay) = 0;
	bool جوڑو_کنارے_from_cell(RTLIL::Cell *cell);
};

ساخت Fwd_Cell_کنارے_Database : Abstract_Cell_کنارے_Database
{
	SigMap &sigmap;
	dict<Sig_ثنائی_ہندسہ, pool<Sig_ثنائی_ہندسہ>> db;
	Fwd_Cell_کنارے_Database(SigMap &sigmap) : sigmap(sigmap) { }

	خالی جوڑو_کنارہ(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحیح from_ثنائی_ہندسہ, RTLIL::IdString مقصد_پورٹ, صحیح مقصد_ثنائی_ہندسہ, صحیح) override {
		Sig_ثنائی_ہندسہ from_sigbit = sigmap(cell->get_پورٹ(from_پورٹ)[from_ثنائی_ہندسہ]);
		Sig_ثنائی_ہندسہ مقصد_sigbit = sigmap(cell->get_پورٹ(مقصد_پورٹ)[مقصد_ثنائی_ہندسہ]);
		db[from_sigbit].داخل_کرو(مقصد_sigbit);
	}
};

ساخت Rev_Cell_کنارے_Database : Abstract_Cell_کنارے_Database
{
	SigMap &sigmap;
	dict<Sig_ثنائی_ہندسہ, pool<Sig_ثنائی_ہندسہ>> db;
	Rev_Cell_کنارے_Database(SigMap &sigmap) : sigmap(sigmap) { }

	خالی جوڑو_کنارہ(RTLIL::Cell *cell, RTLIL::IdString from_پورٹ, صحیح from_ثنائی_ہندسہ, RTLIL::IdString مقصد_پورٹ, صحیح مقصد_ثنائی_ہندسہ, صحیح) override {
		Sig_ثنائی_ہندسہ from_sigbit = sigmap(cell->get_پورٹ(from_پورٹ)[from_ثنائی_ہندسہ]);
		Sig_ثنائی_ہندسہ مقصد_sigbit = sigmap(cell->get_پورٹ(مقصد_پورٹ)[مقصد_ثنائی_ہندسہ]);
		db[مقصد_sigbit].داخل_کرو(from_sigbit);
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
