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

کينډۍ <ډول_نوم I, ډول_نوم Filter> فراغ Module_Hdlname_شاخص::شاخص_توکي(I begin, I پای_ټکی, Filter filter)
{
	لكل (; begin != پای_ټکی; ++begin) {
		خپلکاری ثابت &توکی = *begin;

		إذا (!filter(توکی))
			استمر;
		std::ویکټور<IdString> لار_2 = parse_hdlname(توکی);
		إذا (!لار_2.تشه())
			lookup.emplace(توکی, ونه.ورننباسه(لار_2, توکی));
	}
}

فراغ Module_Hdlname_شاخص::شاخص()
{
	شاخص_wires();
	شاخص_cells();
}

فراغ Module_Hdlname_شاخص::شاخص_wires()
{
	خپلکاری wires = module->wires();
	شاخص_توکي(wires.begin(), wires.پای_ټکی(), [](Wire *) { أرجع ریښتیا; });
}

فراغ Module_Hdlname_شاخص::شاخص_cells()
{
	خپلکاری cells = module->cells();
	شاخص_توکي(cells.begin(), cells.پای_ټکی(), [](Cell *) { أرجع ریښتیا; });
}

فراغ Module_Hdlname_شاخص::شاخص_scopeinfo_cells()
{
	خپلکاری cells = module->cells();
	شاخص_توکي(cells.begin(), cells.پای_ټکی(), [](Cell *cell) { أرجع cell->type == ID($scopeinfo); });
}

std::ویکټور<std::string> Module_Hdlname_شاخص::scope_sources(Cursor cursor)
{
	std::ویکټور<std::string> پایله;

	لكل (; !cursor.is_ریښه(); cursor = cursor.پلار()) {
		إذا (!cursor.has_ثبت()) {
			پایله.ټېل_شا("");
			پایله.ټېل_شا("");
			استمر;
		}
		Cell *cell = cursor.ثبت().cell();
		إذا (cell == تش_اشاره || cell->type != ID($scopeinfo)) {
			پایله.ټېل_شا("");
			پایله.ټېل_شا("");
			استمر;
		}
		پایله.ټېل_شا(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		پایله.ټېل_شا(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	پایله.ټېل_شا(module->get_src_attribute());

	std::سرچپه(پایله.begin(), پایله.پای_ټکی());

	أرجع پایله;
}

ساكن ثابت حرف *attr_مختاړی(ScopeinfoAttrs attrs)
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
	IdString کلی = twines.پیدا_کړه(attr_مختاړی(attrs) + twines.unescaped_str(id));
	أرجع کلی != IdString::Null && scopeinfo->has_attribute(کلی);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلی = twines.پیدا_کړه(attr_مختاړی(attrs) + twines.unescaped_str(id));
	إذا (کلی == IdString::Null)
		أرجع RTLIL::Const();
	خپلکاری وموندل = scopeinfo->ځانګړتیاوې.پیدا_کړه(کلی);
	إذا (وموندل == scopeinfo->ځانګړتیاوې.پای_ټکی())
		أرجع RTLIL::Const();
	أرجع وموندل->دوهم;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ځانګړتیاوې(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> ځانګړتیاوې;

	ثابت حرف *مختاړی_2 = attr_مختاړی(attrs);
	اندازه_t مختاړی_len = مزي_اوږدوالی(مختاړی_2);
	TwinePool &twines = scopeinfo->twines();

	لكل (خپلکاری ثابت &ثبت : scopeinfo->ځانګړتیاوې) {
		std::string name = twines.str(ثبت.لومړی);
		إذا (name.compare(0, مختاړی_len, مختاړی_2) != 0)
			استمر;
		ځانګړتیاوې.emplace(twines.جمع_کړه(RTLIL::escape_id(name.substr(مختاړی_len))), ثبت.دوهم);
	}

	أرجع ځانګړتیاوې;
}

YOSYS_NAMESPACE_پای_ټکی
