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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CELLEDGES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CELLEDGES_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ Abstract_Cell_ਕਿਨਾਰੇ_Database
{
	ਆਭਾਸੀ ~Abstract_Cell_ਕਿਨਾਰੇ_Database() { }
	ਆਭਾਸੀ ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(RTLIL::Cell *cell, RTLIL::IdString from_ਪੋਰਟ, ਪੂਰਨ from_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::IdString ਮੰਜ਼ਿਲ_ਪੋਰਟ, ਪੂਰਨ ਮੰਜ਼ਿਲ_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ delay) = 0;
	bool ਜੋੜੋ_ਕਿਨਾਰੇ_from_cell(RTLIL::Cell *cell);
};

ਬਣਤਰ Fwd_Cell_ਕਿਨਾਰੇ_Database : Abstract_Cell_ਕਿਨਾਰੇ_Database
{
	SigMap &sigmap;
	dict<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>> db;
	Fwd_Cell_ਕਿਨਾਰੇ_Database(SigMap &sigmap) : sigmap(sigmap) { }

	ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(RTLIL::Cell *cell, RTLIL::IdString from_ਪੋਰਟ, ਪੂਰਨ from_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::IdString ਮੰਜ਼ਿਲ_ਪੋਰਟ, ਪੂਰਨ ਮੰਜ਼ਿਲ_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ) override {
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ from_sigbit = sigmap(cell->get_ਪੋਰਟ(from_ਪੋਰਟ)[from_ਦੋਆਧਾਰੀ_ਅੰਕ]);
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਮੰਜ਼ਿਲ_sigbit = sigmap(cell->get_ਪੋਰਟ(ਮੰਜ਼ਿਲ_ਪੋਰਟ)[ਮੰਜ਼ਿਲ_ਦੋਆਧਾਰੀ_ਅੰਕ]);
		db[from_sigbit].ਪਾਓ(ਮੰਜ਼ਿਲ_sigbit);
	}
};

ਬਣਤਰ Rev_Cell_ਕਿਨਾਰੇ_Database : Abstract_Cell_ਕਿਨਾਰੇ_Database
{
	SigMap &sigmap;
	dict<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>> db;
	Rev_Cell_ਕਿਨਾਰੇ_Database(SigMap &sigmap) : sigmap(sigmap) { }

	ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(RTLIL::Cell *cell, RTLIL::IdString from_ਪੋਰਟ, ਪੂਰਨ from_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::IdString ਮੰਜ਼ਿਲ_ਪੋਰਟ, ਪੂਰਨ ਮੰਜ਼ਿਲ_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ) override {
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ from_sigbit = sigmap(cell->get_ਪੋਰਟ(from_ਪੋਰਟ)[from_ਦੋਆਧਾਰੀ_ਅੰਕ]);
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਮੰਜ਼ਿਲ_sigbit = sigmap(cell->get_ਪੋਰਟ(ਮੰਜ਼ਿਲ_ਪੋਰਟ)[ਮੰਜ਼ਿਲ_ਦੋਆਧਾਰੀ_ਅੰਕ]);
		db[ਮੰਜ਼ਿਲ_sigbit].ਪਾਓ(from_sigbit);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
