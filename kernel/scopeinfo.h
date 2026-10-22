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

#اگر_تعریف_نشده SCOPEINFO_H
#تعریف_کن SCOPEINFO_H

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

قالب<typename T>
class Id_درخت
{
عام:
	بنية Cursor;

محمي:
	Id_درخت *والد = nullptr;
	IdString scope_name;
	صحيح عمق = 0;

	pool<IdString> نامها;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<Id_درخت>> subtrees;

	قالب<typename P, typename T_ref>
	ساكن Cursor do_درج(Id_درخت *درخت, P begin, P انتها, T_ref &&مقدار)
	{
		log_assert(begin != انتها && "path must be non-empty");
		طالما (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.خالی());
			درخت->نامها.درج(name);
			إذا (begin == انتها) {
				درخت->entries.emplace(name, std::forward<T_ref>(مقدار));
				أرجع Cursor(درخت, name);
			}
			خودکار &unique = درخت->subtrees[name];
			إذا (!unique) {
				unique.reset(new Id_درخت);
				unique->scope_name = name;
				unique->والد = درخت;
				unique->عمق = درخت->عمق + 1;
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
		دوست class Id_درخت;
	محمي:
	عام:
		Id_درخت *هدف_2;
		IdString scope_name;

		Cursor() : هدف_2(nullptr) {}
		Cursor(Id_درخت *هدف_2, IdString scope_name) : هدف_2(هدف_2), scope_name(scope_name) {
			إذا (scope_name.خالی())
				log_assert(هدف_2->والد == nullptr);
		}

		Cursor do_اولی_فرزند() {
			Id_درخت *درخت = nullptr;
			إذا (scope_name.خالی()) {
				درخت = هدف_2;
			} وإلا {
				خودکار یافت = هدف_2->subtrees.پیدا_کن(scope_name);
				إذا (یافت != هدف_2->subtrees.انتها()) {
					درخت = یافت->دومی.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			إذا (درخت->نامها.خالی()) {
				أرجع Cursor();
			}
			أرجع Cursor(درخت, *درخت->نامها.begin());
		}

		Cursor do_بعدی_sibling() {
			إذا (scope_name.خالی())
				أرجع Cursor();
			خودکار یافت = هدف_2->نامها.پیدا_کن(scope_name);
			إذا (یافت == هدف_2->نامها.انتها())
				أرجع Cursor();
			++یافت;
			إذا (یافت == هدف_2->نامها.انتها())
				أرجع Cursor();
			أرجع Cursor(هدف_2, *یافت);
		}

		Cursor do_والد() {
			إذا (scope_name.خالی())
				أرجع Cursor();
			إذا (هدف_2->والد != nullptr)
				أرجع Cursor(هدف_2->والد, هدف_2->scope_name);
			أرجع Cursor(هدف_2, IdString());
		}

		Cursor do_بعدی_پیشترتیب() {
			Cursor current = *this;
			Cursor بعدی = current.do_اولی_فرزند();
			إذا (بعدی.valid())
				أرجع بعدی;
			طالما (current.valid()) {
				إذا (بعدی.valid())
					أرجع بعدی;
				بعدی = current.do_بعدی_sibling();
				إذا (بعدی.valid())
					أرجع بعدی;
				current = current.do_والد();
			}
			أرجع current;
		}

		Cursor do_فرزند(IdString name) {
			Id_درخت *درخت = nullptr;
			إذا (scope_name.خالی()) {
				درخت = هدف_2;
			} وإلا {
				خودکار یافت = هدف_2->subtrees.پیدا_کن(scope_name);
				إذا (یافت != هدف_2->subtrees.انتها()) {
					درخت = یافت->دومی.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			خودکار یافت = درخت->نامها.پیدا_کن(name);
			إذا (یافت == درخت->نامها.انتها()) {
				أرجع Cursor();
			}
			أرجع Cursor(درخت, *یافت);
		}

	عام:
		bool عملگر==(ثابت Cursor &other) ثابت {
			أرجع هدف_2 == other.هدف_2 && scope_name == other.scope_name;
		}
		bool عملگر!=(ثابت Cursor &other) ثابت {
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

		صحيح عمق() ثابت {
			log_assert(valid());
			أرجع هدف_2->عمق + !scope_name.خالی();
		}

		bool is_ریشه() ثابت {
			أرجع هدف_2 != nullptr && scope_name.خالی();
		}

		bool has_مدخل() ثابت {
			log_assert(valid());
			أرجع !scope_name.خالی() && هدف_2->entries.شمار(scope_name);
		}

		T &مدخل() {
			log_assert(!scope_name.خالی());
			أرجع هدف_2->entries.at(scope_name);
		}

		فراغ تخصیص_بده_مسیر_مقصد(std::وکتور<IdString> &بیرون_مسیر) {
			log_assert(valid());
			بیرون_مسیر.clear();
			إذا (scope_name.خالی())
				أرجع;
			بیرون_مسیر.افزودن_پشت(scope_name);
			Id_درخت *current = هدف_2;
			طالما (current->والد) {
				بیرون_مسیر.افزودن_پشت(current->scope_name);
				current = current->والد;
			}
			std::معکوس(بیرون_مسیر.begin(), بیرون_مسیر.انتها());
		}

		std::وکتور<IdString> مسیر_2() {
			std::وکتور<IdString> نتیجه;
			تخصیص_بده_مسیر_مقصد(نتیجه);
			أرجع نتیجه;
		}

		std::string مسیر_str(ثابت RTLIL::Design *design) {
			std::string نتیجه;
			لكل (ثابت خودکار &قلم : مسیر_2()) {
				إذا (!نتیجه.خالی())
					نتیجه.افزودن_پشت(' ');
				نتیجه += design->twines().unescaped_str(قلم);
			}
			أرجع نتیجه;
		}

		Cursor اولی_فرزند() {
			log_assert(valid());
			أرجع do_اولی_فرزند();
		}

		Cursor بعدی_پیشترتیب() {
			log_assert(valid());
			أرجع do_بعدی_پیشترتیب();
		}

		Cursor والد() {
			log_assert(valid());
			أرجع do_والد();
		}

		Cursor فرزند(IdString name) {
			log_assert(valid());
			أرجع do_فرزند(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			طالما (current != other) {
				إذا (!current.valid() || !other.valid())
					أرجع Cursor();
				صحيح delta = current.عمق() - other.عمق();
				إذا (delta >= 0)
					current = current.do_والد();
				إذا (delta <= 0)
					other = other.do_والد();
			}
			أرجع current;
		}
	};

	قالب<typename P>
	Cursor درج(P begin, P انتها, ثابت T &مقدار) {
		أرجع do_درج(this, begin, انتها, مقدار);
	}

	قالب<typename P>
	Cursor درج(P begin, P انتها, T &&مقدار) {
		أرجع do_درج(this, begin, انتها, std::move(مقدار));
	}

	قالب<typename P>
	Cursor درج(ثابت P &مسیر_2, ثابت T &مقدار) {
		أرجع do_درج(this, مسیر_2.begin(), مسیر_2.انتها(), مقدار);
	}

	قالب<typename P>
	Cursor درج(ثابت P &مسیر_2, T &&مقدار) {
		أرجع do_درج(this, مسیر_2.begin(), مسیر_2.انتها(), std::move(مقدار));
	}

	Cursor cursor() {
		أرجع والد ? Cursor(this->والد, this->scope_name) : Cursor(this, IdString());
	}

	قالب<typename P>
	Cursor cursor(P begin, P انتها) {
		Cursor current = cursor();
		لكل (; begin != انتها; ++begin) {
			current = current.do_فرزند(*begin);
			إذا (!current.valid())
				اكسر;
		}
		أرجع current;
	}

	قالب<typename P>
	Cursor cursor(ثابت P &مسیر_2) {
		أرجع cursor(مسیر_2.begin(), مسیر_2.انتها());
	}
};


بنية Module_قلم {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	Module_قلم(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_قلم(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { أرجع type == Type::Wire; }
	bool is_cell() ثابت { أرجع type == Type::Cell; }

	Wire *wire() ثابت { أرجع type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { أرجع type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool عملگر==(ثابت Module_قلم &other) ثابت { أرجع ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); أرجع h; }
};

ساكن مضمن فراغ log_dump_val_کارگر(typename Id_درخت<Module_قلم>::Cursor cursor ) { لگاریتم("%p ", cursor.هدف_2); log_dump_val_کارگر(cursor.scope_name); }

قالب<typename T>
ساكن مضمن فراغ log_dump_val_کارگر(ثابت typename std::unique_ptr<T> &cursor ) { لگاریتم("unique %p", cursor.get()); }

قالب<typename O>
std::وکتور<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::وکتور<IdString> مسیر_2;
	لكل (خودکار ثابت &قلم : object->get_hdlname_attribute())
		مسیر_2.افزودن_پشت(twines.جمع_کن("\\" + قلم));
	bool synthetic = !name.is_عمومی() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	إذا (مسیر_2.خالی() && name.is_عمومی())
		مسیر_2.افزودن_پشت(name);
	إذا (!مسیر_2.خالی() && synthetic) {
		مسیر_2.برداشت_پشت();
		مسیر_2.افزودن_پشت(name);
	}
	أرجع مسیر_2;
}

قالب<typename O>
std::pair<std::وکتور<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::وکتور<IdString> مسیر_2;
	IdString trailing = object->name;
	إذا (trailing.is_عمومی() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		لكل (خودکار ثابت &قلم : object->get_hdlname_attribute())
			مسیر_2.افزودن_پشت(twines.جمع_کن("\\" + قلم));
		إذا (!مسیر_2.خالی()) {
			trailing = مسیر_2.پشت();
			مسیر_2.برداشت_پشت();
		}
	} وإلا إذا (object->has_attribute(ID::hdlname)) {
		لكل (خودکار ثابت &قلم : object->get_hdlname_attribute())
			مسیر_2.افزودن_پشت(twines.جمع_کن("\\" + قلم));
		إذا (!مسیر_2.خالی()) {
			مسیر_2.برداشت_پشت();
		}
	} وإلا {
		لكل (خودکار ثابت &قلم : تقسیم_نشانهها(object->get_string_attribute(ID(scopename)), " "))
			مسیر_2.افزودن_پشت(twines.جمع_کن("\\" + قلم));
	}
	أرجع {مسیر_2, trailing};
}

بنية Module_Hdlname_شاخص {
	تعريفنوع Id_درخت<Module_قلم>::Cursor Cursor;

	RTLIL::Module *module;
	Id_درخت<Module_قلم> درخت;
	dict<Module_قلم, Cursor> lookup;

	Module_Hdlname_شاخص(RTLIL::Module *module) : module(module) {}

خاص:
	قالب<typename I, typename Filter>
	فراغ شاخص_اقلام(I begin, I انتها, Filter filter);

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
	قالب<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		خودکار pair = parse_scopename(object);
		أرجع {درخت.cursor(pair.اولی), pair.دومی};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::وکتور<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	قالب<typename O>
	std::وکتور<std::string> sources(O *object) {
		خودکار pair = parse_scopename(object);
		std::وکتور<std::string> نتیجه = scope_sources(درخت.cursor(pair.اولی));
		نتیجه.افزودن_پشت(object->get_src_attribute());
		أرجع نتیجه;
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
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_صفات(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_انتها

#پایان_اگر
