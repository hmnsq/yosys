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

#ضمن "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> فراغ ModuleHdlnameIndex::index_items(I begin, I end, Filter filter)
{
	لكل (; begin != end; ++begin) {
		auto ثابت &item = *begin;

		𐡄𐡍 (!filter(item))
			استمر;
		std::vector<IdString> path = parse_hdlname(item);
		𐡄𐡍 (!path.empty())
			lookup.emplace(item, tree.insert(path, item));
	}
}

فراغ ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

فراغ ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_items(wires.begin(), wires.end(), [](Wire *) { أرجع 𐡒𐡔𐡈; });
}

فراغ ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_items(cells.begin(), cells.end(), [](Cell *) { أرجع 𐡒𐡔𐡈; });
}

فراغ ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_items(cells.begin(), cells.end(), [](Cell *cell) { أرجع cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	لكل (; !cursor.is_root(); cursor = cursor.parent()) {
		𐡄𐡍 (!cursor.has_entry()) {
			result.push_back("");
			result.push_back("");
			استمر;
		}
		Cell *cell = cursor.entry().cell();
		𐡄𐡍 (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.push_back("");
			result.push_back("");
			استمر;
		}
		result.push_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.push_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.push_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	أرجع result;
}

ساكن ثابت حرف *attr_prefix(ScopeinfoAttrs attrs)
{
	بدل (attrs) {
	حالة ScopeinfoAttrs::Cell:
		أرجع "\\cell_";
	حالة ScopeinfoAttrs::Module:
		أرجع "\\module_";
	افتراضي:
		log_abort();
	}
}

منطقي scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString key = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	أرجع key != IdString::Null && scopeinfo->has_attribute(key);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString key = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	𐡄𐡍 (key == IdString::Null)
		أرجع RTLIL::Const();
	auto found = scopeinfo->attributes.find(key);
	𐡄𐡍 (found == scopeinfo->attributes.end())
		أرجع RTLIL::Const();
	أرجع found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ثابت حرف *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	لكل (auto ثابت &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		𐡄𐡍 (name.compare(0, prefix_len, prefix) != 0)
			استمر;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	أرجع attributes;
}

YOSYS_NAMESPACE_END
