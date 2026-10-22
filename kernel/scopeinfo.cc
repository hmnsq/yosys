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

#ଅନ୍ତର୍ଭାଵ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> ଶୂନ୍ୟ ModuleHdlnameIndex::index_ବସ୍ତୁଗୁଡ଼ିକ(I begin, I end, Filter filter)
{
	ପାଇଁ (; begin != end; ++begin) {
		auto ଅଚଳ &item = *begin;

		ଯଦି (!filter(item))
			ଜାରି;
		std::vector<IdString> path = parse_hdlname(item);
		ଯଦି (!path.empty())
			lookup.emplace(item, ବୃକ୍ଷ.ଭର୍ତ୍ତି(path, item));
	}
}

ଶୂନ୍ୟ ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

ଶୂନ୍ୟ ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_ବସ୍ତୁଗୁଡ଼ିକ(wires.begin(), wires.end(), [](Wire *) { ଫେରନ୍ତୁ ସତ୍ୟ; });
}

ଶୂନ୍ୟ ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_ବସ୍ତୁଗୁଡ଼ିକ(cells.begin(), cells.end(), [](Cell *) { ଫେରନ୍ତୁ ସତ୍ୟ; });
}

ଶୂନ୍ୟ ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_ବସ୍ତୁଗୁଡ଼ିକ(cells.begin(), cells.end(), [](Cell *cell) { ଫେରନ୍ତୁ cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	ପାଇଁ (; !cursor.is_ମୂଳ(); cursor = cursor.parent()) {
		ଯଦି (!cursor.has_entry()) {
			result.ଠେଲ_back("");
			result.ଠେଲ_back("");
			ଜାରି;
		}
		Cell *cell = cursor.entry().cell();
		ଯଦି (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.ଠେଲ_back("");
			result.ଠେଲ_back("");
			ଜାରି;
		}
		result.ଠେଲ_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.ଠେଲ_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.ଠେଲ_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *attr_prefix(ScopeinfoAttrs attrs)
{
	ବାଛ (attrs) {
	ସ୍ଥିତି ScopeinfoAttrs::Cell:
		ଫେରନ୍ତୁ "\\cell_";
	ସ୍ଥିତି ScopeinfoAttrs::Module:
		ଫେରନ୍ତୁ "\\module_";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		log_abort();
	}
}

ତାର୍କିକ scopeinfo_has_attribute(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ଚାବି = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ଫେରନ୍ତୁ ଚାବି != IdString::Null && scopeinfo->has_attribute(ଚାବି);
}

RTLIL::Const scopeinfo_get_attribute(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ଚାବି = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ଯଦି (ଚାବି == IdString::Null)
		ଫେରନ୍ତୁ RTLIL::Const();
	auto found = scopeinfo->attributes.find(ଚାବି);
	ଯଦି (found == scopeinfo->attributes.end())
		ଫେରନ୍ତୁ RTLIL::Const();
	ଫେରନ୍ତୁ found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ଅଚଳ ଅକ୍ଷର *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	ପାଇଁ (auto ଅଚଳ &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		ଯଦି (name.compare(0, prefix_len, prefix) != 0)
			ଜାରି;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	ଫେରନ୍ତୁ attributes;
}

YOSYS_NAMESPACE_END
