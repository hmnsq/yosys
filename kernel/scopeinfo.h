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

#جيڪڏهن_وصف_نه_ٿيل SCOPEINFO_H
#وصف_ڪريو SCOPEINFO_H

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

سانچو<typename T>
class Id_وڻ
{
عام:
	بنية Cursor;

محمي:
	Id_وڻ *والد = nullptr;
	IdString scope_name;
	صحيح اونهائي = 0;

	pool<IdString> نالا;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<Id_وڻ>> subtrees;

	سانچو<typename P, typename T_ref>
	ساكن Cursor do_وجهو(Id_وڻ *وڻ, P begin, P پڇاڙي, T_ref &&قدر)
	{
		log_assert(begin != پڇاڙي && "path must be non-empty");
		طالما (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.خالي_جايون());
			وڻ->نالا.وجهو(name);
			إذا (begin == پڇاڙي) {
				وڻ->entries.emplace(name, std::forward<T_ref>(قدر));
				أرجع Cursor(وڻ, name);
			}
			خودڪار &unique = وڻ->subtrees[name];
			إذا (!unique) {
				unique.reset(new Id_وڻ);
				unique->scope_name = name;
				unique->والد = وڻ;
				unique->اونهائي = وڻ->اونهائي + 1;
			}
			وڻ = unique.get();
		}
	}

عام:
	Id_وڻ() = افتراضي;
	Id_وڻ(ثابت Id_وڻ &) = delete;
	Id_وڻ(Id_وڻ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	بنية Cursor
	{
		دوست class Id_وڻ;
	محمي:
	عام:
		Id_وڻ *هدف_2;
		IdString scope_name;

		Cursor() : هدف_2(nullptr) {}
		Cursor(Id_وڻ *هدف_2, IdString scope_name) : هدف_2(هدف_2), scope_name(scope_name) {
			إذا (scope_name.خالي_جايون())
				log_assert(هدف_2->والد == nullptr);
		}

		Cursor do_پهريون_ٻار() {
			Id_وڻ *وڻ = nullptr;
			إذا (scope_name.خالي_جايون()) {
				وڻ = هدف_2;
			} وإلا {
				خودڪار مليو = هدف_2->subtrees.ڳوليو(scope_name);
				إذا (مليو != هدف_2->subtrees.پڇاڙي()) {
					وڻ = مليو->ٻيو.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			إذا (وڻ->نالا.خالي_جايون()) {
				أرجع Cursor();
			}
			أرجع Cursor(وڻ, *وڻ->نالا.begin());
		}

		Cursor do_ايندڙ_sibling() {
			إذا (scope_name.خالي_جايون())
				أرجع Cursor();
			خودڪار مليو = هدف_2->نالا.ڳوليو(scope_name);
			إذا (مليو == هدف_2->نالا.پڇاڙي())
				أرجع Cursor();
			++مليو;
			إذا (مليو == هدف_2->نالا.پڇاڙي())
				أرجع Cursor();
			أرجع Cursor(هدف_2, *مليو);
		}

		Cursor do_والد() {
			إذا (scope_name.خالي_جايون())
				أرجع Cursor();
			إذا (هدف_2->والد != nullptr)
				أرجع Cursor(هدف_2->والد, هدف_2->scope_name);
			أرجع Cursor(هدف_2, IdString());
		}

		Cursor do_ايندڙ_اڳين_ترتيب() {
			Cursor current = *this;
			Cursor ايندڙ = current.do_پهريون_ٻار();
			إذا (ايندڙ.valid())
				أرجع ايندڙ;
			طالما (current.valid()) {
				إذا (ايندڙ.valid())
					أرجع ايندڙ;
				ايندڙ = current.do_ايندڙ_sibling();
				إذا (ايندڙ.valid())
					أرجع ايندڙ;
				current = current.do_والد();
			}
			أرجع current;
		}

		Cursor do_ٻار(IdString name) {
			Id_وڻ *وڻ = nullptr;
			إذا (scope_name.خالي_جايون()) {
				وڻ = هدف_2;
			} وإلا {
				خودڪار مليو = هدف_2->subtrees.ڳوليو(scope_name);
				إذا (مليو != هدف_2->subtrees.پڇاڙي()) {
					وڻ = مليو->ٻيو.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			خودڪار مليو = وڻ->نالا.ڳوليو(name);
			إذا (مليو == وڻ->نالا.پڇاڙي()) {
				أرجع Cursor();
			}
			أرجع Cursor(وڻ, *مليو);
		}

	عام:
		bool عامل==(ثابت Cursor &other) ثابت {
			أرجع هدف_2 == other.هدف_2 && scope_name == other.scope_name;
		}
		bool عامل!=(ثابت Cursor &other) ثابت {
			أرجع !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(هدف_2);
			أرجع h;
		}

		bool valid() ثابت {
			أرجع هدف_2 != nullptr;
		}

		صحيح اونهائي() ثابت {
			log_assert(valid());
			أرجع هدف_2->اونهائي + !scope_name.خالي_جايون();
		}

		bool is_پاڙ() ثابت {
			أرجع هدف_2 != nullptr && scope_name.خالي_جايون();
		}

		bool has_داخلا_2() ثابت {
			log_assert(valid());
			أرجع !scope_name.خالي_جايون() && هدف_2->entries.ڳڻپ(scope_name);
		}

		T &داخلا_2() {
			log_assert(!scope_name.خالي_جايون());
			أرجع هدف_2->entries.at(scope_name);
		}

		فراغ سونپيو_رستو_منزل(std::ويڪٽر<IdString> &ٻاهريون_رستو) {
			log_assert(valid());
			ٻاهريون_رستو.clear();
			إذا (scope_name.خالي_جايون())
				أرجع;
			ٻاهريون_رستو.ڌڪو_پٺ(scope_name);
			Id_وڻ *current = هدف_2;
			طالما (current->والد) {
				ٻاهريون_رستو.ڌڪو_پٺ(current->scope_name);
				current = current->والد;
			}
			std::ابتو(ٻاهريون_رستو.begin(), ٻاهريون_رستو.پڇاڙي());
		}

		std::ويڪٽر<IdString> رستو_2() {
			std::ويڪٽر<IdString> نتيجو;
			سونپيو_رستو_منزل(نتيجو);
			أرجع نتيجو;
		}

		std::string رستو_str(ثابت RTLIL::Design *design) {
			std::string نتيجو;
			لكل (ثابت خودڪار &شي : رستو_2()) {
				إذا (!نتيجو.خالي_جايون())
					نتيجو.ڌڪو_پٺ(' ');
				نتيجو += design->twines().unescaped_str(شي);
			}
			أرجع نتيجو;
		}

		Cursor پهريون_ٻار() {
			log_assert(valid());
			أرجع do_پهريون_ٻار();
		}

		Cursor ايندڙ_اڳين_ترتيب() {
			log_assert(valid());
			أرجع do_ايندڙ_اڳين_ترتيب();
		}

		Cursor والد() {
			log_assert(valid());
			أرجع do_والد();
		}

		Cursor ٻار(IdString name) {
			log_assert(valid());
			أرجع do_ٻار(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			طالما (current != other) {
				إذا (!current.valid() || !other.valid())
					أرجع Cursor();
				صحيح delta = current.اونهائي() - other.اونهائي();
				إذا (delta >= 0)
					current = current.do_والد();
				إذا (delta <= 0)
					other = other.do_والد();
			}
			أرجع current;
		}
	};

	سانچو<typename P>
	Cursor وجهو(P begin, P پڇاڙي, ثابت T &قدر) {
		أرجع do_وجهو(this, begin, پڇاڙي, قدر);
	}

	سانچو<typename P>
	Cursor وجهو(P begin, P پڇاڙي, T &&قدر) {
		أرجع do_وجهو(this, begin, پڇاڙي, std::move(قدر));
	}

	سانچو<typename P>
	Cursor وجهو(ثابت P &رستو_2, ثابت T &قدر) {
		أرجع do_وجهو(this, رستو_2.begin(), رستو_2.پڇاڙي(), قدر);
	}

	سانچو<typename P>
	Cursor وجهو(ثابت P &رستو_2, T &&قدر) {
		أرجع do_وجهو(this, رستو_2.begin(), رستو_2.پڇاڙي(), std::move(قدر));
	}

	Cursor cursor() {
		أرجع والد ? Cursor(this->والد, this->scope_name) : Cursor(this, IdString());
	}

	سانچو<typename P>
	Cursor cursor(P begin, P پڇاڙي) {
		Cursor current = cursor();
		لكل (; begin != پڇاڙي; ++begin) {
			current = current.do_ٻار(*begin);
			إذا (!current.valid())
				اكسر;
		}
		أرجع current;
	}

	سانچو<typename P>
	Cursor cursor(ثابت P &رستو_2) {
		أرجع cursor(رستو_2.begin(), رستو_2.پڇاڙي());
	}
};


بنية Module_شي {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	Module_شي(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_شي(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { أرجع type == Type::Wire; }
	bool is_cell() ثابت { أرجع type == Type::Cell; }

	Wire *wire() ثابت { أرجع type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { أرجع type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool عامل==(ثابت Module_شي &other) ثابت { أرجع ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); أرجع h; }
};

ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(typename Id_وڻ<Module_شي>::Cursor cursor ) { لاگرٿم("%p ", cursor.هدف_2); log_dump_val_ڪم_ڪندڙ(cursor.scope_name); }

سانچو<typename T>
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(ثابت typename std::unique_ptr<T> &cursor ) { لاگرٿم("unique %p", cursor.get()); }

سانچو<typename O>
std::ويڪٽر<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::ويڪٽر<IdString> رستو_2;
	لكل (خودڪار ثابت &شي : object->get_hdlname_attribute())
		رستو_2.ڌڪو_پٺ(twines.جوڙيو("\\" + شي));
	bool synthetic = !name.is_عوامي() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	إذا (رستو_2.خالي_جايون() && name.is_عوامي())
		رستو_2.ڌڪو_پٺ(name);
	إذا (!رستو_2.خالي_جايون() && synthetic) {
		رستو_2.ڪڍو_پٺ();
		رستو_2.ڌڪو_پٺ(name);
	}
	أرجع رستو_2;
}

سانچو<typename O>
std::pair<std::ويڪٽر<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::ويڪٽر<IdString> رستو_2;
	IdString trailing = object->name;
	إذا (trailing.is_عوامي() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		لكل (خودڪار ثابت &شي : object->get_hdlname_attribute())
			رستو_2.ڌڪو_پٺ(twines.جوڙيو("\\" + شي));
		إذا (!رستو_2.خالي_جايون()) {
			trailing = رستو_2.پٺ();
			رستو_2.ڪڍو_پٺ();
		}
	} وإلا إذا (object->has_attribute(ID::hdlname)) {
		لكل (خودڪار ثابت &شي : object->get_hdlname_attribute())
			رستو_2.ڌڪو_پٺ(twines.جوڙيو("\\" + شي));
		إذا (!رستو_2.خالي_جايون()) {
			رستو_2.ڪڍو_پٺ();
		}
	} وإلا {
		لكل (خودڪار ثابت &شي : ورهايو_ٽوڪن(object->get_string_attribute(ID(scopename)), " "))
			رستو_2.ڌڪو_پٺ(twines.جوڙيو("\\" + شي));
	}
	أرجع {رستو_2, trailing};
}

بنية Module_Hdlname_اشاريو {
	تعريفنوع Id_وڻ<Module_شي>::Cursor Cursor;

	RTLIL::Module *module;
	Id_وڻ<Module_شي> وڻ;
	dict<Module_شي, Cursor> lookup;

	Module_Hdlname_اشاريو(RTLIL::Module *module) : module(module) {}

خاص:
	سانچو<typename I, typename Filter>
	فراغ اشاريو_شيون(I begin, I پڇاڙي, Filter filter);

عام:
	// Index all wires and cells of the module
	فراغ اشاريو();

	// Index all wires of the module
	فراغ اشاريو_wires();

	// Index all cells of the module
	فراغ اشاريو_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	فراغ اشاريو_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	سانچو<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		خودڪار pair = parse_scopename(object);
		أرجع {وڻ.cursor(pair.پهريون), pair.ٻيو};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::ويڪٽر<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	سانچو<typename O>
	std::ويڪٽر<std::string> sources(O *object) {
		خودڪار pair = parse_scopename(object);
		std::ويڪٽر<std::string> نتيجو = scope_sources(وڻ.cursor(pair.پهريون));
		نتيجو.ڌڪو_پٺ(object->get_src_attribute());
		أرجع نتيجو;
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
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_خاصيتون(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
