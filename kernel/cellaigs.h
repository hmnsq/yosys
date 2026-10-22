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

#که_نه_تعریف_شوی CELLAIGS_H
#تعریف_کړه CELLAIGS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Aig_غوټه
{
	IdString portname;
	صحيح portbit;
	bool inverter;
	صحيح کیڼ_پلار, ښی_پلار;
	ویکټور<pair<IdString, صحيح>> outports;

	Aig_غوټه();
	bool چلوونکی==(ثابت Aig_غوټه &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Aig
{
	string name;
	ویکټور<Aig_غوټه> nodes;
	Aig(Cell *cell);

	bool چلوونکی==(ثابت Aig &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
