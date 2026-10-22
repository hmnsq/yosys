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

#నిర్వచితం_కాకపోతే CELLAIGS_H
#నిర్వచించు CELLAIGS_H

#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం Aig_కణుపు
{
	IdString portname;
	పూర్ణాంకం portbit;
	bool inverter;
	పూర్ణాంకం ఎడమ_మాతృ, కుడి_మాతృ;
	సదిశ<pair<IdString, పూర్ణాంకం>> outports;

	Aig_కణుపు();
	bool క్రియాకారకం==(స్థిరరాశి Aig_కణుపు &other) స్థిరరాశి;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

నిర్మాణం Aig
{
	string name;
	సదిశ<Aig_కణుపు> nodes;
	Aig(Cell *cell);

	bool క్రియాకారకం==(స్థిరరాశి Aig &other) స్థిరరాశి;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
