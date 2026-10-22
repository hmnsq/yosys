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

प्रतिरूपम् <प्रकारनाम I, प्रकारनाम Filter> शून्य Module_Hdlname_सूचकांक::सूचकांक_वस्तूनि(I begin, I अन्तः, Filter filter)
{
	प्रति (; begin != अन्तः; ++begin) {
		स्वचालित ध्रुव &item = *begin;

		यदि (!filter(item))
			अनुवर्त;
		std::सदिशम्<IdString> मार्गः_2 = parse_hdlname(item);
		यदि (!मार्गः_2.रिक्तानि())
			lookup.emplace(item, वृक्षः.निवेशय(मार्गः_2, item));
	}
}

शून्य Module_Hdlname_सूचकांक::सूचकांक()
{
	सूचकांक_wires();
	सूचकांक_cells();
}

शून्य Module_Hdlname_सूचकांक::सूचकांक_wires()
{
	स्वचालित wires = module->wires();
	सूचकांक_वस्तूनि(wires.begin(), wires.अन्तः(), [](Wire *) { प्रत्यागम सत्य; });
}

शून्य Module_Hdlname_सूचकांक::सूचकांक_cells()
{
	स्वचालित cells = module->cells();
	सूचकांक_वस्तूनि(cells.begin(), cells.अन्तः(), [](Cell *) { प्रत्यागम सत्य; });
}

शून्य Module_Hdlname_सूचकांक::सूचकांक_scopeinfo_cells()
{
	स्वचालित cells = module->cells();
	सूचकांक_वस्तूनि(cells.begin(), cells.अन्तः(), [](Cell *cell) { प्रत्यागम cell->type == ID($scopeinfo); });
}

std::सदिशम्<std::string> Module_Hdlname_सूचकांक::scope_sources(Cursor cursor)
{
	std::सदिशम्<std::string> परिणाम;

	प्रति (; !cursor.is_मूलम्(); cursor = cursor.parent()) {
		यदि (!cursor.has_प्रविष्टिः()) {
			परिणाम.प्रक्षिप_पृष्ठभागः("");
			परिणाम.प्रक्षिप_पृष्ठभागः("");
			अनुवर्त;
		}
		Cell *cell = cursor.प्रविष्टिः().cell();
		यदि (cell == शून्यसूचकः || cell->type != ID($scopeinfo)) {
			परिणाम.प्रक्षिप_पृष्ठभागः("");
			परिणाम.प्रक्षिप_पृष्ठभागः("");
			अनुवर्त;
		}
		परिणाम.प्रक्षिप_पृष्ठभागः(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		परिणाम.प्रक्षिप_पृष्ठभागः(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	परिणाम.प्रक्षिप_पृष्ठभागः(module->get_src_attribute());

	std::reverse(परिणाम.begin(), परिणाम.अन्तः());

	प्रत्यागम परिणाम;
}

स्थिर ध्रुव वर्ण *attr_उपसर्गः(ScopeinfoAttrs attrs)
{
	विकल्प (attrs) {
	पक्ष ScopeinfoAttrs::Cell:
		प्रत्यागम "\\cell_";
	पक्ष ScopeinfoAttrs::Module:
		प्रत्यागम "\\module_";
	स्वभाव:
		log_abort();
	}
}

तार्किक scopeinfo_has_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString कुञ्चिका = twines.find(attr_उपसर्गः(attrs) + twines.unescaped_str(id));
	प्रत्यागम कुञ्चिका != IdString::Null && scopeinfo->has_attribute(कुञ्चिका);
}

RTLIL::Const scopeinfo_get_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString कुञ्चिका = twines.find(attr_उपसर्गः(attrs) + twines.unescaped_str(id));
	यदि (कुञ्चिका == IdString::Null)
		प्रत्यागम RTLIL::Const();
	स्वचालित प्राप्तम् = scopeinfo->गुणाः.find(कुञ्चिका);
	यदि (प्राप्तम् == scopeinfo->गुणाः.अन्तः())
		प्रत्यागम RTLIL::Const();
	प्रत्यागम प्राप्तम्->second;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_गुणाः(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> गुणाः;

	ध्रुव वर्ण *उपसर्गः_2 = attr_उपसर्गः(attrs);
	आकार_t उपसर्गः_len = वाक्यदैर्घ्यम्(उपसर्गः_2);
	TwinePool &twines = scopeinfo->twines();

	प्रति (स्वचालित ध्रुव &प्रविष्टिः : scopeinfo->गुणाः) {
		std::string name = twines.str(प्रविष्टिः.first);
		यदि (name.compare(0, उपसर्गः_len, उपसर्गः_2) != 0)
			अनुवर्त;
		गुणाः.emplace(twines.सङ्कलय(RTLIL::escape_id(name.substr(उपसर्गः_len))), प्रविष्टिः.second);
	}

	प्रत्यागम गुणाः;
}

YOSYS_NAMESPACE_अन्तः
