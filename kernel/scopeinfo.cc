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

سانچو <قسم_نالو I, قسم_نالو Filter> فراغ Module_Hdlname_اشاريو::اشاريو_شيون(I begin, I پڇاڙي, Filter filter)
{
	لكل (; begin != پڇاڙي; ++begin) {
		خودڪار ثابت &شي = *begin;

		إذا (!filter(شي))
			استمر;
		std::ويڪٽر<IdString> رستو_2 = parse_hdlname(شي);
		إذا (!رستو_2.خالي_جايون())
			lookup.emplace(شي, وڻ.وجهو(رستو_2, شي));
	}
}

فراغ Module_Hdlname_اشاريو::اشاريو()
{
	اشاريو_wires();
	اشاريو_cells();
}

فراغ Module_Hdlname_اشاريو::اشاريو_wires()
{
	خودڪار wires = module->wires();
	اشاريو_شيون(wires.begin(), wires.پڇاڙي(), [](Wire *) { أرجع سچ; });
}

فراغ Module_Hdlname_اشاريو::اشاريو_cells()
{
	خودڪار cells = module->cells();
	اشاريو_شيون(cells.begin(), cells.پڇاڙي(), [](Cell *) { أرجع سچ; });
}

فراغ Module_Hdlname_اشاريو::اشاريو_scopeinfo_cells()
{
	خودڪار cells = module->cells();
	اشاريو_شيون(cells.begin(), cells.پڇاڙي(), [](Cell *cell) { أرجع cell->type == ID($scopeinfo); });
}

std::ويڪٽر<std::string> Module_Hdlname_اشاريو::scope_sources(Cursor cursor)
{
	std::ويڪٽر<std::string> نتيجو;

	لكل (; !cursor.is_پاڙ(); cursor = cursor.والد()) {
		إذا (!cursor.has_داخلا_2()) {
			نتيجو.ڌڪو_پٺ("");
			نتيجو.ڌڪو_پٺ("");
			استمر;
		}
		Cell *cell = cursor.داخلا_2().cell();
		إذا (cell == خالي_اشارو || cell->type != ID($scopeinfo)) {
			نتيجو.ڌڪو_پٺ("");
			نتيجو.ڌڪو_پٺ("");
			استمر;
		}
		نتيجو.ڌڪو_پٺ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		نتيجو.ڌڪو_پٺ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	نتيجو.ڌڪو_پٺ(module->get_src_attribute());

	std::ابتو(نتيجو.begin(), نتيجو.پڇاڙي());

	أرجع نتيجو;
}

ساكن ثابت حرف *attr_اڳياڙي(ScopeinfoAttrs attrs)
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
	IdString ڪنجي = twines.ڳوليو(attr_اڳياڙي(attrs) + twines.unescaped_str(id));
	أرجع ڪنجي != IdString::Null && scopeinfo->has_attribute(ڪنجي);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ڪنجي = twines.ڳوليو(attr_اڳياڙي(attrs) + twines.unescaped_str(id));
	إذا (ڪنجي == IdString::Null)
		أرجع RTLIL::Const();
	خودڪار مليو = scopeinfo->خاصيتون.ڳوليو(ڪنجي);
	إذا (مليو == scopeinfo->خاصيتون.پڇاڙي())
		أرجع RTLIL::Const();
	أرجع مليو->ٻيو;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_خاصيتون(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> خاصيتون;

	ثابت حرف *اڳياڙي_2 = attr_اڳياڙي(attrs);
	سائيز_t اڳياڙي_len = لڙي_ڊيگهه(اڳياڙي_2);
	TwinePool &twines = scopeinfo->twines();

	لكل (خودڪار ثابت &داخلا_2 : scopeinfo->خاصيتون) {
		std::string name = twines.str(داخلا_2.پهريون);
		إذا (name.compare(0, اڳياڙي_len, اڳياڙي_2) != 0)
			استمر;
		خاصيتون.emplace(twines.جوڙيو(RTLIL::escape_id(name.substr(اڳياڙي_len))), داخلا_2.ٻيو);
	}

	أرجع خاصيتون;
}

YOSYS_NAMESPACE_پڇاڙي
