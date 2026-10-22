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

#شامل "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> تهی ModuleHdlnameIndex::index_اقلام(I begin, I end, Filter filter)
{
	برای (; begin != end; ++begin) {
		auto ثابت &item = *begin;

		اگر (!filter(item))
			ادامه;
		std::vector<IdString> path = parse_hdlname(item);
		اگر (!path.empty())
			lookup.emplace(item, درخت.درج(path, item));
	}
}

تهی ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

تهی ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_اقلام(wires.begin(), wires.end(), [](Wire *) { بازگشت درست; });
}

تهی ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_اقلام(cells.begin(), cells.end(), [](Cell *) { بازگشت درست; });
}

تهی ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_اقلام(cells.begin(), cells.end(), [](Cell *cell) { بازگشت cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	برای (; !cursor.is_ریشه(); cursor = cursor.parent()) {
		اگر (!cursor.has_entry()) {
			result.افزودن_back("");
			result.افزودن_back("");
			ادامه;
		}
		Cell *cell = cursor.entry().cell();
		اگر (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.افزودن_back("");
			result.افزودن_back("");
			ادامه;
		}
		result.افزودن_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.افزودن_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.افزودن_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	بازگشت result;
}

ایستا ثابت نویسه *attr_prefix(ScopeinfoAttrs attrs)
{
	انتخاب (attrs) {
	حالت ScopeinfoAttrs::Cell:
		بازگشت "\\cell_";
	حالت ScopeinfoAttrs::Module:
		بازگشت "\\module_";
	پیشفرض:
		log_abort();
	}
}

منطقی scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	بازگشت کلید != IdString::Null && scopeinfo->has_attribute(کلید);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	اگر (کلید == IdString::Null)
		بازگشت RTLIL::Const();
	auto found = scopeinfo->attributes.find(کلید);
	اگر (found == scopeinfo->attributes.end())
		بازگشت RTLIL::Const();
	بازگشت found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ثابت نویسه *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	برای (auto ثابت &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		اگر (name.compare(0, prefix_len, prefix) != 0)
			ادامه;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	بازگشت attributes;
}

YOSYS_NAMESPACE_END
