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

#অন্তর্ভাৱ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> শূন্য ModuleHdlnameIndex::index_বস্তুসমূহ(I begin, I end, Filter filter)
{
	বাবে (; begin != end; ++begin) {
		auto ধ্রুৱ &item = *begin;

		যদি (!filter(item))
			অনুৱর্ত;
		std::vector<IdString> path = parse_hdlname(item);
		যদি (!path.empty())
			lookup.emplace(item, বৃক্ষ.ঢোকাও(path, item));
	}
}

শূন্য ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

শূন্য ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_বস্তুসমূহ(wires.begin(), wires.end(), [](Wire *) { ঘূৰাই সঁচা; });
}

শূন্য ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_বস্তুসমূহ(cells.begin(), cells.end(), [](Cell *) { ঘূৰাই সঁচা; });
}

শূন্য ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_বস্তুসমূহ(cells.begin(), cells.end(), [](Cell *cell) { ঘূৰাই cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	বাবে (; !cursor.is_মূল(); cursor = cursor.parent()) {
		যদি (!cursor.has_entry()) {
			result.ঠেলো_back("");
			result.ঠেলো_back("");
			অনুৱর্ত;
		}
		Cell *cell = cursor.entry().cell();
		যদি (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.ঠেলো_back("");
			result.ঠেলো_back("");
			অনুৱর্ত;
		}
		result.ঠেলো_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.ঠেলো_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.ঠেলো_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	ঘূৰাই result;
}

স্থির ধ্রুৱ ৱর্ণ *attr_prefix(ScopeinfoAttrs attrs)
{
	ৱিকল্প (attrs) {
	পক্ষ ScopeinfoAttrs::Cell:
		ঘূৰাই "\\cell_";
	পক্ষ ScopeinfoAttrs::Module:
		ঘূৰাই "\\module_";
	স্ৱভাৱ:
		log_abort();
	}
}

তার্কিক scopeinfo_has_attribute(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString চাবি = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ঘূৰাই চাবি != IdString::Null && scopeinfo->has_attribute(চাবি);
}

RTLIL::Const scopeinfo_get_attribute(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString চাবি = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	যদি (চাবি == IdString::Null)
		ঘূৰাই RTLIL::Const();
	auto found = scopeinfo->attributes.find(চাবি);
	যদি (found == scopeinfo->attributes.end())
		ঘূৰাই RTLIL::Const();
	ঘূৰাই found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ধ্রুৱ ৱর্ণ *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	বাবে (auto ধ্রুৱ &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		যদি (name.compare(0, prefix_len, prefix) != 0)
			অনুৱর্ত;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	ঘূৰাই attributes;
}

YOSYS_NAMESPACE_END
