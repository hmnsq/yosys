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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CELLEDGES_H
#ವ್ಯಾಖ್ಯಾನಿಸು CELLEDGES_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ Abstract_Cell_ಅಂಚುಗಳು_Database
{
	ವಾಸ್ತವಿಕ ~Abstract_Cell_ಅಂಚುಗಳು_Database() { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(RTLIL::Cell *cell, RTLIL::IdString from_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ from_ಬಿಟ್, RTLIL::IdString ಗಮ್ಯ_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_ಬಿಟ್, ಪೂರ್ಣಾಂಕ delay) = 0;
	bool ಕೂಡಿಸು_ಅಂಚುಗಳು_from_cell(RTLIL::Cell *cell);
};

ರಚನೆ Fwd_Cell_ಅಂಚುಗಳು_Database : Abstract_Cell_ಅಂಚುಗಳು_Database
{
	SigMap &sigmap;
	dict<Sig_ಬಿಟ್, pool<Sig_ಬಿಟ್>> db;
	Fwd_Cell_ಅಂಚುಗಳು_Database(SigMap &sigmap) : sigmap(sigmap) { }

	ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(RTLIL::Cell *cell, RTLIL::IdString from_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ from_ಬಿಟ್, RTLIL::IdString ಗಮ್ಯ_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_ಬಿಟ್, ಪೂರ್ಣಾಂಕ) override {
		Sig_ಬಿಟ್ from_sigbit = sigmap(cell->get_ಪೋರ್ಟ್(from_ಪೋರ್ಟ್)[from_ಬಿಟ್]);
		Sig_ಬಿಟ್ ಗಮ್ಯ_sigbit = sigmap(cell->get_ಪೋರ್ಟ್(ಗಮ್ಯ_ಪೋರ್ಟ್)[ಗಮ್ಯ_ಬಿಟ್]);
		db[from_sigbit].ಸೇರಿಸು(ಗಮ್ಯ_sigbit);
	}
};

ರಚನೆ Rev_Cell_ಅಂಚುಗಳು_Database : Abstract_Cell_ಅಂಚುಗಳು_Database
{
	SigMap &sigmap;
	dict<Sig_ಬಿಟ್, pool<Sig_ಬಿಟ್>> db;
	Rev_Cell_ಅಂಚುಗಳು_Database(SigMap &sigmap) : sigmap(sigmap) { }

	ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(RTLIL::Cell *cell, RTLIL::IdString from_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ from_ಬಿಟ್, RTLIL::IdString ಗಮ್ಯ_ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_ಬಿಟ್, ಪೂರ್ಣಾಂಕ) override {
		Sig_ಬಿಟ್ from_sigbit = sigmap(cell->get_ಪೋರ್ಟ್(from_ಪೋರ್ಟ್)[from_ಬಿಟ್]);
		Sig_ಬಿಟ್ ಗಮ್ಯ_sigbit = sigmap(cell->get_ಪೋರ್ಟ್(ಗಮ್ಯ_ಪೋರ್ಟ್)[ಗಮ್ಯ_ಬಿಟ್]);
		db[ಗಮ್ಯ_sigbit].ಸೇರಿಸು(from_sigbit);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
