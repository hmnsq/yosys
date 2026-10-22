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

#اگر_تعریف_نشده CELLAIGS_H
#تعریف_کن CELLAIGS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Aig_گره
{
	IdString portname;
	صحيح portbit;
	bool inverter;
	صحيح چپ_والد, راست_والد;
	وکتور<pair<IdString, صحيح>> outports;

	Aig_گره();
	bool عملگر==(ثابت Aig_گره &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Aig
{
	string name;
	وکتور<Aig_گره> nodes;
	Aig(Cell *cell);

	bool عملگر==(ثابت Aig &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
