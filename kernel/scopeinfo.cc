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

#അന്തര്ഭാവ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> ശൂന്യം ModuleHdlnameIndex::index_ഇനങ്ങൾ(I begin, I end, Filter filter)
{
	വേണ്ടി (; begin != end; ++begin) {
		auto സ്ഥിരം &item = *begin;

		എങ്കിൽ (!filter(item))
			തുടരുക;
		std::vector<IdString> path = parse_hdlname(item);
		എങ്കിൽ (!path.empty())
			lookup.emplace(item, വൃക്ഷം.തിരുകുക(path, item));
	}
}

ശൂന്യം ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

ശൂന്യം ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_ഇനങ്ങൾ(wires.begin(), wires.end(), [](Wire *) { തിരികെ സത്യം; });
}

ശൂന്യം ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_ഇനങ്ങൾ(cells.begin(), cells.end(), [](Cell *) { തിരികെ സത്യം; });
}

ശൂന്യം ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_ഇനങ്ങൾ(cells.begin(), cells.end(), [](Cell *cell) { തിരികെ cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	വേണ്ടി (; !cursor.is_വേര്(); cursor = cursor.parent()) {
		എങ്കിൽ (!cursor.has_entry()) {
			result.തള്ളുക_back("");
			result.തള്ളുക_back("");
			തുടരുക;
		}
		Cell *cell = cursor.entry().cell();
		എങ്കിൽ (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.തള്ളുക_back("");
			result.തള്ളുക_back("");
			തുടരുക;
		}
		result.തള്ളുക_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.തള്ളുക_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.തള്ളുക_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	തിരികെ result;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *attr_prefix(ScopeinfoAttrs attrs)
{
	തിരഞ്ഞെടുപ്പ് (attrs) {
	അവസ്ഥ ScopeinfoAttrs::Cell:
		തിരികെ "\\cell_";
	അവസ്ഥ ScopeinfoAttrs::Module:
		തിരികെ "\\module_";
	സ്ഥിരസ്ഥിതി:
		log_abort();
	}
}

തർക്കം scopeinfo_has_attribute(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString താക്കോൽ = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	തിരികെ താക്കോൽ != IdString::Null && scopeinfo->has_attribute(താക്കോൽ);
}

RTLIL::Const scopeinfo_get_attribute(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString താക്കോൽ = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	എങ്കിൽ (താക്കോൽ == IdString::Null)
		തിരികെ RTLIL::Const();
	auto found = scopeinfo->attributes.find(താക്കോൽ);
	എങ്കിൽ (found == scopeinfo->attributes.end())
		തിരികെ RTLIL::Const();
	തിരികെ found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	സ്ഥിരം അക്ഷരം *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	വേണ്ടി (auto സ്ഥിരം &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		എങ്കിൽ (name.compare(0, prefix_len, prefix) != 0)
			തുടരുക;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	തിരികെ attributes;
}

YOSYS_NAMESPACE_END
