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

#ಅನ್ತರ್ಭಾವ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

template <typename I, typename Filter> ಶೂನ್ಯ ModuleHdlnameIndex::index_ವಸ್ತುಗಳು(I begin, I end, Filter filter)
{
	ಪ್ರತಿ (; begin != end; ++begin) {
		auto ಧ್ರುವ &item = *begin;

		ಯದಿ (!filter(item))
			ಅನುವರ್ತ;
		std::vector<IdString> path = parse_hdlname(item);
		ಯದಿ (!path.empty())
			lookup.emplace(item, ವೃಕ್ಷ.ಸೇರಿಸು(path, item));
	}
}

ಶೂನ್ಯ ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

ಶೂನ್ಯ ModuleHdlnameIndex::index_wires()
{
	auto wires = module->wires();
	index_ವಸ್ತುಗಳು(wires.begin(), wires.end(), [](Wire *) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; });
}

ಶೂನ್ಯ ModuleHdlnameIndex::index_cells()
{
	auto cells = module->cells();
	index_ವಸ್ತುಗಳು(cells.begin(), cells.end(), [](Cell *) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; });
}

ಶೂನ್ಯ ModuleHdlnameIndex::index_scopeinfo_cells()
{
	auto cells = module->cells();
	index_ವಸ್ತುಗಳು(cells.begin(), cells.end(), [](Cell *cell) { ಪ್ರತ್ಯಾಗಮ cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	ಪ್ರತಿ (; !cursor.is_ಬೇರು(); cursor = cursor.parent()) {
		ಯದಿ (!cursor.has_entry()) {
			result.ತಳ್ಳು_back("");
			result.ತಳ್ಳು_back("");
			ಅನುವರ್ತ;
		}
		Cell *cell = cursor.entry().cell();
		ಯದಿ (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.ತಳ್ಳು_back("");
			result.ತಳ್ಳು_back("");
			ಅನುವರ್ತ;
		}
		result.ತಳ್ಳು_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.ತಳ್ಳು_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.ತಳ್ಳು_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *attr_prefix(ScopeinfoAttrs attrs)
{
	ವಿಕಲ್ಪ (attrs) {
	ಪಕ್ಷ ScopeinfoAttrs::Cell:
		ಪ್ರತ್ಯಾಗಮ "\\cell_";
	ಪಕ್ಷ ScopeinfoAttrs::Module:
		ಪ್ರತ್ಯಾಗಮ "\\module_";
	ಸ್ವಭಾವ:
		log_abort();
	}
}

ತಾರ್ಕಿಕ scopeinfo_has_attribute(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ಕೀಲಿ = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ != IdString::Null && scopeinfo->has_attribute(ಕೀಲಿ);
}

RTLIL::Const scopeinfo_get_attribute(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ಕೀಲಿ = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	ಯದಿ (ಕೀಲಿ == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const();
	auto found = scopeinfo->attributes.find(ಕೀಲಿ);
	ಯದಿ (found == scopeinfo->attributes.end())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const();
	ಪ್ರತ್ಯಾಗಮ found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	ಧ್ರುವ ವರ್ಣ *prefix = attr_prefix(attrs);
	size_t prefix_len = strlen(prefix);
	TwinePool &twines = scopeinfo->twines();

	ಪ್ರತಿ (auto ಧ್ರುವ &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		ಯದಿ (name.compare(0, prefix_len, prefix) != 0)
			ಅನುವರ್ತ;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	ಪ್ರತ್ಯಾಗಮ attributes;
}

YOSYS_NAMESPACE_END
