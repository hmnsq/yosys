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

#ifndef CELLAIGS_H
#define CELLAIGS_H

#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ഘടന Aig_കണ്ണി
{
	IdString portname;
	പൂർണ്ണസംഖ്യ portbit;
	bool inverter;
	പൂർണ്ണസംഖ്യ ഇടത്_parent, വലത്_parent;
	vector<pair<IdString, പൂർണ്ണസംഖ്യ>> outports;

	Aig_കണ്ണി();
	bool operator==(സ്ഥിരം Aig_കണ്ണി &other) സ്ഥിരം;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

ഘടന Aig
{
	string name;
	vector<Aig_കണ്ണി> nodes;
	Aig(Cell *cell);

	bool operator==(സ്ഥിരം Aig &other) സ്ഥിരം;
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

YOSYS_NAMESPACE_END

#endif
