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

#جے_بنا_تعریف SCOPEINFO_H
#تعریف_کرو SCOPEINFO_H

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

سانچا<typename T>
class Id_درخت
{
عام:
	بنية Cursor;

محمي:
	Id_درخت *ماپا = nullptr;
	IdString scope_name;
	صحيح ڈونگھائی = 0;

	pool<IdString> ناں;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<Id_درخت>> subtrees;

	سانچا<typename P, typename T_ref>
	ساكن Cursor do_داخل_کرو(Id_درخت *درخت, P begin, P سرا, T_ref &&قدر)
	{
		log_assert(begin != سرا && "path must be non-empty");
		طالما (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.خالی_تھاواں());
			درخت->ناں.داخل_کرو(name);
			إذا (begin == سرا) {
				درخت->entries.emplace(name, std::forward<T_ref>(قدر));
				أرجع Cursor(درخت, name);
			}
			خودکار &unique = درخت->subtrees[name];
			إذا (!unique) {
				unique.reset(new Id_درخت);
				unique->scope_name = name;
				unique->ماپا = درخت;
				unique->ڈونگھائی = درخت->ڈونگھائی + 1;
			}
			درخت = unique.get();
		}
	}

عام:
	Id_درخت() = افتراضي;
	Id_درخت(ثابت Id_درخت &) = delete;
	Id_درخت(Id_درخت &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	بنية Cursor
	{
		یار class Id_درخت;
	محمي:
	عام:
		Id_درخت *نشانہ_2;
		IdString scope_name;

		Cursor() : نشانہ_2(nullptr) {}
		Cursor(Id_درخت *نشانہ_2, IdString scope_name) : نشانہ_2(نشانہ_2), scope_name(scope_name) {
			إذا (scope_name.خالی_تھاواں())
				log_assert(نشانہ_2->ماپا == nullptr);
		}

		Cursor do_پہلا_بچہ() {
			Id_درخت *درخت = nullptr;
			إذا (scope_name.خالی_تھاواں()) {
				درخت = نشانہ_2;
			} وإلا {
				خودکار لبھیا = نشانہ_2->subtrees.لبھو(scope_name);
				إذا (لبھیا != نشانہ_2->subtrees.سرا()) {
					درخت = لبھیا->دوجا.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			إذا (درخت->ناں.خالی_تھاواں()) {
				أرجع Cursor();
			}
			أرجع Cursor(درخت, *درخت->ناں.begin());
		}

		Cursor do_اگلا_sibling() {
			إذا (scope_name.خالی_تھاواں())
				أرجع Cursor();
			خودکار لبھیا = نشانہ_2->ناں.لبھو(scope_name);
			إذا (لبھیا == نشانہ_2->ناں.سرا())
				أرجع Cursor();
			++لبھیا;
			إذا (لبھیا == نشانہ_2->ناں.سرا())
				أرجع Cursor();
			أرجع Cursor(نشانہ_2, *لبھیا);
		}

		Cursor do_ماپا() {
			إذا (scope_name.خالی_تھاواں())
				أرجع Cursor();
			إذا (نشانہ_2->ماپا != nullptr)
				أرجع Cursor(نشانہ_2->ماپا, نشانہ_2->scope_name);
			أرجع Cursor(نشانہ_2, IdString());
		}

		Cursor do_اگلا_پہلا_ترتیب() {
			Cursor current = *this;
			Cursor اگلا = current.do_پہلا_بچہ();
			إذا (اگلا.valid())
				أرجع اگلا;
			طالما (current.valid()) {
				إذا (اگلا.valid())
					أرجع اگلا;
				اگلا = current.do_اگلا_sibling();
				إذا (اگلا.valid())
					أرجع اگلا;
				current = current.do_ماپا();
			}
			أرجع current;
		}

		Cursor do_بچہ(IdString name) {
			Id_درخت *درخت = nullptr;
			إذا (scope_name.خالی_تھاواں()) {
				درخت = نشانہ_2;
			} وإلا {
				خودکار لبھیا = نشانہ_2->subtrees.لبھو(scope_name);
				إذا (لبھیا != نشانہ_2->subtrees.سرا()) {
					درخت = لبھیا->دوجا.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			خودکار لبھیا = درخت->ناں.لبھو(name);
			إذا (لبھیا == درخت->ناں.سرا()) {
				أرجع Cursor();
			}
			أرجع Cursor(درخت, *لبھیا);
		}

	عام:
		bool عامل==(ثابت Cursor &other) ثابت {
			أرجع نشانہ_2 == other.نشانہ_2 && scope_name == other.scope_name;
		}
		bool عامل!=(ثابت Cursor &other) ثابت {
			أرجع !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(نشانہ_2);
			أرجع h;
		}

		bool valid() ثابت {
			أرجع نشانہ_2 != nullptr;
		}

		صحيح ڈونگھائی() ثابت {
			log_assert(valid());
			أرجع نشانہ_2->ڈونگھائی + !scope_name.خالی_تھاواں();
		}

		bool is_جڑ() ثابت {
			أرجع نشانہ_2 != nullptr && scope_name.خالی_تھاواں();
		}

		bool has_اندراج() ثابت {
			log_assert(valid());
			أرجع !scope_name.خالی_تھاواں() && نشانہ_2->entries.گنتی(scope_name);
		}

		T &اندراج() {
			log_assert(!scope_name.خالی_تھاواں());
			أرجع نشانہ_2->entries.at(scope_name);
		}

		فراغ سونپو_رستہ_منزل(std::سمتیہ<IdString> &باہرلا_رستہ) {
			log_assert(valid());
			باہرلا_رستہ.clear();
			إذا (scope_name.خالی_تھاواں())
				أرجع;
			باہرلا_رستہ.ڈالو_پٹھ(scope_name);
			Id_درخت *current = نشانہ_2;
			طالما (current->ماپا) {
				باہرلا_رستہ.ڈالو_پٹھ(current->scope_name);
				current = current->ماپا;
			}
			std::الٹ(باہرلا_رستہ.begin(), باہرلا_رستہ.سرا());
		}

		std::سمتیہ<IdString> رستہ_2() {
			std::سمتیہ<IdString> نتیجہ;
			سونپو_رستہ_منزل(نتیجہ);
			أرجع نتیجہ;
		}

		std::string رستہ_str(ثابت RTLIL::Design *design) {
			std::string نتیجہ;
			لكل (ثابت خودکار &شے : رستہ_2()) {
				إذا (!نتیجہ.خالی_تھاواں())
					نتیجہ.ڈالو_پٹھ(' ');
				نتیجہ += design->twines().unescaped_str(شے);
			}
			أرجع نتیجہ;
		}

		Cursor پہلا_بچہ() {
			log_assert(valid());
			أرجع do_پہلا_بچہ();
		}

		Cursor اگلا_پہلا_ترتیب() {
			log_assert(valid());
			أرجع do_اگلا_پہلا_ترتیب();
		}

		Cursor ماپا() {
			log_assert(valid());
			أرجع do_ماپا();
		}

		Cursor بچہ(IdString name) {
			log_assert(valid());
			أرجع do_بچہ(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			طالما (current != other) {
				إذا (!current.valid() || !other.valid())
					أرجع Cursor();
				صحيح delta = current.ڈونگھائی() - other.ڈونگھائی();
				إذا (delta >= 0)
					current = current.do_ماپا();
				إذا (delta <= 0)
					other = other.do_ماپا();
			}
			أرجع current;
		}
	};

	سانچا<typename P>
	Cursor داخل_کرو(P begin, P سرا, ثابت T &قدر) {
		أرجع do_داخل_کرو(this, begin, سرا, قدر);
	}

	سانچا<typename P>
	Cursor داخل_کرو(P begin, P سرا, T &&قدر) {
		أرجع do_داخل_کرو(this, begin, سرا, std::move(قدر));
	}

	سانچا<typename P>
	Cursor داخل_کرو(ثابت P &رستہ_2, ثابت T &قدر) {
		أرجع do_داخل_کرو(this, رستہ_2.begin(), رستہ_2.سرا(), قدر);
	}

	سانچا<typename P>
	Cursor داخل_کرو(ثابت P &رستہ_2, T &&قدر) {
		أرجع do_داخل_کرو(this, رستہ_2.begin(), رستہ_2.سرا(), std::move(قدر));
	}

	Cursor cursor() {
		أرجع ماپا ? Cursor(this->ماپا, this->scope_name) : Cursor(this, IdString());
	}

	سانچا<typename P>
	Cursor cursor(P begin, P سرا) {
		Cursor current = cursor();
		لكل (; begin != سرا; ++begin) {
			current = current.do_بچہ(*begin);
			إذا (!current.valid())
				اكسر;
		}
		أرجع current;
	}

	سانچا<typename P>
	Cursor cursor(ثابت P &رستہ_2) {
		أرجع cursor(رستہ_2.begin(), رستہ_2.سرا());
	}
};


بنية Module_شے {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	Module_شے(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_شے(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { أرجع type == Type::Wire; }
	bool is_cell() ثابت { أرجع type == Type::Cell; }

	Wire *wire() ثابت { أرجع type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { أرجع type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool عامل==(ثابت Module_شے &other) ثابت { أرجع ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); أرجع h; }
};

ساكن مضمن فراغ log_dump_val_کامہ(typename Id_درخت<Module_شے>::Cursor cursor ) { لوگارتھم("%p ", cursor.نشانہ_2); log_dump_val_کامہ(cursor.scope_name); }

سانچا<typename T>
ساكن مضمن فراغ log_dump_val_کامہ(ثابت typename std::unique_ptr<T> &cursor ) { لوگارتھم("unique %p", cursor.get()); }

سانچا<typename O>
std::سمتیہ<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::سمتیہ<IdString> رستہ_2;
	لكل (خودکار ثابت &شے : object->get_hdlname_attribute())
		رستہ_2.ڈالو_پٹھ(twines.جوڑو("\\" + شے));
	bool synthetic = !name.is_عوامی() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	إذا (رستہ_2.خالی_تھاواں() && name.is_عوامی())
		رستہ_2.ڈالو_پٹھ(name);
	إذا (!رستہ_2.خالی_تھاواں() && synthetic) {
		رستہ_2.نکالو_پٹھ();
		رستہ_2.ڈالو_پٹھ(name);
	}
	أرجع رستہ_2;
}

سانچا<typename O>
std::pair<std::سمتیہ<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::سمتیہ<IdString> رستہ_2;
	IdString trailing = object->name;
	إذا (trailing.is_عوامی() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		لكل (خودکار ثابت &شے : object->get_hdlname_attribute())
			رستہ_2.ڈالو_پٹھ(twines.جوڑو("\\" + شے));
		إذا (!رستہ_2.خالی_تھاواں()) {
			trailing = رستہ_2.پٹھ();
			رستہ_2.نکالو_پٹھ();
		}
	} وإلا إذا (object->has_attribute(ID::hdlname)) {
		لكل (خودکار ثابت &شے : object->get_hdlname_attribute())
			رستہ_2.ڈالو_پٹھ(twines.جوڑو("\\" + شے));
		إذا (!رستہ_2.خالی_تھاواں()) {
			رستہ_2.نکالو_پٹھ();
		}
	} وإلا {
		لكل (خودکار ثابت &شے : ونڈو_ٹوکن(object->get_string_attribute(ID(scopename)), " "))
			رستہ_2.ڈالو_پٹھ(twines.جوڑو("\\" + شے));
	}
	أرجع {رستہ_2, trailing};
}

بنية Module_Hdlname_اشاریہ {
	تعريفنوع Id_درخت<Module_شے>::Cursor Cursor;

	RTLIL::Module *module;
	Id_درخت<Module_شے> درخت;
	dict<Module_شے, Cursor> lookup;

	Module_Hdlname_اشاریہ(RTLIL::Module *module) : module(module) {}

خاص:
	سانچا<typename I, typename Filter>
	فراغ اشاریہ_اشیاء(I begin, I سرا, Filter filter);

عام:
	// Index all wires and cells of the module
	فراغ اشاریہ();

	// Index all wires of the module
	فراغ اشاریہ_wires();

	// Index all cells of the module
	فراغ اشاریہ_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	فراغ اشاریہ_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	سانچا<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		خودکار pair = parse_scopename(object);
		أرجع {درخت.cursor(pair.پہلا), pair.دوجا};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::سمتیہ<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	سانچا<typename O>
	std::سمتیہ<std::string> sources(O *object) {
		خودکار pair = parse_scopename(object);
		std::سمتیہ<std::string> نتیجہ = scope_sources(درخت.cursor(pair.پہلا));
		نتیجہ.ڈالو_پٹھ(object->get_src_attribute());
		أرجع نتیجہ;
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
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_گن(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_سرا

#جے_انت
