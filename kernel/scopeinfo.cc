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

ढाँचा <typename I, typename Filter> शून्य ModuleHdlnameIndex::index_वस्तुसंच(I begin, I end, Filter filter)
{
	साठी (; begin != end; ++begin) {
		स्वतः स्थिरांक &item = *begin;

		जर (!filter(item))
			सुरूठेवा;
		std::vector<IdString> path = parse_hdlname(item);
		जर (!path.empty())
			lookup.emplace(item, वृक्ष.घाला(path, item));
	}
}

शून्य ModuleHdlnameIndex::index()
{
	index_wires();
	index_cells();
}

शून्य ModuleHdlnameIndex::index_wires()
{
	स्वतः wires = module->wires();
	index_वस्तुसंच(wires.begin(), wires.end(), [](Wire *) { परत सत्य; });
}

शून्य ModuleHdlnameIndex::index_cells()
{
	स्वतः cells = module->cells();
	index_वस्तुसंच(cells.begin(), cells.end(), [](Cell *) { परत सत्य; });
}

शून्य ModuleHdlnameIndex::index_scopeinfo_cells()
{
	स्वतः cells = module->cells();
	index_वस्तुसंच(cells.begin(), cells.end(), [](Cell *cell) { परत cell->type == ID($scopeinfo); });
}

std::vector<std::string> ModuleHdlnameIndex::scope_sources(Cursor cursor)
{
	std::vector<std::string> result;

	साठी (; !cursor.is_मूळ(); cursor = cursor.parent()) {
		जर (!cursor.has_entry()) {
			result.ढकला_back("");
			result.ढकला_back("");
			सुरूठेवा;
		}
		Cell *cell = cursor.entry().cell();
		जर (cell == nullptr || cell->type != ID($scopeinfo)) {
			result.ढकला_back("");
			result.ढकला_back("");
			सुरूठेवा;
		}
		result.ढकला_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		result.ढकला_back(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	result.ढकला_back(module->get_src_attribute());

	std::reverse(result.begin(), result.end());

	परत result;
}

स्थिर स्थिरांक अक्षर *attr_prefix(ScopeinfoAttrs attrs)
{
	निवड (attrs) {
	स्थिती ScopeinfoAttrs::Cell:
		परत "\\cell_";
	स्थिती ScopeinfoAttrs::Module:
		परत "\\module_";
	पूर्वनिर्धारित:
		log_abort();
	}
}

तार्किक scopeinfo_has_attribute(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString किल्ली = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	परत किल्ली != IdString::Null && scopeinfo->has_attribute(किल्ली);
}

RTLIL::Const scopeinfo_get_attribute(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString किल्ली = twines.find(attr_prefix(attrs) + twines.unescaped_str(id));
	जर (किल्ली == IdString::Null)
		परत RTLIL::Const();
	स्वतः found = scopeinfo->attributes.find(किल्ली);
	जर (found == scopeinfo->attributes.end())
		परत RTLIL::Const();
	परत found->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	स्थिरांक अक्षर *prefix = attr_prefix(attrs);
	माप_प्रकार prefix_len = म_माप(prefix);
	TwinePool &twines = scopeinfo->twines();

	साठी (स्वतः स्थिरांक &entry : scopeinfo->attributes) {
		std::string name = twines.str(entry.first);
		जर (name.compare(0, prefix_len, prefix) != 0)
			सुरूठेवा;
		attributes.emplace(twines.add(RTLIL::escape_id(name.substr(prefix_len))), entry.second);
	}

	परत attributes;
}

YOSYS_NAMESPACE_END
