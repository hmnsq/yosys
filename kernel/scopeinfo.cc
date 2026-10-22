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

#include "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> void ModuleHdlnameIndex::index_items(I begin, I end, Filter filter)
{
	cho (; begin != end; ++begin) {
		auto const &item = *begin;

		nếu (!filter(item))
			continue;
		std::vector<IdString> path = parse_hdlname(item);
		nếu (!path.empty())
			lookup.emplace(item, tree.insert(path, item));
	}
}

void ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

void ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_items(wires.begin(), wires.end(), [](Wire *) { trả_về true; });
}

void ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_items(cells.begin(), cells.end(), [](Cell *) { trả_về true; });
}

void ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_items(cells.begin(), cells.end(), [](Cell *cell) { trả_về cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	cho (; !cursor.is_root(); cursor = cursor.parent()) {
		nếu (!cursor.has_entry()) {
			result.push_back("");
			result.push_back("");
			continue;
		}
		Cell *cell = cursor.entry().cell();
		nếu (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.push_back("");
			result.push_back("");
			continue;
		}
		result.push_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.push_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.push_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	trả_về result;
}

static const char *attr_prefix(ScopeinfoAttrs attrs)
{
	switch (attrs) {
	case ScopeinfoAttrs::Cell:
		trả_về "\\cell_";
	case ScopeinfoAttrs::Module:
		trả_về "\\module_";
	default:
		log_abort();
	}
}

bool scopeinfo_has_attribute(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString key = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	trả_về key != IdString::Null && scopeinfo->has_attribute(key);
}

RTLIL::Const scopeinfo_get_attribute(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString key = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	nếu (key == IdString::Null)
		trả_về RTLIL::Const();
	auto found = scopeinfo->attributes.find(key);
	nếu (found == scopeinfo->attributes.end())
		trả_về RTLIL::Const();
	trả_về found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	const char *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	cho (auto const &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		nếu (name.compare(0, prefix_len, prefix) != 0)
			continue;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	trả_về attributes;
}

YOSYS_NAMESPACE_END
