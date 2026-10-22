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

#اگر_غیر_تعریف_شدہ CELLAIGS_H
#تعریف_کرو CELLAIGS_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساخت Aig_گرہ
{
	IdString portname;
	صحیح portbit;
	bool inverter;
	صحیح بایاں_والد, دایاں_والد;
	سمتیہ<pair<IdString, صحیح>> outports;

	Aig_گرہ();
	bool عامل==(ثابت Aig_گرہ &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساخت Aig
{
	string name;
	سمتیہ<Aig_گرہ> nodes;
	Aig(Cell *cell);

	bool عامل==(ثابت Aig &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
