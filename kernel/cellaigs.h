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

#অন্তর্ভাৱ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা Aig_গ্ৰন্থি
{
	IdString portname;
	পূর্ণাঙ্ক portbit;
	bool inverter;
	পূর্ণাঙ্ক বাম_parent, ডান_parent;
	vector<pair<IdString, পূর্ণাঙ্ক>> outports;

	Aig_গ্ৰন্থি();
	bool operator==(ধ্রুৱ Aig_গ্ৰন্থি &other) ধ্রুৱ;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ;
};

সংরচনা Aig
{
	string name;
	vector<Aig_গ্ৰন্থি> nodes;
	Aig(Cell *cell);

	bool operator==(ধ্রুৱ Aig &other) ধ্রুৱ;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ;
};

YOSYS_NAMESPACE_END

#endif
