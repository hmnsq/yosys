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

#اگر_تعریف_نشده CELLEDGES_H
#تعریف_کن CELLEDGES_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

بنية Abstract_Cell_یالها_Database
{
	مجازی ~Abstract_Cell_یالها_Database() { }
	مجازی فراغ جمع_کن_یال(RTLIL::Cell *cell, RTLIL::IdString from_پورت, صحيح from_بیت, RTLIL::IdString مقصد_پورت, صحيح مقصد_بیت, صحيح delay) = 0;
	bool جمع_کن_یالها_from_cell(RTLIL::Cell *cell);
};

بنية Fwd_Cell_یالها_Database : Abstract_Cell_یالها_Database
{
	SigMap &sigmap;
	dict<Sig_بیت, pool<Sig_بیت>> db;
	Fwd_Cell_یالها_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جمع_کن_یال(RTLIL::Cell *cell, RTLIL::IdString from_پورت, صحيح from_بیت, RTLIL::IdString مقصد_پورت, صحيح مقصد_بیت, صحيح) override {
		Sig_بیت from_sigbit = sigmap(cell->get_پورت(from_پورت)[from_بیت]);
		Sig_بیت مقصد_sigbit = sigmap(cell->get_پورت(مقصد_پورت)[مقصد_بیت]);
		db[from_sigbit].درج(مقصد_sigbit);
	}
};

بنية Rev_Cell_یالها_Database : Abstract_Cell_یالها_Database
{
	SigMap &sigmap;
	dict<Sig_بیت, pool<Sig_بیت>> db;
	Rev_Cell_یالها_Database(SigMap &sigmap) : sigmap(sigmap) { }

	فراغ جمع_کن_یال(RTLIL::Cell *cell, RTLIL::IdString from_پورت, صحيح from_بیت, RTLIL::IdString مقصد_پورت, صحيح مقصد_بیت, صحيح) override {
		Sig_بیت from_sigbit = sigmap(cell->get_پورت(from_پورت)[from_بیت]);
		Sig_بیت مقصد_sigbit = sigmap(cell->get_پورت(مقصد_پورت)[مقصد_بیت]);
		db[مقصد_sigbit].درج(from_sigbit);
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
