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

#شامل "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

سانچہ <قسم_نام I, قسم_نام Filter> خالی Module_Hdlname_اشاریہ::اشاریہ_اشیاء(I begin, I انتہا, Filter filter)
{
	برائے (; begin != انتہا; ++begin) {
		خودکار ثابت &شے_2 = *begin;

		اگر (!filter(شے_2))
			جاری;
		std::سمتیہ<IdString> راستہ_2 = parse_hdlname(شے_2);
		اگر (!راستہ_2.خالی_خانے())
			lookup.emplace(شے_2, درخت.داخل_کرو(راستہ_2, شے_2));
	}
}

خالی Module_Hdlname_اشاریہ::اشاریہ()
{
	اشاریہ_wires();
	اشاریہ_cells();
}

خالی Module_Hdlname_اشاریہ::اشاریہ_wires()
{
	خودکار wires = module->wires();
	اشاریہ_اشیاء(wires.begin(), wires.انتہا(), [](Wire *) { واپس سچ; });
}

خالی Module_Hdlname_اشاریہ::اشاریہ_cells()
{
	خودکار cells = module->cells();
	اشاریہ_اشیاء(cells.begin(), cells.انتہا(), [](Cell *) { واپس سچ; });
}

خالی Module_Hdlname_اشاریہ::اشاریہ_scopeinfo_cells()
{
	خودکار cells = module->cells();
	اشاریہ_اشیاء(cells.begin(), cells.انتہا(), [](Cell *cell) { واپس cell->type == ID($scopeinfo); });
}

std::سمتیہ<std::string> Module_Hdlname_اشاریہ::scope_sources(Cursor cursor)
{
	std::سمتیہ<std::string> نتیجہ;

	برائے (; !cursor.is_جڑ(); cursor = cursor.والد()) {
		اگر (!cursor.has_اندراج()) {
			نتیجہ.ڈالو_عقب("");
			نتیجہ.ڈالو_عقب("");
			جاری;
		}
		Cell *cell = cursor.اندراج_2().cell();
		اگر (cell == خالی_اشاریہ || cell->type != ID($scopeinfo)) {
			نتیجہ.ڈالو_عقب("");
			نتیجہ.ڈالو_عقب("");
			جاری;
		}
		نتیجہ.ڈالو_عقب(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		نتیجہ.ڈالو_عقب(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	نتیجہ.ڈالو_عقب(module->get_src_attribute());

	std::الٹ(نتیجہ.begin(), نتیجہ.انتہا());

	واپس نتیجہ;
}

ساکن ثابت حرف *attr_سابقہ(ScopeinfoAttrs attrs)
{
	انتخاب (attrs) {
	حالت ScopeinfoAttrs::Cell:
		واپس "\\cell_";
	حالت ScopeinfoAttrs::Module:
		واپس "\\module_";
	طےشدہ:
		log_abort();
	}
}

منطقی scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.ڈھونڈو(attr_سابقہ(attrs) + twines.unescaped_str(id));
	واپس کلید != IdString::Null && scopeinfo->has_attribute(کلید);
}

RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString کلید = twines.ڈھونڈو(attr_سابقہ(attrs) + twines.unescaped_str(id));
	اگر (کلید == IdString::Null)
		واپس RTLIL::Const();
	خودکار ملا = scopeinfo->اوصاف.ڈھونڈو(کلید);
	اگر (ملا == scopeinfo->اوصاف.انتہا())
		واپس RTLIL::Const();
	واپس ملا->دوسرا;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_اوصاف(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> اوصاف;

	ثابت حرف *سابقہ_2 = attr_سابقہ(attrs);
	سائز_t سابقہ_len = لڑی_لمبائی(سابقہ_2);
	TwinePool &twines = scopeinfo->twines();

	برائے (خودکار ثابت &اندراج_2 : scopeinfo->اوصاف) {
		std::string name = twines.str(اندراج_2.پہلا);
		اگر (name.compare(0, سابقہ_len, سابقہ_2) != 0)
			جاری;
		اوصاف.emplace(twines.جوڑو(RTLIL::escape_id(name.substr(سابقہ_len))), اندراج_2.دوسرا);
	}

	واپس اوصاف;
}

YOSYS_NAMESPACE_انتہا
