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

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_درەخت
{
عام:
	بنية Cursor;

محمي:
	Id_درەخت *parent = nullptr;
	IdString scope_name;
	صحيح depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<Id_درەخت>> subtrees;

	template<typename P, typename T_ref>
	ساكن Cursor do_زیادکردن(Id_درەخت *درەخت, P begin, P end, T_ref &&بەها)
	{
		log_assert(begin != end && "path must be non-empty");
		طالما (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			درەخت->names.زیادکردن(name);
			إذا (begin == end) {
				درەخت->entries.emplace(name, std::forward<T_ref>(بەها));
				أرجع Cursor(درەخت, name);
			}
			auto &unique = درەخت->subtrees[name];
			إذا (!unique) {
				unique.reset(new Id_درەخت);
				unique->scope_name = name;
				unique->parent = درەخت;
				unique->depth = درەخت->depth + 1;
			}
			درەخت = unique.get();
		}
	}

عام:
	Id_درەخت() = افتراضي;
	Id_درەخت(ثابت Id_درەخت &) = delete;
	Id_درەخت(Id_درەخت &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	بنية Cursor
	{
		friend class Id_درەخت;
	محمي:
	عام:
		Id_درەخت *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_درەخت *target, IdString scope_name) : target(target), scope_name(scope_name) {
			إذا (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_درەخت *درەخت = nullptr;
			إذا (scope_name.empty()) {
				درەخت = target;
			} وإلا {
				auto found = target->subtrees.find(scope_name);
				إذا (found != target->subtrees.end()) {
					درەخت = found->second.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			إذا (درەخت->names.empty()) {
				أرجع Cursor();
			}
			أرجع Cursor(درەخت, *درەخت->names.begin());
		}

		Cursor do_next_sibling() {
			إذا (scope_name.empty())
				أرجع Cursor();
			auto found = target->names.find(scope_name);
			إذا (found == target->names.end())
				أرجع Cursor();
			++found;
			إذا (found == target->names.end())
				أرجع Cursor();
			أرجع Cursor(target, *found);
		}

		Cursor do_parent() {
			إذا (scope_name.empty())
				أرجع Cursor();
			إذا (target->parent != nullptr)
				أرجع Cursor(target->parent, target->scope_name);
			أرجع Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			إذا (next.valid())
				أرجع next;
			طالما (current.valid()) {
				إذا (next.valid())
					أرجع next;
				next = current.do_next_sibling();
				إذا (next.valid())
					أرجع next;
				current = current.do_parent();
			}
			أرجع current;
		}

		Cursor do_child(IdString name) {
			Id_درەخت *درەخت = nullptr;
			إذا (scope_name.empty()) {
				درەخت = target;
			} وإلا {
				auto found = target->subtrees.find(scope_name);
				إذا (found != target->subtrees.end()) {
					درەخت = found->second.get();
				} وإلا {
					أرجع Cursor();
				}
			}
			auto found = درەخت->names.find(name);
			إذا (found == درەخت->names.end()) {
				أرجع Cursor();
			}
			أرجع Cursor(درەخت, *found);
		}

	عام:
		bool operator==(ثابت Cursor &other) ثابت {
			أرجع target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ثابت Cursor &other) ثابت {
			أرجع !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(target);
			أرجع h;
		}

		bool valid() ثابت {
			أرجع target != nullptr;
		}

		صحيح depth() ثابت {
			log_assert(valid());
			أرجع target->depth + !scope_name.empty();
		}

		bool is_ڕەگ() ثابت {
			أرجع target != nullptr && scope_name.empty();
		}

		bool has_entry() ثابت {
			log_assert(valid());
			أرجع !scope_name.empty() && target->entries.ژماردن(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			أرجع target->entries.at(scope_name);
		}

		فراغ assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			إذا (scope_name.empty())
				أرجع;
			out_path.پاڵنان_back(scope_name);
			Id_درەخت *current = target;
			طالما (current->parent) {
				out_path.پاڵنان_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			أرجع result;
		}

		std::string path_str(ثابت RTLIL::Design *design) {
			std::string result;
			لكل (ثابت auto &item : path()) {
				إذا (!result.empty())
					result.پاڵنان_back(' ');
				result += design->twines().unescaped_str(item);
			}
			أرجع result;
		}

		Cursor first_child() {
			log_assert(valid());
			أرجع do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			أرجع do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			أرجع do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			أرجع do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			طالما (current != other) {
				إذا (!current.valid() || !other.valid())
					أرجع Cursor();
				صحيح delta = current.depth() - other.depth();
				إذا (delta >= 0)
					current = current.do_parent();
				إذا (delta <= 0)
					other = other.do_parent();
			}
			أرجع current;
		}
	};

	template<typename P>
	Cursor زیادکردن(P begin, P end, ثابت T &بەها) {
		أرجع do_زیادکردن(this, begin, end, بەها);
	}

	template<typename P>
	Cursor زیادکردن(P begin, P end, T &&بەها) {
		أرجع do_زیادکردن(this, begin, end, std::move(بەها));
	}

	template<typename P>
	Cursor زیادکردن(ثابت P &path, ثابت T &بەها) {
		أرجع do_زیادکردن(this, path.begin(), path.end(), بەها);
	}

	template<typename P>
	Cursor زیادکردن(ثابت P &path, T &&بەها) {
		أرجع do_زیادکردن(this, path.begin(), path.end(), std::move(بەها));
	}

	Cursor cursor() {
		أرجع parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		لكل (; begin != end; ++begin) {
			current = current.do_child(*begin);
			إذا (!current.valid())
				اكسر;
		}
		أرجع current;
	}

	template<typename P>
	Cursor cursor(ثابت P &path) {
		أرجع cursor(path.begin(), path.end());
	}
};


بنية ModuleItem {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { أرجع type == Type::Wire; }
	bool is_cell() ثابت { أرجع type == Type::Cell; }

	Wire *wire() ثابت { أرجع type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { أرجع type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ثابت ModuleItem &other) ثابت { أرجع ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); أرجع h; }
};

ساكن مضمن فراغ log_dump_val_worker(typename Id_درەخت<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
ساكن مضمن فراغ log_dump_val_worker(ثابت typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	لكل (auto ثابت &item : object->get_hdlname_attribute())
		path.پاڵنان_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	إذا (path.empty() && name.isPublic())
		path.پاڵنان_back(name);
	إذا (!path.empty() && synthetic) {
		path.دەرهێنان_back();
		path.پاڵنان_back(name);
	}
	أرجع path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	إذا (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		لكل (auto ثابت &item : object->get_hdlname_attribute())
			path.پاڵنان_back(twines.add("\\" + item));
		إذا (!path.empty()) {
			trailing = path.back();
			path.دەرهێنان_back();
		}
	} وإلا إذا (object->has_attribute(ID::hdlname)) {
		لكل (auto ثابت &item : object->get_hdlname_attribute())
			path.پاڵنان_back(twines.add("\\" + item));
		إذا (!path.empty()) {
			path.دەرهێنان_back();
		}
	} وإلا {
		لكل (auto ثابت &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.پاڵنان_back(twines.add("\\" + item));
	}
	أرجع {path, trailing};
}

بنية ModuleHdlnameIndex {
	تعريفنوع Id_درەخت<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_درەخت<ModuleItem> درەخت;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

خاص:
	template<typename I, typename Filter>
	فراغ index_کاڵاکان(I begin, I end, Filter filter);

عام:
	// Index all wires and cells of the module
	فراغ index();

	// Index all wires of the module
	فراغ index_wires();

	// Index all cells of the module
	فراغ index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	فراغ index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		أرجع {درەخت.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(درەخت.cursor(pair.first));
		result.پاڵنان_back(object->get_src_attribute());
		أرجع result;
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
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
