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

#ifndef SCOPEINFO_H
#define SCOPEINFO_H

#شامل <vector>
#شامل <algorithm>

#شامل "kernel/yosys.h"
#شامل "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_درخت
{
عمومی:
	ساختار Cursor;

محافظتشده:
	Id_درخت *parent = nullptr;
	IdString scope_name;
	صحیح depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
عمومی: // XXX
	dict<IdString, std::unique_ptr<Id_درخت>> subtrees;

	template<typename P, typename T_ref>
	ایستا Cursor do_درج(Id_درخت *درخت, P begin, P end, T_ref &&مقدار)
	{
		log_assert(begin != end && "path must be non-empty");
		وقتی (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			درخت->names.درج(name);
			اگر (begin == end) {
				درخت->entries.emplace(name, std::forward<T_ref>(مقدار));
				بازگشت Cursor(درخت, name);
			}
			auto &unique = درخت->subtrees[name];
			اگر (!unique) {
				unique.reset(new Id_درخت);
				unique->scope_name = name;
				unique->parent = درخت;
				unique->depth = درخت->depth + 1;
			}
			درخت = unique.get();
		}
	}

عمومی:
	Id_درخت() = پیشفرض;
	Id_درخت(ثابت Id_درخت &) = delete;
	Id_درخت(Id_درخت &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ساختار Cursor
	{
		friend class Id_درخت;
	محافظتشده:
	عمومی:
		Id_درخت *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_درخت *target, IdString scope_name) : target(target), scope_name(scope_name) {
			اگر (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_درخت *درخت = nullptr;
			اگر (scope_name.empty()) {
				درخت = target;
			} وگرنه {
				auto found = target->subtrees.find(scope_name);
				اگر (found != target->subtrees.end()) {
					درخت = found->second.get();
				} وگرنه {
					بازگشت Cursor();
				}
			}
			اگر (درخت->names.empty()) {
				بازگشت Cursor();
			}
			بازگشت Cursor(درخت, *درخت->names.begin());
		}

		Cursor do_next_sibling() {
			اگر (scope_name.empty())
				بازگشت Cursor();
			auto found = target->names.find(scope_name);
			اگر (found == target->names.end())
				بازگشت Cursor();
			++found;
			اگر (found == target->names.end())
				بازگشت Cursor();
			بازگشت Cursor(target, *found);
		}

		Cursor do_parent() {
			اگر (scope_name.empty())
				بازگشت Cursor();
			اگر (target->parent != nullptr)
				بازگشت Cursor(target->parent, target->scope_name);
			بازگشت Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			اگر (next.valid())
				بازگشت next;
			وقتی (current.valid()) {
				اگر (next.valid())
					بازگشت next;
				next = current.do_next_sibling();
				اگر (next.valid())
					بازگشت next;
				current = current.do_parent();
			}
			بازگشت current;
		}

		Cursor do_child(IdString name) {
			Id_درخت *درخت = nullptr;
			اگر (scope_name.empty()) {
				درخت = target;
			} وگرنه {
				auto found = target->subtrees.find(scope_name);
				اگر (found != target->subtrees.end()) {
					درخت = found->second.get();
				} وگرنه {
					بازگشت Cursor();
				}
			}
			auto found = درخت->names.find(name);
			اگر (found == درخت->names.end()) {
				بازگشت Cursor();
			}
			بازگشت Cursor(درخت, *found);
		}

	عمومی:
		bool operator==(ثابت Cursor &other) ثابت {
			بازگشت target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ثابت Cursor &other) ثابت {
			بازگشت !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(target);
			بازگشت h;
		}

		bool valid() ثابت {
			بازگشت target != nullptr;
		}

		صحیح depth() ثابت {
			log_assert(valid());
			بازگشت target->depth + !scope_name.empty();
		}

		bool is_ریشه() ثابت {
			بازگشت target != nullptr && scope_name.empty();
		}

		bool has_entry() ثابت {
			log_assert(valid());
			بازگشت !scope_name.empty() && target->entries.شمار(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			بازگشت target->entries.at(scope_name);
		}

		تهی assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			اگر (scope_name.empty())
				بازگشت;
			out_path.افزودن_back(scope_name);
			Id_درخت *current = target;
			وقتی (current->parent) {
				out_path.افزودن_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			بازگشت result;
		}

		std::string path_str(ثابت RTLIL::Design *design) {
			std::string result;
			برای (ثابت auto &item : path()) {
				اگر (!result.empty())
					result.افزودن_back(' ');
				result += design->twines().unescaped_str(item);
			}
			بازگشت result;
		}

		Cursor first_child() {
			log_assert(valid());
			بازگشت do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			بازگشت do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			بازگشت do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			بازگشت do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			وقتی (current != other) {
				اگر (!current.valid() || !other.valid())
					بازگشت Cursor();
				صحیح delta = current.depth() - other.depth();
				اگر (delta >= 0)
					current = current.do_parent();
				اگر (delta <= 0)
					other = other.do_parent();
			}
			بازگشت current;
		}
	};

	template<typename P>
	Cursor درج(P begin, P end, ثابت T &مقدار) {
		بازگشت do_درج(this, begin, end, مقدار);
	}

	template<typename P>
	Cursor درج(P begin, P end, T &&مقدار) {
		بازگشت do_درج(this, begin, end, std::move(مقدار));
	}

	template<typename P>
	Cursor درج(ثابت P &path, ثابت T &مقدار) {
		بازگشت do_درج(this, path.begin(), path.end(), مقدار);
	}

	template<typename P>
	Cursor درج(ثابت P &path, T &&مقدار) {
		بازگشت do_درج(this, path.begin(), path.end(), std::move(مقدار));
	}

	Cursor cursor() {
		بازگشت parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		برای (; begin != end; ++begin) {
			current = current.do_child(*begin);
			اگر (!current.valid())
				بشکن;
		}
		بازگشت current;
	}

	template<typename P>
	Cursor cursor(ثابت P &path) {
		بازگشت cursor(path.begin(), path.end());
	}
};


ساختار ModuleItem {
	شمارش class Type {
		Wire,
		Cell,
	};
	Type type;
	تهی *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { بازگشت type == Type::Wire; }
	bool is_cell() ثابت { بازگشت type == Type::Cell; }

	Wire *wire() ثابت { بازگشت type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { بازگشت type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ثابت ModuleItem &other) ثابت { بازگشت ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); بازگشت h; }
};

ایستا درونخطی تهی log_dump_val_worker(typename Id_درخت<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
ایستا درونخطی تهی log_dump_val_worker(ثابت typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	برای (auto ثابت &item : object->get_hdlname_attribute())
		path.افزودن_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	اگر (path.empty() && name.isPublic())
		path.افزودن_back(name);
	اگر (!path.empty() && synthetic) {
		path.برداشت_back();
		path.افزودن_back(name);
	}
	بازگشت path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	اگر (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		برای (auto ثابت &item : object->get_hdlname_attribute())
			path.افزودن_back(twines.add("\\" + item));
		اگر (!path.empty()) {
			trailing = path.back();
			path.برداشت_back();
		}
	} وگرنه اگر (object->has_attribute(ID::hdlname)) {
		برای (auto ثابت &item : object->get_hdlname_attribute())
			path.افزودن_back(twines.add("\\" + item));
		اگر (!path.empty()) {
			path.برداشت_back();
		}
	} وگرنه {
		برای (auto ثابت &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.افزودن_back(twines.add("\\" + item));
	}
	بازگشت {path, trailing};
}

ساختار ModuleHdlnameIndex {
	تعریفنوع Id_درخت<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_درخت<ModuleItem> درخت;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

خصوصی:
	template<typename I, typename Filter>
	تهی index_اقلام(I begin, I end, Filter filter);

عمومی:
	// Index all wires and cells of the module
	تهی index();

	// Index all wires of the module
	تهی index_wires();

	// Index all cells of the module
	تهی index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	تهی index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		بازگشت {درخت.cursor(pair.first), pair.second};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::vector<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	template<typename O>
	std::vector<std::string> sources(O *object) {
		auto pair = parse_scopename(object);
		std::vector<std::string> result = scope_sources(درخت.cursor(pair.first));
		result.افزودن_back(object->get_src_attribute());
		بازگشت result;
	}
};

شمارش class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
