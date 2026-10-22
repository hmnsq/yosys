/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Jannis Harder <jix@yosyshq.com> <me@jix.one>
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

#अन्तर्भाव "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा <typename I, typename Filter> शून्य ModuleHdlnameIndex::index_वस्तुहरू(I begin, I end, Filter filter)
{
	लागि (; begin != end; ++begin) {
		स्वतः अचर &item = *begin;

		यदि (!filter(item))
			जारी;
		std::vector<IdString> path = parse_hdlname(item);
		यदि (!path.empty())
			lookup.emplace(item, रूख.घुसाउ(path, item));
	}
}

शून्य ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

शून्य ModuleHdlnameIndex::index_wires()
{
	स्वतः wires = module->wires();
	index_वस्तुहरू(wires.begin(), wires.end(), [](Wire *) { फर्काऊ सत्य; });
}

शून्य ModuleHdlnameIndex::index_cells()
{
	स्वतः cells = module->cells();
	index_वस्तुहरू(cells.begin(), cells.end(), [](Cell *) { फर्काऊ सत्य; });
}

शून्य ModuleHdlnameIndex::index_scopeinfo_cells()
{
	स्वतः cells = module->cells();
	index_वस्तुहरू(cells.begin(), cells.end(), [](Cell *cell) { फर्काऊ cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	लागि (; !cursor.is_जरा(); cursor = cursor.parent()) {
		यदि (!cursor.has_entry()) {
			result.थप_back("");
			result.थप_back("");
			जारी;
		}
		Cell *cell = cursor.entry().cell();
		यदि (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.थप_back("");
			result.थप_back("");
			जारी;
		}
		result.थप_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.थप_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.थप_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	फर्काऊ result;
}

स्थिर अचर वर्ण *attr_prefix(ScopeinfoAttrs attrs)
{
	विकल्प (attrs) {
	पक्ष ScopeinfoAttrs::Cell:
		फर्काऊ "\\cell_";
	पक्ष ScopeinfoAttrs::Module:
		फर्काऊ "\\module_";
	स्वभाव:
		log_abort();
	}
}

तार्किक scopeinfo_has_attribute(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString साँचो = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	फर्काऊ साँचो != IdString::Null && scopeinfo->has_attribute(साँचो);
}

RTLIL::Const scopeinfo_get_attribute(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString साँचो = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	यदि (साँचो == IdString::Null)
		फर्काऊ RTLIL::Const();
	स्वतः found = scopeinfo->attributes.find(साँचो);
	यदि (found == scopeinfo->attributes.end())
		फर्काऊ RTLIL::Const();
	फर्काऊ found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	अचर वर्ण *prefix = attr_prefix(attrs);
	माप_प्रकार prefix_len = म_माप(prefix);
	TwinePool &twines = scopeinfo->twines();

	लागि (स्वतः अचर &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		यदि (name.compare(0, prefix_len, prefix) != 0)
			जारी;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	फर्काऊ attributes;
}

YOSYS_NAMESPACE_END
