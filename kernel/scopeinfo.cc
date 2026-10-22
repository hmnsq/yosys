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

قالب <نام_نوع I, نام_نوع Filter> فراغ Module_Hdlname_شاخص::شاخص_اقلام(I begin, I انتها, Filter filter)
{
	لكل (; begin != انتها; ++begin) {
		خودکار ثابت &قلم = *begin;

		إذا (!filter(قلم))
			استمر;
		std::وکتور<IdString> مسیر_2 = parse_hdlname(قلم);
		إذا (!مسیر_2.خالی())
			lookup.emplace(قلم, درخت.درج(مسیر_2, قلم));
	}
}

فراغ Module_Hdlname_شاخص::شاخص()
{
	شاخص_wires();
	شاخص_cells();
}

فراغ Module_Hdlname_شاخص::شاخص_wires()
{
	خودکار wires = module->wires();
	شاخص_اقلام(wires.begin(), wires.انتها(), [](Wire *) { أرجع درست; });
}

فراغ Module_Hdlname_شاخص::شاخص_cells()
{
	خودکار cells = module->cells();
	شاخص_اقلام(cells.begin(), cells.انتها(), [](Cell *) { أرجع درست; });
}

فراغ Module_Hdlname_شاخص::شاخص_scopeinfo_cells()
{
	خودکار cells = module->cells();
	شاخص_اقلام(cells.begin(), cells.انتها(), [](Cell *cell) { أرجع cell->type == ID($scopeinfo); });
}

std::وکتور<std::string> Module_Hdlname_شاخص::scope_sources(Cursor cursor)
{
	std::وکتور<std::string> نتیجه;

	لكل (; !cursor.is_ریشه(); cursor = cursor.والد()) {
		إذا (!cursor.has_مدخل()) {
			نتیجه.افزودن_پشت("");
			نتیجه.افزودن_پشت("");
			استمر;
		}
		Cell *cell = cursor.مدخل().cell();
		إذا (cell == اشاره_تهی || cell->type != ID($scopeinfo)) {
			نتیجه.افزودن_پشت("");
			نتیجه.افزودن_پشت("");
			استمر;
		}
		نتیجه.افزودن_پشت(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		نتیجه.افزودن_پشت(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	نتیجه.افزودن_پشت(module->get_src_attribute());

	std::معکوس(نتیجه.begin(), نتیجه.انتها());

	أرجع نتیجه;
}

ساكن ثابت حرف *attr_پیشوند(ScopeinfoAttrs attrs)
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
	IdString کلید = twines.پیدا_کن(attr_پیشوند(attrs) + twines.unescaped_str(id));
	أرجع کلید != IdString::Null && scopeinfo->has_attribute(کلید);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.پیدا_کن(attr_پیشوند(attrs) + twines.unescaped_str(id));
	إذا (کلید == IdString::Null)
		أرجع RTLIL::Const();
	خودکار یافت = scopeinfo->صفات.پیدا_کن(کلید);
	إذا (یافت == scopeinfo->صفات.انتها())
		أرجع RTLIL::Const();
	أرجع یافت->دومی;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_صفات(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> صفات;

	ثابت حرف *پیشوند_2 = attr_پیشوند(attrs);
	اندازه_t پیشوند_len = طول_رشته(پیشوند_2);
	TwinePool &twines = scopeinfo->twines();

	لكل (خودکار ثابت &مدخل : scopeinfo->صفات) {
		std::string name = twines.str(مدخل.اولی);
		إذا (name.compare(0, پیشوند_len, پیشوند_2) != 0)
			استمر;
		صفات.emplace(twines.جمع_کن(RTLIL::escape_id(name.substr(پیشوند_len))), مدخل.دومی);
	}

	أرجع صفات;
}

YOSYS_NAMESPACE_انتها
