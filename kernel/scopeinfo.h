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

#اگر_غیر_تعریف_شدہ SCOPEINFO_H
#تعریف_کرو SCOPEINFO_H

#شامل <vector>
#شامل <algorithm>

#شامل "kernel/yosys.h"
#شامل "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

سانچہ<typename T>
class Id_درخت
{
عوامی:
	ساخت Cursor;

محفوظ:
	Id_درخت *والد = nullptr;
	IdString scope_name;
	صحیح گہرائی = 0;

	pool<IdString> نام;
	dict<IdString, T> entries;
عوامی: // XXX
	dict<IdString, std::unique_ptr<Id_درخت>> subtrees;

	سانچہ<typename P, typename T_ref>
	ساکن Cursor do_داخل_کرو(Id_درخت *درخت, P begin, P انتہا, T_ref &&قدر)
	{
		log_assert(begin != انتہا && "path must be non-empty");
		جب (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.خالی_خانے());
			درخت->نام.داخل_کرو(name);
			اگر (begin == انتہا) {
				درخت->entries.emplace(name, std::forward<T_ref>(قدر));
				واپس Cursor(درخت, name);
			}
			خودکار &unique = درخت->subtrees[name];
			اگر (!unique) {
				unique.reset(new Id_درخت);
				unique->scope_name = name;
				unique->والد = درخت;
				unique->گہرائی = درخت->گہرائی + 1;
			}
			درخت = unique.get();
		}
	}

عوامی:
	Id_درخت() = طےشدہ;
	Id_درخت(ثابت Id_درخت &) = delete;
	Id_درخت(Id_درخت &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ساخت Cursor
	{
		دوست class Id_درخت;
	محفوظ:
	عوامی:
		Id_درخت *ہدف_2;
		IdString scope_name;

		Cursor() : ہدف_2(nullptr) {}
		Cursor(Id_درخت *ہدف_2, IdString scope_name) : ہدف_2(ہدف_2), scope_name(scope_name) {
			اگر (scope_name.خالی_خانے())
				log_assert(ہدف_2->والد == nullptr);
		}

		Cursor do_پہلا_بچہ() {
			Id_درخت *درخت = nullptr;
			اگر (scope_name.خالی_خانے()) {
				درخت = ہدف_2;
			} ورنہ {
				خودکار ملا = ہدف_2->subtrees.ڈھونڈو(scope_name);
				اگر (ملا != ہدف_2->subtrees.انتہا()) {
					درخت = ملا->دوسرا.get();
				} ورنہ {
					واپس Cursor();
				}
			}
			اگر (درخت->نام.خالی_خانے()) {
				واپس Cursor();
			}
			واپس Cursor(درخت, *درخت->نام.begin());
		}

		Cursor do_اگلا_sibling() {
			اگر (scope_name.خالی_خانے())
				واپس Cursor();
			خودکار ملا = ہدف_2->نام.ڈھونڈو(scope_name);
			اگر (ملا == ہدف_2->نام.انتہا())
				واپس Cursor();
			++ملا;
			اگر (ملا == ہدف_2->نام.انتہا())
				واپس Cursor();
			واپس Cursor(ہدف_2, *ملا);
		}

		Cursor do_والد() {
			اگر (scope_name.خالی_خانے())
				واپس Cursor();
			اگر (ہدف_2->والد != nullptr)
				واپس Cursor(ہدف_2->والد, ہدف_2->scope_name);
			واپس Cursor(ہدف_2, IdString());
		}

		Cursor do_اگلا_پیش_ترتیب() {
			Cursor current = *this;
			Cursor اگلا = current.do_پہلا_بچہ();
			اگر (اگلا.valid())
				واپس اگلا;
			جب (current.valid()) {
				اگر (اگلا.valid())
					واپس اگلا;
				اگلا = current.do_اگلا_sibling();
				اگر (اگلا.valid())
					واپس اگلا;
				current = current.do_والد();
			}
			واپس current;
		}

		Cursor do_بچہ(IdString name) {
			Id_درخت *درخت = nullptr;
			اگر (scope_name.خالی_خانے()) {
				درخت = ہدف_2;
			} ورنہ {
				خودکار ملا = ہدف_2->subtrees.ڈھونڈو(scope_name);
				اگر (ملا != ہدف_2->subtrees.انتہا()) {
					درخت = ملا->دوسرا.get();
				} ورنہ {
					واپس Cursor();
				}
			}
			خودکار ملا = درخت->نام.ڈھونڈو(name);
			اگر (ملا == درخت->نام.انتہا()) {
				واپس Cursor();
			}
			واپس Cursor(درخت, *ملا);
		}

	عوامی:
		bool عامل==(ثابت Cursor &other) ثابت {
			واپس ہدف_2 == other.ہدف_2 && scope_name == other.scope_name;
		}
		bool عامل!=(ثابت Cursor &other) ثابت {
			واپس !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(ہدف_2);
			واپس h;
		}

		bool valid() ثابت {
			واپس ہدف_2 != nullptr;
		}

		صحیح گہرائی() ثابت {
			log_assert(valid());
			واپس ہدف_2->گہرائی + !scope_name.خالی_خانے();
		}

		bool is_جڑ() ثابت {
			واپس ہدف_2 != nullptr && scope_name.خالی_خانے();
		}

		bool has_اندراج() ثابت {
			log_assert(valid());
			واپس !scope_name.خالی_خانے() && ہدف_2->entries.گنتی(scope_name);
		}

		T &اندراج_2() {
			log_assert(!scope_name.خالی_خانے());
			واپس ہدف_2->entries.at(scope_name);
		}

		خالی تفویض_راستہ_مقصد(std::سمتیہ<IdString> &برآمد_راستہ) {
			log_assert(valid());
			برآمد_راستہ.clear();
			اگر (scope_name.خالی_خانے())
				واپس;
			برآمد_راستہ.ڈالو_عقب(scope_name);
			Id_درخت *current = ہدف_2;
			جب (current->والد) {
				برآمد_راستہ.ڈالو_عقب(current->scope_name);
				current = current->والد;
			}
			std::الٹ(برآمد_راستہ.begin(), برآمد_راستہ.انتہا());
		}

		std::سمتیہ<IdString> راستہ_2() {
			std::سمتیہ<IdString> نتیجہ;
			تفویض_راستہ_مقصد(نتیجہ);
			واپس نتیجہ;
		}

		std::string راستہ_str(ثابت RTLIL::Design *design) {
			std::string نتیجہ;
			برائے (ثابت خودکار &شے_2 : راستہ_2()) {
				اگر (!نتیجہ.خالی_خانے())
					نتیجہ.ڈالو_عقب(' ');
				نتیجہ += design->twines().unescaped_str(شے_2);
			}
			واپس نتیجہ;
		}

		Cursor پہلا_بچہ() {
			log_assert(valid());
			واپس do_پہلا_بچہ();
		}

		Cursor اگلا_پیش_ترتیب() {
			log_assert(valid());
			واپس do_اگلا_پیش_ترتیب();
		}

		Cursor والد() {
			log_assert(valid());
			واپس do_والد();
		}

		Cursor بچہ(IdString name) {
			log_assert(valid());
			واپس do_بچہ(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			جب (current != other) {
				اگر (!current.valid() || !other.valid())
					واپس Cursor();
				صحیح delta = current.گہرائی() - other.گہرائی();
				اگر (delta >= 0)
					current = current.do_والد();
				اگر (delta <= 0)
					other = other.do_والد();
			}
			واپس current;
		}
	};

	سانچہ<typename P>
	Cursor داخل_کرو(P begin, P انتہا, ثابت T &قدر) {
		واپس do_داخل_کرو(this, begin, انتہا, قدر);
	}

	سانچہ<typename P>
	Cursor داخل_کرو(P begin, P انتہا, T &&قدر) {
		واپس do_داخل_کرو(this, begin, انتہا, std::move(قدر));
	}

	سانچہ<typename P>
	Cursor داخل_کرو(ثابت P &راستہ_2, ثابت T &قدر) {
		واپس do_داخل_کرو(this, راستہ_2.begin(), راستہ_2.انتہا(), قدر);
	}

	سانچہ<typename P>
	Cursor داخل_کرو(ثابت P &راستہ_2, T &&قدر) {
		واپس do_داخل_کرو(this, راستہ_2.begin(), راستہ_2.انتہا(), std::move(قدر));
	}

	Cursor cursor() {
		واپس والد ? Cursor(this->والد, this->scope_name) : Cursor(this, IdString());
	}

	سانچہ<typename P>
	Cursor cursor(P begin, P انتہا) {
		Cursor current = cursor();
		برائے (; begin != انتہا; ++begin) {
			current = current.do_بچہ(*begin);
			اگر (!current.valid())
				توڑو;
		}
		واپس current;
	}

	سانچہ<typename P>
	Cursor cursor(ثابت P &راستہ_2) {
		واپس cursor(راستہ_2.begin(), راستہ_2.انتہا());
	}
};


ساخت Module_شے {
	شمار class Type {
		Wire,
		Cell,
	};
	Type type;
	خالی *ptr;

	Module_شے(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_شے(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { واپس type == Type::Wire; }
	bool is_cell() ثابت { واپس type == Type::Cell; }

	Wire *wire() ثابت { واپس type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { واپس type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool عامل==(ثابت Module_شے &other) ثابت { واپس ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); واپس h; }
};

ساکن اندرونی خالی log_dump_val_کارکن(typename Id_درخت<Module_شے>::Cursor cursor ) { لوگارتھم("%p ", cursor.ہدف_2); log_dump_val_کارکن(cursor.scope_name); }

سانچہ<typename T>
ساکن اندرونی خالی log_dump_val_کارکن(ثابت typename std::unique_ptr<T> &cursor ) { لوگارتھم("unique %p", cursor.get()); }

سانچہ<typename O>
std::سمتیہ<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::سمتیہ<IdString> راستہ_2;
	برائے (خودکار ثابت &شے_2 : object->get_hdlname_attribute())
		راستہ_2.ڈالو_عقب(twines.جوڑو("\\" + شے_2));
	bool synthetic = !name.is_عوامی() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	اگر (راستہ_2.خالی_خانے() && name.is_عوامی())
		راستہ_2.ڈالو_عقب(name);
	اگر (!راستہ_2.خالی_خانے() && synthetic) {
		راستہ_2.نکالو_عقب();
		راستہ_2.ڈالو_عقب(name);
	}
	واپس راستہ_2;
}

سانچہ<typename O>
std::pair<std::سمتیہ<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::سمتیہ<IdString> راستہ_2;
	IdString trailing = object->name;
	اگر (trailing.is_عوامی() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		برائے (خودکار ثابت &شے_2 : object->get_hdlname_attribute())
			راستہ_2.ڈالو_عقب(twines.جوڑو("\\" + شے_2));
		اگر (!راستہ_2.خالی_خانے()) {
			trailing = راستہ_2.عقب();
			راستہ_2.نکالو_عقب();
		}
	} ورنہ اگر (object->has_attribute(ID::hdlname)) {
		برائے (خودکار ثابت &شے_2 : object->get_hdlname_attribute())
			راستہ_2.ڈالو_عقب(twines.جوڑو("\\" + شے_2));
		اگر (!راستہ_2.خالی_خانے()) {
			راستہ_2.نکالو_عقب();
		}
	} ورنہ {
		برائے (خودکار ثابت &شے_2 : بانٹو_علامات(object->get_string_attribute(ID(scopename)), " "))
			راستہ_2.ڈالو_عقب(twines.جوڑو("\\" + شے_2));
	}
	واپس {راستہ_2, trailing};
}

ساخت Module_Hdlname_اشاریہ {
	قسمتعریف Id_درخت<Module_شے>::Cursor Cursor;

	RTLIL::Module *module;
	Id_درخت<Module_شے> درخت;
	dict<Module_شے, Cursor> lookup;

	Module_Hdlname_اشاریہ(RTLIL::Module *module) : module(module) {}

نجی:
	سانچہ<typename I, typename Filter>
	خالی اشاریہ_اشیاء(I begin, I انتہا, Filter filter);

عوامی:
	// Index all wires and cells of the module
	خالی اشاریہ();

	// Index all wires of the module
	خالی اشاریہ_wires();

	// Index all cells of the module
	خالی اشاریہ_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	خالی اشاریہ_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	سانچہ<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		خودکار pair = parse_scopename(object);
		واپس {درخت.cursor(pair.پہلا), pair.دوسرا};
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
	سانچہ<typename O>
	std::سمتیہ<std::string> sources(O *object) {
		خودکار pair = parse_scopename(object);
		std::سمتیہ<std::string> نتیجہ = scope_sources(درخت.cursor(pair.پہلا));
		نتیجہ.ڈالو_عقب(object->get_src_attribute());
		واپس نتیجہ;
	}
};

شمار class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_اوصاف(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_انتہا

#اگر_ختم
