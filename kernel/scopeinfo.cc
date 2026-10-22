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

#অন্তর্ভাব "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> শূন্য Module_Hdlname_সূচক::সূচক_বস্তুসমূহ(I begin, I end, Filter filter)
{
	জন্য (; begin != end; ++begin) {
		auto ধ্রুবক &item = *begin;

		যদি (!filter(item))
			চালু;
		std::vector<IdString> path = parse_hdlname(item);
		যদি (!path.empty())
			lookup.emplace(item, বৃক্ষ.ঢোকাও(path, item));
	}
}

শূন্য Module_Hdlname_সূচক::সূচক()
{
	সূচক_wires();
	সূচক_cells();
}

শূন্য Module_Hdlname_সূচক::সূচক_wires()
{
	auto wires = module->wires();
	সূচক_বস্তুসমূহ(wires.begin(), wires.end(), [](Wire *) { ফেরত সত্য; });
}

শূন্য Module_Hdlname_সূচক::সূচক_cells()
{
	auto cells = module->cells();
	সূচক_বস্তুসমূহ(cells.begin(), cells.end(), [](Cell *) { ফেরত সত্য; });
}

শূন্য Module_Hdlname_সূচক::সূচক_scopeinfo_cells()
{
	auto cells = module->cells();
	সূচক_বস্তুসমূহ(cells.begin(), cells.end(), [](Cell *cell) { ফেরত cell->type == ID($scopeinfo); });
}

std::vector<std::string> Module_Hdlname_সূচক::scope_sources(Cursor cursor)
{
	std::vector<std::string> ফলাফল;

	জন্য (; !cursor.is_মূল(); cursor = cursor.parent()) {
		যদি (!cursor.has_entry()) {
			ফলাফল.ঠেলো_back("");
			ফলাফল.ঠেলো_back("");
			চালু;
		}
		Cell *cell = cursor.entry().cell();
		যদি (cell == nullptr || cell->type != ID($scopeinfo)) {
			ফলাফল.ঠেলো_back("");
			ফলাফল.ঠেলো_back("");
			চালু;
		}
		ফলাফল.ঠেলো_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		ফলাফল.ঠেলো_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	ফলাফল.ঠেলো_back(module->get_src_attribute());

	std::reverse(ফলাফল.begin(), ফলাফল.end());

	ফেরত ফলাফল;
}

স্থির ধ্রুবক অক্ষর *attr_prefix(ScopeinfoAttrs attrs)
{
	নির্বাচন (attrs) {
	ক্ষেত্র ScopeinfoAttrs::Cell:
		ফেরত "\\cell_";
	ক্ষেত্র ScopeinfoAttrs::Module:
		ফেরত "\\module_";
	পূর্বনির্ধারিত:
		log_abort();
	}
}

যৌক্তিক scopeinfo_has_attribute(ধ্রুবক RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString চাবি = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ফেরত চাবি != IdString::Null && scopeinfo->has_attribute(চাবি);
}

RTLIL::Const scopeinfo_get_attribute(ধ্রুবক RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString চাবি = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	যদি (চাবি == IdString::Null)
		ফেরত RTLIL::Const();
	auto found = scopeinfo->attributes.find(চাবি);
	যদি (found == scopeinfo->attributes.end())
		ফেরত RTLIL::Const();
	ফেরত found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ধ্রুবক RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ধ্রুবক অক্ষর *prefix = attr_prefix(attrs);
	আকার_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	জন্য (auto ধ্রুবক &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		যদি (name.compare(0, prefix_len, prefix) != 0)
			চালু;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	ফেরত attributes;
}

YOSYS_NAMESPACE_END
