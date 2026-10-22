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

#अगर_अघोषित CELLAIGS_H
#घोषणा CELLAIGS_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा Aig_गाँठ
{
	IdString portname;
	पूर्णांक portbit;
	bool inverter;
	पूर्णांक बायाँ_parent, दायाँ_parent;
	vector<pair<IdString, पूर्णांक>> outports;

	Aig_गाँठ();
	bool चालक==(स्थिर Aig_गाँठ &other) स्थिर;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

काष्ठा Aig
{
	string name;
	vector<Aig_गाँठ> nodes;
	Aig(Cell *cell);

	bool चालक==(स्थिर Aig &other) स्थिर;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
