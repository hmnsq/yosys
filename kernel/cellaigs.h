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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା Aig_ଗଣ୍ଠି
{
	IdString portname;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା portbit;
	bool inverter;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ_parent, ଡାହାଣ_parent;
	vector<pair<IdString, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> outports;

	Aig_ଗଣ୍ଠି();
	bool operator==(ଅଚଳ Aig_ଗଣ୍ଠି &other) ଅଚଳ;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

ସଂରଚନା Aig
{
	string name;
	vector<Aig_ଗଣ୍ଠି> nodes;
	Aig(Cell *cell);

	bool operator==(ଅଚଳ Aig &other) ଅଚଳ;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

YOSYS_NAMESPACE_END

#endif
