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

سانچا <قسم_ناں I, قسم_ناں Filter> فراغ Module_Hdlname_اشاریہ::اشاریہ_اشیاء(I begin, I سرا, Filter filter)
{
	لكل (; begin != سرا; ++begin) {
		خودکار ثابت &شے = *begin;

		إذا (!filter(شے))
			استمر;
		std::سمتیہ<IdString> رستہ_2 = parse_hdlname(شے);
		إذا (!رستہ_2.خالی_تھاواں())
			lookup.emplace(شے, درخت.داخل_کرو(رستہ_2, شے));
	}
}

فراغ Module_Hdlname_اشاریہ::اشاریہ()
{
	اشاریہ_wires();
	اشاریہ_cells();
}

فراغ Module_Hdlname_اشاریہ::اشاریہ_wires()
{
	خودکار wires = module->wires();
	اشاریہ_اشیاء(wires.begin(), wires.سرا(), [](Wire *) { أرجع سچ; });
}

فراغ Module_Hdlname_اشاریہ::اشاریہ_cells()
{
	خودکار cells = module->cells();
	اشاریہ_اشیاء(cells.begin(), cells.سرا(), [](Cell *) { أرجع سچ; });
}

فراغ Module_Hdlname_اشاریہ::اشاریہ_scopeinfo_cells()
{
	خودکار cells = module->cells();
	اشاریہ_اشیاء(cells.begin(), cells.سرا(), [](Cell *cell) { أرجع cell->type == ID($scopeinfo); });
}

std::سمتیہ<std::string> Module_Hdlname_اشاریہ::scope_sources(Cursor cursor)
{
	std::سمتیہ<std::string> نتیجہ;

	لكل (; !cursor.is_جڑ(); cursor = cursor.ماپا()) {
		إذا (!cursor.has_اندراج()) {
			نتیجہ.ڈالو_پٹھ("");
			نتیجہ.ڈالو_پٹھ("");
			استمر;
		}
		Cell *cell = cursor.اندراج().cell();
		إذا (cell == خالی_سوچک || cell->type != ID($scopeinfo)) {
			نتیجہ.ڈالو_پٹھ("");
			نتیجہ.ڈالو_پٹھ("");
			استمر;
		}
		نتیجہ.ڈالو_پٹھ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		نتیجہ.ڈالو_پٹھ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	نتیجہ.ڈالو_پٹھ(module->get_src_attribute());

	std::الٹ(نتیجہ.begin(), نتیجہ.سرا());

	أرجع نتیجہ;
}

ساكن ثابت حرف *attr_اگیتر(ScopeinfoAttrs attrs)
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
	IdString کلید = twines.لبھو(attr_اگیتر(attrs) + twines.unescaped_str(id));
	أرجع کلید != IdString::Null && scopeinfo->has_attribute(کلید);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.لبھو(attr_اگیتر(attrs) + twines.unescaped_str(id));
	إذا (کلید == IdString::Null)
		أرجع RTLIL::Const();
	خودکار لبھیا = scopeinfo->گن.لبھو(کلید);
	إذا (لبھیا == scopeinfo->گن.سرا())
		أرجع RTLIL::Const();
	أرجع لبھیا->دوجا;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_گن(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> گن;

	ثابت حرف *اگیتر_2 = attr_اگیتر(attrs);
	ناپ_t اگیتر_len = لڑی_لمبائی(اگیتر_2);
	TwinePool &twines = scopeinfo->twines();

	لكل (خودکار ثابت &اندراج : scopeinfo->گن) {
		std::string name = twines.str(اندراج.پہلا);
		إذا (name.compare(0, اگیتر_len, اگیتر_2) != 0)
			استمر;
		گن.emplace(twines.جوڑو(RTLIL::escape_id(name.substr(اگیتر_len))), اندراج.دوجا);
	}

	أرجع گن;
}

YOSYS_NAMESPACE_سرا
