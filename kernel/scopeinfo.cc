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

#అన్తర్భావ "kernel/scopeinfo.h"

YOSYS_NAMESPACE_BEGIN

మూస <రకం_పేరు I, రకం_పేరు Filter> శూన్య Module_Hdlname_సూచీ::సూచీ_వస్తువులు(I begin, I చివర, Filter filter)
{
	కొరకు (; begin != చివర; ++begin) {
		స్వయంచాలక స్థిరరాశి &అంశం = *begin;

		ఒకవేళ (!filter(అంశం))
			కొనసాగు;
		std::సదిశ<IdString> మార్గం_2 = parse_hdlname(అంశం);
		ఒకవేళ (!మార్గం_2.ఖాళీలు())
			lookup.emplace(అంశం, వృక్షం.చొప్పించు(మార్గం_2, అంశం));
	}
}

శూన్య Module_Hdlname_సూచీ::సూచీ()
{
	సూచీ_wires();
	సూచీ_cells();
}

శూన్య Module_Hdlname_సూచీ::సూచీ_wires()
{
	స్వయంచాలక wires = module->wires();
	సూచీ_వస్తువులు(wires.begin(), wires.చివర(), [](Wire *) { తిరిగి నిజం; });
}

శూన్య Module_Hdlname_సూచీ::సూచీ_cells()
{
	స్వయంచాలక cells = module->cells();
	సూచీ_వస్తువులు(cells.begin(), cells.చివర(), [](Cell *) { తిరిగి నిజం; });
}

శూన్య Module_Hdlname_సూచీ::సూచీ_scopeinfo_cells()
{
	స్వయంచాలక cells = module->cells();
	సూచీ_వస్తువులు(cells.begin(), cells.చివర(), [](Cell *cell) { తిరిగి cell->type == ID($scopeinfo); });
}

std::సదిశ<std::string> Module_Hdlname_సూచీ::scope_sources(Cursor cursor)
{
	std::సదిశ<std::string> ఫలితం;

	కొరకు (; !cursor.is_మూలం(); cursor = cursor.మాతృ()) {
		ఒకవేళ (!cursor.has_నమోదు()) {
			ఫలితం.నెట్టు_వెనుక("");
			ఫలితం.నెట్టు_వెనుక("");
			కొనసాగు;
		}
		Cell *cell = cursor.నమోదు_2().cell();
		ఒకవేళ (cell == శూన్యసూచిక || cell->type != ID($scopeinfo)) {
			ఫలితం.నెట్టు_వెనుక("");
			ఫలితం.నెట్టు_వెనుక("");
			కొనసాగు;
		}
		ఫలితం.నెట్టు_వెనుక(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Module, ID::src).decode_string());
		ఫలితం.నెట్టు_వెనుక(scopeinfo_get_attribute(cell, ScopeinfoAttrs::Cell, ID::src).decode_string());
	}

	ఫలితం.నెట్టు_వెనుక(module->get_src_attribute());

	std::తిరగేయి(ఫలితం.begin(), ఫలితం.చివర());

	తిరిగి ఫలితం;
}

స్థిర స్థిరరాశి అక్షరం *attr_ఉపసర్గ(ScopeinfoAttrs attrs)
{
	ఎంపిక (attrs) {
	స్థితి ScopeinfoAttrs::Cell:
		తిరిగి "\\cell_";
	స్థితి ScopeinfoAttrs::Module:
		తిరిగి "\\module_";
	అప్రమేయ:
		log_abort();
	}
}

తార్కిక scopeinfo_has_attribute(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString కీలకం = twines.కనుగొను(attr_ఉపసర్గ(attrs) + twines.unescaped_str(id));
	తిరిగి కీలకం != IdString::Null && scopeinfo->has_attribute(కీలకం);
}

RTLIL::Const scopeinfo_get_attribute(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id)
{
	log_assert(scopeinfo->type == ID($scopeinfo));
	TwinePool &twines = scopeinfo->twines();
	IdString కీలకం = twines.కనుగొను(attr_ఉపసర్గ(attrs) + twines.unescaped_str(id));
	ఒకవేళ (కీలకం == IdString::Null)
		తిరిగి RTLIL::Const();
	స్వయంచాలక దొరికింది = scopeinfo->లక్షణాలు.కనుగొను(కీలకం);
	ఒకవేళ (దొరికింది == scopeinfo->లక్షణాలు.చివర())
		తిరిగి RTLIL::Const();
	తిరిగి దొరికింది->రెండవ;
}

dict<RTLIL::IdString, RTLIL::Const> scopeinfo_లక్షణాలు(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs)
{
	dict<RTLIL::IdString, RTLIL::Const> లక్షణాలు;

	స్థిరరాశి అక్షరం *ఉపసర్గ_2 = attr_ఉపసర్గ(attrs);
	కొలత_t ఉపసర్గ_len = అక్షరశ్రేణి_పొడవు(ఉపసర్గ_2);
	TwinePool &twines = scopeinfo->twines();

	కొరకు (స్వయంచాలక స్థిరరాశి &నమోదు_2 : scopeinfo->లక్షణాలు) {
		std::string name = twines.str(నమోదు_2.మొదటి);
		ఒకవేళ (name.compare(0, ఉపసర్గ_len, ఉపసర్గ_2) != 0)
			కొనసాగు;
		లక్షణాలు.emplace(twines.కూడు(RTLIL::escape_id(name.substr(ఉపసర్గ_len))), నమోదు_2.రెండవ);
	}

	తిరిగి లక్షణాలు;
}

YOSYS_NAMESPACE_చివర
