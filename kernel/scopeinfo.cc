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

#અન્તર્ભાવ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> શૂન્ય ModuleHdlnameIndex::index_વસ્તુઓ(I begin, I end, Filter filter)
{
	માટે (; begin != end; ++begin) {
		auto અચલ &item = *begin;

		જો (!filter(item))
			ચાલુ;
		std::vector<IdString> path = parse_hdlname(item);
		જો (!path.empty())
			lookup.emplace(item, વૃક્ષ.ઉમેરો(path, item));
	}
}

શૂન્ય ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

શૂન્ય ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_વસ્તુઓ(wires.begin(), wires.end(), [](Wire *) { પાછા સાચું; });
}

શૂન્ય ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_વસ્તુઓ(cells.begin(), cells.end(), [](Cell *) { પાછા સાચું; });
}

શૂન્ય ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_વસ્તુઓ(cells.begin(), cells.end(), [](Cell *cell) { પાછા cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	માટે (; !cursor.is_મૂળ(); cursor = cursor.parent()) {
		જો (!cursor.has_entry()) {
			result.ધકેલો_back("");
			result.ધકેલો_back("");
			ચાલુ;
		}
		Cell *cell = cursor.entry().cell();
		જો (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.ધકેલો_back("");
			result.ધકેલો_back("");
			ચાલુ;
		}
		result.ધકેલો_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.ધકેલો_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.ધકેલો_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	પાછા result;
}

સ્થિર અચલ અક્ષર *attr_prefix(ScopeinfoAttrs attrs)
{
	પસંદગી (attrs) {
	સ્થિતિ ScopeinfoAttrs::Cell:
		પાછા "\\cell_";
	સ્થિતિ ScopeinfoAttrs::Module:
		પાછા "\\module_";
	મૂળભૂત:
		log_abort();
	}
}

તાર્કિક scopeinfo_has_attribute(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ચાવી = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	પાછા ચાવી != IdString::Null && scopeinfo->has_attribute(ચાવી);
}

RTLIL::Const scopeinfo_get_attribute(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ચાવી = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	જો (ચાવી == IdString::Null)
		પાછા RTLIL::Const();
	auto found = scopeinfo->attributes.find(ચાવી);
	જો (found == scopeinfo->attributes.end())
		પાછા RTLIL::Const();
	પાછા found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	અચલ અક્ષર *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	માટે (auto અચલ &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		જો (name.compare(0, prefix_len, prefix) != 0)
			ચાલુ;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	પાછા attributes;
}

YOSYS_NAMESPACE_END
