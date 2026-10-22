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

#அந்தர்பாவ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> வெறுமை ModuleHdlnameIndex::index_பொருட்கள்(I begin, I end, Filter filter)
{
	க்கு (; begin != end; ++begin) {
		auto மாறிலி &item = *begin;

		எனில் (!filter(item))
			தொடர்;
		std::vector<IdString> path = parse_hdlname(item);
		எனில் (!path.empty())
			lookup.emplace(item, மரம்.செருகு(path, item));
	}
}

வெறுமை ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

வெறுமை ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_பொருட்கள்(wires.begin(), wires.end(), [](Wire *) { திரும்பு உண்மை; });
}

வெறுமை ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_பொருட்கள்(cells.begin(), cells.end(), [](Cell *) { திரும்பு உண்மை; });
}

வெறுமை ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_பொருட்கள்(cells.begin(), cells.end(), [](Cell *cell) { திரும்பு cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	க்கு (; !cursor.is_வேர்(); cursor = cursor.parent()) {
		எனில் (!cursor.has_entry()) {
			result.தள்ளு_back("");
			result.தள்ளு_back("");
			தொடர்;
		}
		Cell *cell = cursor.entry().cell();
		எனில் (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.தள்ளு_back("");
			result.தள்ளு_back("");
			தொடர்;
		}
		result.தள்ளு_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.தள்ளு_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.தள்ளு_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	திரும்பு result;
}

நிலையான மாறிலி எழுத்து *attr_prefix(ScopeinfoAttrs attrs)
{
	தேர்வு (attrs) {
	நிலை ScopeinfoAttrs::Cell:
		திரும்பு "\\cell_";
	நிலை ScopeinfoAttrs::Module:
		திரும்பு "\\module_";
	இயல்பு:
		log_abort();
	}
}

தருக்க scopeinfo_has_attribute(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString சாவி = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	திரும்பு சாவி != IdString::Null && scopeinfo->has_attribute(சாவி);
}

RTLIL::Const scopeinfo_get_attribute(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString சாவி = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	எனில் (சாவி == IdString::Null)
		திரும்பு RTLIL::Const();
	auto found = scopeinfo->attributes.find(சாவி);
	எனில் (found == scopeinfo->attributes.end())
		திரும்பு RTLIL::Const();
	திரும்பு found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	மாறிலி எழுத்து *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	க்கு (auto மாறிலி &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		எனில் (name.compare(0, prefix_len, prefix) != 0)
			தொடர்;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	திரும்பு attributes;
}

YOSYS_NAMESPACE_END
