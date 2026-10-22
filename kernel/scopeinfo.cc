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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ I, ਕਿਸਮ_ਨਾਂ Filter> ਖਾਲੀ Module_Hdlname_ਸੂਚਕਾਂਕ::ਸੂਚਕਾਂਕ_ਚੀਜ਼ਾਂ(I begin, I ਸਿਰਾ, Filter filter)
{
	ਲਈ (; begin != ਸਿਰਾ; ++begin) {
		ਸਵੈਚਾਲਿਤ ਅਚਲ &ਵਸਤੂ_2 = *begin;

		ਜੇ (!filter(ਵਸਤੂ_2))
			ਜਾਰੀ;
		std::ਸਦਿਸ਼<IdString> ਰਸਤਾ_2 = parse_hdlname(ਵਸਤੂ_2);
		ਜੇ (!ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ())
			lookup.emplace(ਵਸਤੂ_2, ਰੁੱਖ.ਪਾਓ(ਰਸਤਾ_2, ਵਸਤੂ_2));
	}
}

ਖਾਲੀ Module_Hdlname_ਸੂਚਕਾਂਕ::ਸੂਚਕਾਂਕ()
{
	ਸੂਚਕਾਂਕ_wires();
	ਸੂਚਕਾਂਕ_cells();
}

ਖਾਲੀ Module_Hdlname_ਸੂਚਕਾਂਕ::ਸੂਚਕਾਂਕ_wires()
{
	ਸਵੈਚਾਲਿਤ wires = module->wires();
	ਸੂਚਕਾਂਕ_ਚੀਜ਼ਾਂ(wires.begin(), wires.ਸਿਰਾ(), [](Wire *) { ਵਾਪਸ ਸੱਚ; });
}

ਖਾਲੀ Module_Hdlname_ਸੂਚਕਾਂਕ::ਸੂਚਕਾਂਕ_cells()
{
	ਸਵੈਚਾਲਿਤ cells = module->cells();
	ਸੂਚਕਾਂਕ_ਚੀਜ਼ਾਂ(cells.begin(), cells.ਸਿਰਾ(), [](Cell *) { ਵਾਪਸ ਸੱਚ; });
}

ਖਾਲੀ Module_Hdlname_ਸੂਚਕਾਂਕ::ਸੂਚਕਾਂਕ_scopeinfo_cells()
{
	ਸਵੈਚਾਲਿਤ cells = module->cells();
	ਸੂਚਕਾਂਕ_ਚੀਜ਼ਾਂ(cells.begin(), cells.ਸਿਰਾ(), [](Cell *cell) { ਵਾਪਸ cell->type == ID($scopeinfo); });
}

std::ਸਦਿਸ਼<std::string> Module_Hdlname_ਸੂਚਕਾਂਕ::scope_sources(Cursor cursor)
{
	std::ਸਦਿਸ਼<std::string> ਨਤੀਜਾ;

	ਲਈ (; !cursor.is_ਜੜ੍ਹ(); cursor = cursor.ਮਾਪਾ()) {
		ਜੇ (!cursor.has_ਇੰਦਰਾਜ()) {
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ("");
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ("");
			ਜਾਰੀ;
		}
		Cell *cell = cursor.ਇੰਦਰਾਜ().cell();
		ਜੇ (cell == ਖਾਲੀ_ਸੂਚਕ || cell->type != ID($scopeinfo)) {
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ("");
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ("");
			ਜਾਰੀ;
		}
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(module->get_src_attribute());

	std::ਉਲਟ(ਨਤੀਜਾ.begin(), ਨਤੀਜਾ.ਸਿਰਾ());

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *attr_ਅਗੇਤਰ(ScopeinfoAttrs attrs)
{
	ਚੋਣ (attrs) {
	ਹਾਲਤ ScopeinfoAttrs::Cell:
		ਵਾਪਸ "\\cell_";
	ਹਾਲਤ ScopeinfoAttrs::Module:
		ਵਾਪਸ "\\module_";
	ਮੂਲ:
		log_abort();
	}
}

ਤਾਰਕਿਕ scopeinfo_has_attribute(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ਕੁੰਜੀ = twines.ਲੱਭੋ(attr_ਅਗੇਤਰ(attrs) + twines.unescaped_str(id));
	ਵਾਪਸ ਕੁੰਜੀ != IdString::Null && scopeinfo->has_attribute(ਕੁੰਜੀ);
}

RTLIL::Const scopeinfo_get_attribute(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ਕੁੰਜੀ = twines.ਲੱਭੋ(attr_ਅਗੇਤਰ(attrs) + twines.unescaped_str(id));
	ਜੇ (ਕੁੰਜੀ == IdString::Null)
		ਵਾਪਸ RTLIL::Const();
	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = scopeinfo->ਗੁਣ.ਲੱਭੋ(ਕੁੰਜੀ);
	ਜੇ (ਮਿਲਿਆ == scopeinfo->ਗੁਣ.ਸਿਰਾ())
		ਵਾਪਸ RTLIL::Const();
	ਵਾਪਸ ਮਿਲਿਆ->ਦੂਜਾ;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ਗੁਣ(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> ਗੁਣ;

	ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ_2 = attr_ਅਗੇਤਰ(attrs);
	ਮਾਪ_t ਅਗੇਤਰ_len = ਲੜੀ_ਲੰਬਾਈ(ਅਗੇਤਰ_2);
	TwinePool &twines = scopeinfo->twines();

	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਇੰਦਰਾਜ : scopeinfo->ਗੁਣ) {
		std::string name = twines.str(ਇੰਦਰਾਜ.ਪਹਿਲਾ);
		ਜੇ (name.compare(0, ਅਗੇਤਰ_len, ਅਗੇਤਰ_2) != 0)
			ਜਾਰੀ;
		ਗੁਣ.emplace(twines.ਜੋੜੋ(RTLIL::escape_id(name.substr(ਅਗੇਤਰ_len))), ਇੰਦਰਾਜ.ਦੂਜਾ);
	}

	ਵਾਪਸ ਗੁਣ;
}

YOSYS_NAMESPACE_ਸਿਰਾ
