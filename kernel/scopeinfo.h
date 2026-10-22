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

#నిర్వచితం_కాకపోతే SCOPEINFO_H
#నిర్వచించు SCOPEINFO_H

#అన్తర్భావ <vector>
#అన్తర్భావ <algorithm>

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

మూస<typename T>
class Id_వృక్షం
{
బహిరంగ:
	నిర్మాణం Cursor;

రక్షిత:
	Id_వృక్షం *మాతృ = nullptr;
	IdString scope_name;
	పూర్ణాంకం లోతు = 0;

	pool<IdString> పేర్లు;
	dict<IdString, T> entries;
బహిరంగ: // XXX
	dict<IdString, std::unique_ptr<Id_వృక్షం>> subtrees;

	మూస<typename P, typename T_ref>
	స్థిర Cursor do_చొప్పించు(Id_వృక్షం *వృక్షం, P begin, P చివర, T_ref &&విలువ)
	{
		log_assert(begin != చివర && "path must be non-empty");
		అయితే (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.ఖాళీలు());
			వృక్షం->పేర్లు.చొప్పించు(name);
			ఒకవేళ (begin == చివర) {
				వృక్షం->entries.emplace(name, std::forward<T_ref>(విలువ));
				తిరిగి Cursor(వృక్షం, name);
			}
			స్వయంచాలక &unique = వృక్షం->subtrees[name];
			ఒకవేళ (!unique) {
				unique.reset(new Id_వృక్షం);
				unique->scope_name = name;
				unique->మాతృ = వృక్షం;
				unique->లోతు = వృక్షం->లోతు + 1;
			}
			వృక్షం = unique.get();
		}
	}

బహిరంగ:
	Id_వృక్షం() = అప్రమేయ;
	Id_వృక్షం(స్థిరరాశి Id_వృక్షం &) = delete;
	Id_వృక్షం(Id_వృక్షం &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	నిర్మాణం Cursor
	{
		మిత్ర class Id_వృక్షం;
	రక్షిత:
	బహిరంగ:
		Id_వృక్షం *లక్ష్యం_2;
		IdString scope_name;

		Cursor() : లక్ష్యం_2(nullptr) {}
		Cursor(Id_వృక్షం *లక్ష్యం_2, IdString scope_name) : లక్ష్యం_2(లక్ష్యం_2), scope_name(scope_name) {
			ఒకవేళ (scope_name.ఖాళీలు())
				log_assert(లక్ష్యం_2->మాతృ == nullptr);
		}

		Cursor do_మొదటి_బిడ్డ() {
			Id_వృక్షం *వృక్షం = nullptr;
			ఒకవేళ (scope_name.ఖాళీలు()) {
				వృక్షం = లక్ష్యం_2;
			} లేకపోతే {
				స్వయంచాలక దొరికింది = లక్ష్యం_2->subtrees.కనుగొను(scope_name);
				ఒకవేళ (దొరికింది != లక్ష్యం_2->subtrees.చివర()) {
					వృక్షం = దొరికింది->రెండవ.get();
				} లేకపోతే {
					తిరిగి Cursor();
				}
			}
			ఒకవేళ (వృక్షం->పేర్లు.ఖాళీలు()) {
				తిరిగి Cursor();
			}
			తిరిగి Cursor(వృక్షం, *వృక్షం->పేర్లు.begin());
		}

		Cursor do_తదుపరి_sibling() {
			ఒకవేళ (scope_name.ఖాళీలు())
				తిరిగి Cursor();
			స్వయంచాలక దొరికింది = లక్ష్యం_2->పేర్లు.కనుగొను(scope_name);
			ఒకవేళ (దొరికింది == లక్ష్యం_2->పేర్లు.చివర())
				తిరిగి Cursor();
			++దొరికింది;
			ఒకవేళ (దొరికింది == లక్ష్యం_2->పేర్లు.చివర())
				తిరిగి Cursor();
			తిరిగి Cursor(లక్ష్యం_2, *దొరికింది);
		}

		Cursor do_మాతృ() {
			ఒకవేళ (scope_name.ఖాళీలు())
				తిరిగి Cursor();
			ఒకవేళ (లక్ష్యం_2->మాతృ != nullptr)
				తిరిగి Cursor(లక్ష్యం_2->మాతృ, లక్ష్యం_2->scope_name);
			తిరిగి Cursor(లక్ష్యం_2, IdString());
		}

		Cursor do_తదుపరి_పూర్వక్రమం() {
			Cursor current = *this;
			Cursor తదుపరి = current.do_మొదటి_బిడ్డ();
			ఒకవేళ (తదుపరి.valid())
				తిరిగి తదుపరి;
			అయితే (current.valid()) {
				ఒకవేళ (తదుపరి.valid())
					తిరిగి తదుపరి;
				తదుపరి = current.do_తదుపరి_sibling();
				ఒకవేళ (తదుపరి.valid())
					తిరిగి తదుపరి;
				current = current.do_మాతృ();
			}
			తిరిగి current;
		}

		Cursor do_బిడ్డ(IdString name) {
			Id_వృక్షం *వృక్షం = nullptr;
			ఒకవేళ (scope_name.ఖాళీలు()) {
				వృక్షం = లక్ష్యం_2;
			} లేకపోతే {
				స్వయంచాలక దొరికింది = లక్ష్యం_2->subtrees.కనుగొను(scope_name);
				ఒకవేళ (దొరికింది != లక్ష్యం_2->subtrees.చివర()) {
					వృక్షం = దొరికింది->రెండవ.get();
				} లేకపోతే {
					తిరిగి Cursor();
				}
			}
			స్వయంచాలక దొరికింది = వృక్షం->పేర్లు.కనుగొను(name);
			ఒకవేళ (దొరికింది == వృక్షం->పేర్లు.చివర()) {
				తిరిగి Cursor();
			}
			తిరిగి Cursor(వృక్షం, *దొరికింది);
		}

	బహిరంగ:
		bool క్రియాకారకం==(స్థిరరాశి Cursor &other) స్థిరరాశి {
			తిరిగి లక్ష్యం_2 == other.లక్ష్యం_2 && scope_name == other.scope_name;
		}
		bool క్రియాకారకం!=(స్థిరరాశి Cursor &other) స్థిరరాశి {
			తిరిగి !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి
		{
			h.eat(scope_name);
			h.eat(లక్ష్యం_2);
			తిరిగి h;
		}

		bool valid() స్థిరరాశి {
			తిరిగి లక్ష్యం_2 != nullptr;
		}

		పూర్ణాంకం లోతు() స్థిరరాశి {
			log_assert(valid());
			తిరిగి లక్ష్యం_2->లోతు + !scope_name.ఖాళీలు();
		}

		bool is_మూలం() స్థిరరాశి {
			తిరిగి లక్ష్యం_2 != nullptr && scope_name.ఖాళీలు();
		}

		bool has_నమోదు() స్థిరరాశి {
			log_assert(valid());
			తిరిగి !scope_name.ఖాళీలు() && లక్ష్యం_2->entries.లెక్క(scope_name);
		}

		T &నమోదు_2() {
			log_assert(!scope_name.ఖాళీలు());
			తిరిగి లక్ష్యం_2->entries.at(scope_name);
		}

		శూన్య కేటాయించు_మార్గం_గమ్యం(std::సదిశ<IdString> &బహిర్గతం_మార్గం) {
			log_assert(valid());
			బహిర్గతం_మార్గం.clear();
			ఒకవేళ (scope_name.ఖాళీలు())
				తిరిగి;
			బహిర్గతం_మార్గం.నెట్టు_వెనుక(scope_name);
			Id_వృక్షం *current = లక్ష్యం_2;
			అయితే (current->మాతృ) {
				బహిర్గతం_మార్గం.నెట్టు_వెనుక(current->scope_name);
				current = current->మాతృ;
			}
			std::తిరగేయి(బహిర్గతం_మార్గం.begin(), బహిర్గతం_మార్గం.చివర());
		}

		std::సదిశ<IdString> మార్గం_2() {
			std::సదిశ<IdString> ఫలితం;
			కేటాయించు_మార్గం_గమ్యం(ఫలితం);
			తిరిగి ఫలితం;
		}

		std::string మార్గం_str(స్థిరరాశి RTLIL::Design *design) {
			std::string ఫలితం;
			కొరకు (స్థిరరాశి స్వయంచాలక &అంశం : మార్గం_2()) {
				ఒకవేళ (!ఫలితం.ఖాళీలు())
					ఫలితం.నెట్టు_వెనుక(' ');
				ఫలితం += design->twines().unescaped_str(అంశం);
			}
			తిరిగి ఫలితం;
		}

		Cursor మొదటి_బిడ్డ() {
			log_assert(valid());
			తిరిగి do_మొదటి_బిడ్డ();
		}

		Cursor తదుపరి_పూర్వక్రమం() {
			log_assert(valid());
			తిరిగి do_తదుపరి_పూర్వక్రమం();
		}

		Cursor మాతృ() {
			log_assert(valid());
			తిరిగి do_మాతృ();
		}

		Cursor బిడ్డ(IdString name) {
			log_assert(valid());
			తిరిగి do_బిడ్డ(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			అయితే (current != other) {
				ఒకవేళ (!current.valid() || !other.valid())
					తిరిగి Cursor();
				పూర్ణాంకం delta = current.లోతు() - other.లోతు();
				ఒకవేళ (delta >= 0)
					current = current.do_మాతృ();
				ఒకవేళ (delta <= 0)
					other = other.do_మాతృ();
			}
			తిరిగి current;
		}
	};

	మూస<typename P>
	Cursor చొప్పించు(P begin, P చివర, స్థిరరాశి T &విలువ) {
		తిరిగి do_చొప్పించు(this, begin, చివర, విలువ);
	}

	మూస<typename P>
	Cursor చొప్పించు(P begin, P చివర, T &&విలువ) {
		తిరిగి do_చొప్పించు(this, begin, చివర, std::move(విలువ));
	}

	మూస<typename P>
	Cursor చొప్పించు(స్థిరరాశి P &మార్గం_2, స్థిరరాశి T &విలువ) {
		తిరిగి do_చొప్పించు(this, మార్గం_2.begin(), మార్గం_2.చివర(), విలువ);
	}

	మూస<typename P>
	Cursor చొప్పించు(స్థిరరాశి P &మార్గం_2, T &&విలువ) {
		తిరిగి do_చొప్పించు(this, మార్గం_2.begin(), మార్గం_2.చివర(), std::move(విలువ));
	}

	Cursor cursor() {
		తిరిగి మాతృ ? Cursor(this->మాతృ, this->scope_name) : Cursor(this, IdString());
	}

	మూస<typename P>
	Cursor cursor(P begin, P చివర) {
		Cursor current = cursor();
		కొరకు (; begin != చివర; ++begin) {
			current = current.do_బిడ్డ(*begin);
			ఒకవేళ (!current.valid())
				ఆపు;
		}
		తిరిగి current;
	}

	మూస<typename P>
	Cursor cursor(స్థిరరాశి P &మార్గం_2) {
		తిరిగి cursor(మార్గం_2.begin(), మార్గం_2.చివర());
	}
};


నిర్మాణం Module_అంశం {
	గణన class Type {
		Wire,
		Cell,
	};
	Type type;
	శూన్య *ptr;

	Module_అంశం(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_అంశం(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() స్థిరరాశి { తిరిగి type == Type::Wire; }
	bool is_cell() స్థిరరాశి { తిరిగి type == Type::Cell; }

	Wire *wire() స్థిరరాశి { తిరిగి type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() స్థిరరాశి { తిరిగి type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool క్రియాకారకం==(స్థిరరాశి Module_అంశం &other) స్థిరరాశి { తిరిగి ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(ptr); తిరిగి h; }
};

స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(typename Id_వృక్షం<Module_అంశం>::Cursor cursor ) { సంవర్గమానం("%p ", cursor.లక్ష్యం_2); log_dump_val_కార్మికుడు(cursor.scope_name); }

మూస<typename T>
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(స్థిరరాశి typename std::unique_ptr<T> &cursor ) { సంవర్గమానం("unique %p", cursor.get()); }

మూస<typename O>
std::సదిశ<IdString> parse_hdlname(స్థిరరాశి O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::సదిశ<IdString> మార్గం_2;
	కొరకు (స్వయంచాలక స్థిరరాశి &అంశం : object->get_hdlname_attribute())
		మార్గం_2.నెట్టు_వెనుక(twines.కూడు("\\" + అంశం));
	bool synthetic = !name.is_ప్రజా() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ఒకవేళ (మార్గం_2.ఖాళీలు() && name.is_ప్రజా())
		మార్గం_2.నెట్టు_వెనుక(name);
	ఒకవేళ (!మార్గం_2.ఖాళీలు() && synthetic) {
		మార్గం_2.తీయి_వెనుక();
		మార్గం_2.నెట్టు_వెనుక(name);
	}
	తిరిగి మార్గం_2;
}

మూస<typename O>
std::pair<std::సదిశ<IdString>, IdString> parse_scopename(స్థిరరాశి O* object)
{
	TwinePool &twines = object->design()->twines();
	std::సదిశ<IdString> మార్గం_2;
	IdString trailing = object->name;
	ఒకవేళ (trailing.is_ప్రజా() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		కొరకు (స్వయంచాలక స్థిరరాశి &అంశం : object->get_hdlname_attribute())
			మార్గం_2.నెట్టు_వెనుక(twines.కూడు("\\" + అంశం));
		ఒకవేళ (!మార్గం_2.ఖాళీలు()) {
			trailing = మార్గం_2.వెనుక();
			మార్గం_2.తీయి_వెనుక();
		}
	} లేకపోతే ఒకవేళ (object->has_attribute(ID::hdlname)) {
		కొరకు (స్వయంచాలక స్థిరరాశి &అంశం : object->get_hdlname_attribute())
			మార్గం_2.నెట్టు_వెనుక(twines.కూడు("\\" + అంశం));
		ఒకవేళ (!మార్గం_2.ఖాళీలు()) {
			మార్గం_2.తీయి_వెనుక();
		}
	} లేకపోతే {
		కొరకు (స్వయంచాలక స్థిరరాశి &అంశం : విభజించు_టోకెన్లు(object->get_string_attribute(ID(scopename)), " "))
			మార్గం_2.నెట్టు_వెనుక(twines.కూడు("\\" + అంశం));
	}
	తిరిగి {మార్గం_2, trailing};
}

నిర్మాణం Module_Hdlname_సూచీ {
	రకనిర్వచనం Id_వృక్షం<Module_అంశం>::Cursor Cursor;

	RTLIL::Module *module;
	Id_వృక్షం<Module_అంశం> వృక్షం;
	dict<Module_అంశం, Cursor> lookup;

	Module_Hdlname_సూచీ(RTLIL::Module *module) : module(module) {}

అంతరంగ:
	మూస<typename I, typename Filter>
	శూన్య సూచీ_వస్తువులు(I begin, I చివర, Filter filter);

బహిరంగ:
	// Index all wires and cells of the module
	శూన్య సూచీ();

	// Index all wires of the module
	శూన్య సూచీ_wires();

	// Index all cells of the module
	శూన్య సూచీ_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	శూన్య సూచీ_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	మూస<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		స్వయంచాలక pair = parse_scopename(object);
		తిరిగి {వృక్షం.cursor(pair.మొదటి), pair.రెండవ};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::సదిశ<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	మూస<typename O>
	std::సదిశ<std::string> sources(O *object) {
		స్వయంచాలక pair = parse_scopename(object);
		std::సదిశ<std::string> ఫలితం = scope_sources(వృక్షం.cursor(pair.మొదటి));
		ఫలితం.నెట్టు_వెనుక(object->get_src_attribute());
		తిరిగి ఫలితం;
	}
};

గణన class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_లక్షణాలు(స్థిరరాశి RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
