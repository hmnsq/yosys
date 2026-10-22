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

#که_نه_تعریف_شوی SCOPEINFO_H
#تعریف_کړه SCOPEINFO_H

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

کينډۍ<typename T>
class Id_ونه
{
عام:
	بنية Cursor;

محمي:
	Id_ونه *پلار = nullptr;
	IdString scope_name;
	صحيح ژوروالی = 0;

	pool<IdString> نومونه;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<Id_ونه>> subtrees;

	کينډۍ<typename P, typename T_ref>
	ساكن Cursor do_ورننباسه(Id_ونه *ونه, P begin, P پای_ټکی, T_ref &&ارزښت)
	{
		log_assert(begin != پای_ټکی && "path must be non-empty");
		طالما (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.تشه());
			ونه->نومونه.ورننباسه(name);
			إذا (begin == پای_ټکی) {
				ونه->entries.emplace(name, std::forward<T_ref>(ارزښت));
				أرجع Cursor(ونه, name);
			}
			خپلکاری &unique = ونه->subtrees[name];
			إذا (!unique) {
				unique.reset(new Id_ونه);
				unique->scope_name = name;
				unique->پلار = ونه;
				unique->ژوروالی = ونه->ژوروالی + 1;
			}
			ونه = unique.get();
		}
	}

عام:
	Id_ونه() = افتراضي;
	Id_ونه(ثابت Id_ونه &) = delete;
	Id_ونه(Id_ونه &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	بنية Cursor
	{
		ملګری class Id_ونه;
	محمي:
	عام:
		Id_ونه *موخه_2;
		IdString scope_name;

		Cursor() : موخه_2(nullptr) {}
		Cursor(Id_ونه *موخه_2, IdString scope_name) : موخه_2(موخه_2), scope_name(scope_name) {
			إذا (scope_name.تشه())
				log_assert(موخه_2->پلار == nullptr);
		}

		Cursor do_لومړی_ماشوم() {
			Id_ونه *ونه = nullptr;
			إذا (scope_name.تشه()) {
				ونه = موخه_2;
			} وإلا {
				خپلکاری وموندل = موخه_2->subtrees.پیدا_کړه(scope_name);
				إذا (وموندل != موخه_2->subtrees.پای_ټکی()) {
					ونه = وموندل->دوهم.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			إذا (ونه->نومونه.تشه()) {
				أرجع Cursor();
			}
			أرجع Cursor(ونه, *ونه->نومونه.begin());
		}

		Cursor do_بل_sibling() {
			إذا (scope_name.تشه())
				أرجع Cursor();
			خپلکاری وموندل = موخه_2->نومونه.پیدا_کړه(scope_name);
			إذا (وموندل == موخه_2->نومونه.پای_ټکی())
				أرجع Cursor();
			++وموندل;
			إذا (وموندل == موخه_2->نومونه.پای_ټکی())
				أرجع Cursor();
			أرجع Cursor(موخه_2, *وموندل);
		}

		Cursor do_پلار() {
			إذا (scope_name.تشه())
				أرجع Cursor();
			إذا (موخه_2->پلار != nullptr)
				أرجع Cursor(موخه_2->پلار, موخه_2->scope_name);
			أرجع Cursor(موخه_2, IdString());
		}

		Cursor do_بل_مخکنی_ترتیب() {
			Cursor current = *this;
			Cursor بل = current.do_لومړی_ماشوم();
			إذا (بل.valid())
				أرجع بل;
			طالما (current.valid()) {
				إذا (بل.valid())
					أرجع بل;
				بل = current.do_بل_sibling();
				إذا (بل.valid())
					أرجع بل;
				current = current.do_پلار();
			}
			أرجع current;
		}

		Cursor do_ماشوم(IdString name) {
			Id_ونه *ونه = nullptr;
			إذا (scope_name.تشه()) {
				ونه = موخه_2;
			} وإلا {
				خپلکاری وموندل = موخه_2->subtrees.پیدا_کړه(scope_name);
				إذا (وموندل != موخه_2->subtrees.پای_ټکی()) {
					ونه = وموندل->دوهم.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			خپلکاری وموندل = ونه->نومونه.پیدا_کړه(name);
			إذا (وموندل == ونه->نومونه.پای_ټکی()) {
				أرجع Cursor();
			}
			أرجع Cursor(ونه, *وموندل);
		}

	عام:
		bool چلوونکی==(ثابت Cursor &other) ثابت {
			أرجع موخه_2 == other.موخه_2 && scope_name == other.scope_name;
		}
		bool چلوونکی!=(ثابت Cursor &other) ثابت {
			أرجع !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(موخه_2);
			أرجع h;
		}

		bool valid() ثابت {
			أرجع موخه_2 != nullptr;
		}

		صحيح ژوروالی() ثابت {
			log_assert(valid());
			أرجع موخه_2->ژوروالی + !scope_name.تشه();
		}

		bool is_ریښه() ثابت {
			أرجع موخه_2 != nullptr && scope_name.تشه();
		}

		bool has_ثبت() ثابت {
			log_assert(valid());
			أرجع !scope_name.تشه() && موخه_2->entries.شمېر(scope_name);
		}

		T &ثبت() {
			log_assert(!scope_name.تشه());
			أرجع موخه_2->entries.at(scope_name);
		}

		فراغ ټاکل_لار_منزل(std::ویکټور<IdString> &بهر_لار) {
			log_assert(valid());
			بهر_لار.clear();
			إذا (scope_name.تشه())
				أرجع;
			بهر_لار.ټېل_شا(scope_name);
			Id_ونه *current = موخه_2;
			طالما (current->پلار) {
				بهر_لار.ټېل_شا(current->scope_name);
				current = current->پلار;
			}
			std::سرچپه(بهر_لار.begin(), بهر_لار.پای_ټکی());
		}

		std::ویکټور<IdString> لار_2() {
			std::ویکټور<IdString> پایله;
			ټاکل_لار_منزل(پایله);
			أرجع پایله;
		}

		std::string لار_str(ثابت RTLIL::Design *design) {
			std::string پایله;
			لكل (ثابت خپلکاری &توکی : لار_2()) {
				إذا (!پایله.تشه())
					پایله.ټېل_شا(' ');
				پایله += design->twines().unescaped_str(توکی);
			}
			أرجع پایله;
		}

		Cursor لومړی_ماشوم() {
			log_assert(valid());
			أرجع do_لومړی_ماشوم();
		}

		Cursor بل_مخکنی_ترتیب() {
			log_assert(valid());
			أرجع do_بل_مخکنی_ترتیب();
		}

		Cursor پلار() {
			log_assert(valid());
			أرجع do_پلار();
		}

		Cursor ماشوم(IdString name) {
			log_assert(valid());
			أرجع do_ماشوم(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			طالما (current != other) {
				إذا (!current.valid() || !other.valid())
					أرجع Cursor();
				صحيح delta = current.ژوروالی() - other.ژوروالی();
				إذا (delta >= 0)
					current = current.do_پلار();
				إذا (delta <= 0)
					other = other.do_پلار();
			}
			أرجع current;
		}
	};

	کينډۍ<typename P>
	Cursor ورننباسه(P begin, P پای_ټکی, ثابت T &ارزښت) {
		أرجع do_ورننباسه(this, begin, پای_ټکی, ارزښت);
	}

	کينډۍ<typename P>
	Cursor ورننباسه(P begin, P پای_ټکی, T &&ارزښت) {
		أرجع do_ورننباسه(this, begin, پای_ټکی, std::move(ارزښت));
	}

	کينډۍ<typename P>
	Cursor ورننباسه(ثابت P &لار_2, ثابت T &ارزښت) {
		أرجع do_ورننباسه(this, لار_2.begin(), لار_2.پای_ټکی(), ارزښت);
	}

	کينډۍ<typename P>
	Cursor ورننباسه(ثابت P &لار_2, T &&ارزښت) {
		أرجع do_ورننباسه(this, لار_2.begin(), لار_2.پای_ټکی(), std::move(ارزښت));
	}

	Cursor cursor() {
		أرجع پلار ? Cursor(this->پلار, this->scope_name) : Cursor(this, IdString());
	}

	کينډۍ<typename P>
	Cursor cursor(P begin, P پای_ټکی) {
		Cursor current = cursor();
		لكل (; begin != پای_ټکی; ++begin) {
			current = current.do_ماشوم(*begin);
			إذا (!current.valid())
				اكسر;
		}
		أرجع current;
	}

	کينډۍ<typename P>
	Cursor cursor(ثابت P &لار_2) {
		أرجع cursor(لار_2.begin(), لار_2.پای_ټکی());
	}
};


بنية Module_توکی {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	Module_توکی(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_توکی(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { أرجع type == Type::Wire; }
	bool is_cell() ثابت { أرجع type == Type::Cell; }

	Wire *wire() ثابت { أرجع type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { أرجع type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool چلوونکی==(ثابت Module_توکی &other) ثابت { أرجع ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); أرجع h; }
};

ساكن مضمن فراغ log_dump_val_کارګر(typename Id_ونه<Module_توکی>::Cursor cursor ) { لوګاریتم("%p ", cursor.موخه_2); log_dump_val_کارګر(cursor.scope_name); }

کينډۍ<typename T>
ساكن مضمن فراغ log_dump_val_کارګر(ثابت typename std::unique_ptr<T> &cursor ) { لوګاریتم("unique %p", cursor.get()); }

کينډۍ<typename O>
std::ویکټور<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::ویکټور<IdString> لار_2;
	لكل (خپلکاری ثابت &توکی : object->get_hdlname_attribute())
		لار_2.ټېل_شا(twines.جمع_کړه("\\" + توکی));
	bool synthetic = !name.is_عامه() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	إذا (لار_2.تشه() && name.is_عامه())
		لار_2.ټېل_شا(name);
	إذا (!لار_2.تشه() && synthetic) {
		لار_2.راوباسه_شا();
		لار_2.ټېل_شا(name);
	}
	أرجع لار_2;
}

کينډۍ<typename O>
std::pair<std::ویکټور<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::ویکټور<IdString> لار_2;
	IdString trailing = object->name;
	إذا (trailing.is_عامه() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		لكل (خپلکاری ثابت &توکی : object->get_hdlname_attribute())
			لار_2.ټېل_شا(twines.جمع_کړه("\\" + توکی));
		إذا (!لار_2.تشه()) {
			trailing = لار_2.شا();
			لار_2.راوباسه_شا();
		}
	} وإلا إذا (object->has_attribute(ID::hdlname)) {
		لكل (خپلکاری ثابت &توکی : object->get_hdlname_attribute())
			لار_2.ټېل_شا(twines.جمع_کړه("\\" + توکی));
		إذا (!لار_2.تشه()) {
			لار_2.راوباسه_شا();
		}
	} وإلا {
		لكل (خپلکاری ثابت &توکی : وېشل_نښې(object->get_string_attribute(ID(scopename)), " "))
			لار_2.ټېل_شا(twines.جمع_کړه("\\" + توکی));
	}
	أرجع {لار_2, trailing};
}

بنية Module_Hdlname_شاخص {
	تعريفنوع Id_ونه<Module_توکی>::Cursor Cursor;

	RTLIL::Module *module;
	Id_ونه<Module_توکی> ونه;
	dict<Module_توکی, Cursor> lookup;

	Module_Hdlname_شاخص(RTLIL::Module *module) : module(module) {}

خاص:
	کينډۍ<typename I, typename Filter>
	فراغ شاخص_توکي(I begin, I پای_ټکی, Filter filter);

عام:
	// Index all wires and cells of the module
	فراغ شاخص();

	// Index all wires of the module
	فراغ شاخص_wires();

	// Index all cells of the module
	فراغ شاخص_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	فراغ شاخص_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	کينډۍ<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		خپلکاری pair = parse_scopename(object);
		أرجع {ونه.cursor(pair.لومړی), pair.دوهم};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::ویکټور<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	کينډۍ<typename O>
	std::ویکټور<std::string> sources(O *object) {
		خپلکاری pair = parse_scopename(object);
		std::ویکټور<std::string> پایله = scope_sources(ونه.cursor(pair.لومړی));
		پایله.ټېل_شا(object->get_src_attribute());
		أرجع پایله;
	}
};

تعداد class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ځانګړتیاوې(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
