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

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು I, ಬಗೆ_ಹೆಸರು Filter> ಶೂನ್ಯ Module_Hdlname_ಸೂಚ್ಯಂಕ::ಸೂಚ್ಯಂಕ_ವಸ್ತುಗಳು(I begin, I ಕೊನೆ, Filter filter)
{
	ಗಾಗಿ (; begin != ಕೊನೆ; ++begin) {
		ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಶ = *begin;

		ಆದರೆ (!filter(ಅಂಶ))
			ಮುಂದುವರಿ;
		std::ಸದಿಶ<IdString> ಮಾರ್ಗ_2 = parse_hdlname(ಅಂಶ);
		ಆದರೆ (!ಮಾರ್ಗ_2.ಖಾಲಿಗಳು())
			lookup.emplace(ಅಂಶ, ವೃಕ್ಷ.ಸೇರಿಸು(ಮಾರ್ಗ_2, ಅಂಶ));
	}
}

ಶೂನ್ಯ Module_Hdlname_ಸೂಚ್ಯಂಕ::ಸೂಚ್ಯಂಕ()
{
	ಸೂಚ್ಯಂಕ_wires();
	ಸೂಚ್ಯಂಕ_cells();
}

ಶೂನ್ಯ Module_Hdlname_ಸೂಚ್ಯಂಕ::ಸೂಚ್ಯಂಕ_wires()
{
	ಸ್ವಯಂಚಾಲಿತ wires = module->wires();
	ಸೂಚ್ಯಂಕ_ವಸ್ತುಗಳು(wires.begin(), wires.ಕೊನೆ(), [](Wire *) { ಹಿಂತಿರುಗಿ ನಿಜ; });
}

ಶೂನ್ಯ Module_Hdlname_ಸೂಚ್ಯಂಕ::ಸೂಚ್ಯಂಕ_cells()
{
	ಸ್ವಯಂಚಾಲಿತ cells = module->cells();
	ಸೂಚ್ಯಂಕ_ವಸ್ತುಗಳು(cells.begin(), cells.ಕೊನೆ(), [](Cell *) { ಹಿಂತಿರುಗಿ ನಿಜ; });
}

ಶೂನ್ಯ Module_Hdlname_ಸೂಚ್ಯಂಕ::ಸೂಚ್ಯಂಕ_scopeinfo_cells()
{
	ಸ್ವಯಂಚಾಲಿತ cells = module->cells();
	ಸೂಚ್ಯಂಕ_ವಸ್ತುಗಳು(cells.begin(), cells.ಕೊನೆ(), [](Cell *cell) { ಹಿಂತಿರುಗಿ cell->type == ID($scopeinfo); });
}

std::ಸದಿಶ<std::string> Module_Hdlname_ಸೂಚ್ಯಂಕ::scope_sources(Cursor cursor)
{
	std::ಸದಿಶ<std::string> ಫಲಿತಾಂಶ;

	ಗಾಗಿ (; !cursor.is_ಬೇರು(); cursor = cursor.ಪೋಷಕ()) {
		ಆದರೆ (!cursor.has_ನಮೂದು()) {
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ("");
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ("");
			ಮುಂದುವರಿ;
		}
		Cell *cell = cursor.ನಮೂದು().cell();
		ಆದರೆ (cell == ಶೂನ್ಯಸೂಚಕ || cell->type != ID($scopeinfo)) {
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ("");
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ("");
			ಮುಂದುವರಿ;
		}
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(module->get_src_attribute());

	std::ಹಿಮ್ಮುಖ(ಫಲಿತಾಂಶ.begin(), ಫಲಿತಾಂಶ.ಕೊನೆ());

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attr_ಪೂರ್ವಪ್ರತ್ಯಯ(ScopeinfoAttrs attrs)
{
	ಆಯ್ಕೆ (attrs) {
	ಸ್ಥಿತಿ ScopeinfoAttrs::Cell:
		ಹಿಂತಿರುಗಿ "\\cell_";
	ಸ್ಥಿತಿ ScopeinfoAttrs::Module:
		ಹಿಂತಿರುಗಿ "\\module_";
	ಪೂರ್ವನಿಯೋಜಿತ:
		log_abort();
	}
}

ತಾರ್ಕಿಕ scopeinfo_has_attribute(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ಕೀಲಿ = twines.ಕಂಡುಹಿಡಿ(attr_ಪೂರ್ವಪ್ರತ್ಯಯ(attrs) + twines.unescaped_str(id));
	ಹಿಂತಿರುಗಿ ಕೀಲಿ != IdString::Null && scopeinfo->has_attribute(ಕೀಲಿ);
}

RTLIL::Const scopeinfo_get_attribute(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString ಕೀಲಿ = twines.ಕಂಡುಹಿಡಿ(attr_ಪೂರ್ವಪ್ರತ್ಯಯ(attrs) + twines.unescaped_str(id));
	ಆದರೆ (ಕೀಲಿ == IdString::Null)
		ಹಿಂತಿರುಗಿ RTLIL::Const();
	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = scopeinfo->ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(ಕೀಲಿ);
	ಆದರೆ (ಸಿಕ್ಕಿತು == scopeinfo->ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ RTLIL::Const();
	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು->ಎರಡನೇ;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ಗುಣಲಕ್ಷಣಗಳು(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> ಗುಣಲಕ್ಷಣಗಳು;

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ_2 = attr_ಪೂರ್ವಪ್ರತ್ಯಯ(attrs);
	ಗಾತ್ರ_t ಪೂರ್ವಪ್ರತ್ಯಯ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪೂರ್ವಪ್ರತ್ಯಯ_2);
	TwinePool &twines = scopeinfo->twines();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ನಮೂದು : scopeinfo->ಗುಣಲಕ್ಷಣಗಳು) {
		std::string name = twines.str(ನಮೂದು.ಮೊದಲ);
		ಆದರೆ (name.compare(0, ಪೂರ್ವಪ್ರತ್ಯಯ_len, ಪೂರ್ವಪ್ರತ್ಯಯ_2) != 0)
			ಮುಂದುವರಿ;
		ಗುಣಲಕ್ಷಣಗಳು.emplace(twines.ಕೂಡಿಸು(RTLIL::escape_id(name.substr(ಪೂರ್ವಪ್ರತ್ಯಯ_len))), ನಮೂದು.ಎರಡನೇ);
	}

	ಹಿಂತಿರುಗಿ ಗುಣಲಕ್ಷಣಗಳು;
}

YOSYS_NAMESPACE_ಕೊನೆ
