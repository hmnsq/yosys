/*
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CELLAIGS_H
#ವ್ಯಾಖ್ಯಾನಿಸು CELLAIGS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ Aig_ಗಂಟು
{
	IdString portname;
	ಪೂರ್ಣಾಂಕ portbit;
	bool inverter;
	ಪೂರ್ಣಾಂಕ ಎಡ_ಪೋಷಕ, ಬಲ_ಪೋಷಕ;
	ಸದಿಶ<pair<IdString, ಪೂರ್ಣಾಂಕ>> outports;

	Aig_ಗಂಟು();
	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Aig_ಗಂಟು &other) ಸ್ಥಿರಾಂಕ;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ Aig
{
	string name;
	ಸದಿಶ<Aig_ಗಂಟು> nodes;
	Aig(Cell *cell);

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Aig &other) ಸ್ಥಿರಾಂಕ;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
