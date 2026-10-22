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

template <typename I, typename Filter> शून्य ModuleHdlnameIndex::index_वस्तूनि(I begin, I end, Filter filter)
{
	अत्थाय (; begin != end; ++begin) {
		auto ध्रुव &item = *begin;

		सचे (!filter(item))
			अनुवर्त;
		std::vector<IdString> path = parse_hdlname(item);
		सचे (!path.empty())
			lookup.emplace(item, वृक्षः.निवेशय(path, item));
	}
}

शून्य ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

शून्य ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_वस्तूनि(wires.begin(), wires.end(), [](Wire *) { पटिनिवत्त सच्च; });
}

शून्य ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_वस्तूनि(cells.begin(), cells.end(), [](Cell *) { पटिनिवत्त सच्च; });
}

शून्य ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_वस्तूनि(cells.begin(), cells.end(), [](Cell *cell) { पटिनिवत्त cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	अत्थाय (; !cursor.is_मूलम्(); cursor = cursor.parent()) {
		सचे (!cursor.has_entry()) {
			result.प्रक्षिप_back("");
			result.प्रक्षिप_back("");
			अनुवर्त;
		}
		Cell *cell = cursor.entry().cell();
		सचे (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.प्रक्षिप_back("");
			result.प्रक्षिप_back("");
			अनुवर्त;
		}
		result.प्रक्षिप_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.प्रक्षिप_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.प्रक्षिप_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	पटिनिवत्त result;
}

स्थिर ध्रुव वर्ण *attr_prefix(ScopeinfoAttrs attrs)
{
	विकल्प (attrs) {
	पक्ष ScopeinfoAttrs::Cell:
		पटिनिवत्त "\\cell_";
	पक्ष ScopeinfoAttrs::Module:
		पटिनिवत्त "\\module_";
	स्वभाव:
		log_abort();
	}
}

तार्किक scopeinfo_has_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString कुञ्चिका = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	पटिनिवत्त कुञ्चिका != IdString::Null && scopeinfo->has_attribute(कुञ्चिका);
}

RTLIL::Const scopeinfo_get_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString कुञ्चिका = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	सचे (कुञ्चिका == IdString::Null)
		पटिनिवत्त RTLIL::Const();
	auto found = scopeinfo->attributes.find(कुञ्चिका);
	सचे (found == scopeinfo->attributes.end())
		पटिनिवत्त RTLIL::Const();
	पटिनिवत्त found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ध्रुव वर्ण *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	अत्थाय (auto ध्रुव &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		सचे (name.compare(0, prefix_len, prefix) != 0)
			अनुवर्त;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	पटिनिवत्त attributes;
}

YOSYS_NAMESPACE_END
