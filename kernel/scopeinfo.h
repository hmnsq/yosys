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

#அந்தர்பாவ <vector>
#அந்தர்பாவ <algorithm>

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_மரம்
{
பொது:
	கட்டமைப்பு Cursor;

பாதுகாக்கப்பட்ட:
	Id_மரம் *parent = nullptr;
	IdString scope_name;
	முழு depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
பொது: // XXX
	dict<IdString, std::unique_ptr<Id_மரம்>> subtrees;

	template<typename P, typename T_ref>
	நிலையான Cursor do_செருகு(Id_மரம் *மரம், P begin, P end, T_ref &&மதிப்பு)
	{
		log_assert(begin != end && "path must be non-empty");
		போது (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			மரம்->names.செருகு(name);
			எனில் (begin == end) {
				மரம்->entries.emplace(name, std::forward<T_ref>(மதிப்பு));
				திரும்பு Cursor(மரம், name);
			}
			auto &unique = மரம்->subtrees[name];
			எனில் (!unique) {
				unique.reset(new Id_மரம்);
				unique->scope_name = name;
				unique->parent = மரம்;
				unique->depth = மரம்->depth + 1;
			}
			மரம் = unique.get();
		}
	}

பொது:
	Id_மரம்() = இயல்பு;
	Id_மரம்(மாறிலி Id_மரம் &) = delete;
	Id_மரம்(Id_மரம் &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	கட்டமைப்பு Cursor
	{
		friend class Id_மரம்;
	பாதுகாக்கப்பட்ட:
	பொது:
		Id_மரம் *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_மரம் *target, IdString scope_name) : target(target), scope_name(scope_name) {
			எனில் (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_மரம் *மரம் = nullptr;
			எனில் (scope_name.empty()) {
				மரம் = target;
			} இல்லையெனில் {
				auto found = target->subtrees.find(scope_name);
				எனில் (found != target->subtrees.end()) {
					மரம் = found->second.get();
				} இல்லையெனில் {
					திரும்பு Cursor();
				}
			}
			எனில் (மரம்->names.empty()) {
				திரும்பு Cursor();
			}
			திரும்பு Cursor(மரம், *மரம்->names.begin());
		}

		Cursor do_next_sibling() {
			எனில் (scope_name.empty())
				திரும்பு Cursor();
			auto found = target->names.find(scope_name);
			எனில் (found == target->names.end())
				திரும்பு Cursor();
			++found;
			எனில் (found == target->names.end())
				திரும்பு Cursor();
			திரும்பு Cursor(target, *found);
		}

		Cursor do_parent() {
			எனில் (scope_name.empty())
				திரும்பு Cursor();
			எனில் (target->parent != nullptr)
				திரும்பு Cursor(target->parent, target->scope_name);
			திரும்பு Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			எனில் (next.valid())
				திரும்பு next;
			போது (current.valid()) {
				எனில் (next.valid())
					திரும்பு next;
				next = current.do_next_sibling();
				எனில் (next.valid())
					திரும்பு next;
				current = current.do_parent();
			}
			திரும்பு current;
		}

		Cursor do_child(IdString name) {
			Id_மரம் *மரம் = nullptr;
			எனில் (scope_name.empty()) {
				மரம் = target;
			} இல்லையெனில் {
				auto found = target->subtrees.find(scope_name);
				எனில் (found != target->subtrees.end()) {
					மரம் = found->second.get();
				} இல்லையெனில் {
					திரும்பு Cursor();
				}
			}
			auto found = மரம்->names.find(name);
			எனில் (found == மரம்->names.end()) {
				திரும்பு Cursor();
			}
			திரும்பு Cursor(மரம், *found);
		}

	பொது:
		bool operator==(மாறிலி Cursor &other) மாறிலி {
			திரும்பு target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(மாறிலி Cursor &other) மாறிலி {
			திரும்பு !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி
		{
			h.eat(scope_name);
			h.eat(target);
			திரும்பு h;
		}

		bool valid() மாறிலி {
			திரும்பு target != nullptr;
		}

		முழு depth() மாறிலி {
			log_assert(valid());
			திரும்பு target->depth + !scope_name.empty();
		}

		bool is_வேர்() மாறிலி {
			திரும்பு target != nullptr && scope_name.empty();
		}

		bool has_entry() மாறிலி {
			log_assert(valid());
			திரும்பு !scope_name.empty() && target->entries.மொத்தம்(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			திரும்பு target->entries.at(scope_name);
		}

		வெறுமை assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			எனில் (scope_name.empty())
				திரும்பு;
			out_path.தள்ளு_back(scope_name);
			Id_மரம் *current = target;
			போது (current->parent) {
				out_path.தள்ளு_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			திரும்பு result;
		}

		std::string path_str(மாறிலி RTLIL::Design *design) {
			std::string result;
			க்கு (மாறிலி auto &item : path()) {
				எனில் (!result.empty())
					result.தள்ளு_back(' ');
				result += design->twines().unescaped_str(item);
			}
			திரும்பு result;
		}

		Cursor first_child() {
			log_assert(valid());
			திரும்பு do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			திரும்பு do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			திரும்பு do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			திரும்பு do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			போது (current != other) {
				எனில் (!current.valid() || !other.valid())
					திரும்பு Cursor();
				முழு delta = current.depth() - other.depth();
				எனில் (delta >= 0)
					current = current.do_parent();
				எனில் (delta <= 0)
					other = other.do_parent();
			}
			திரும்பு current;
		}
	};

	template<typename P>
	Cursor செருகு(P begin, P end, மாறிலி T &மதிப்பு) {
		திரும்பு do_செருகு(this, begin, end, மதிப்பு);
	}

	template<typename P>
	Cursor செருகு(P begin, P end, T &&மதிப்பு) {
		திரும்பு do_செருகு(this, begin, end, std::move(மதிப்பு));
	}

	template<typename P>
	Cursor செருகு(மாறிலி P &path, மாறிலி T &மதிப்பு) {
		திரும்பு do_செருகு(this, path.begin(), path.end(), மதிப்பு);
	}

	template<typename P>
	Cursor செருகு(மாறிலி P &path, T &&மதிப்பு) {
		திரும்பு do_செருகு(this, path.begin(), path.end(), std::move(மதிப்பு));
	}

	Cursor cursor() {
		திரும்பு parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		க்கு (; begin != end; ++begin) {
			current = current.do_child(*begin);
			எனில் (!current.valid())
				நிறுத்து;
		}
		திரும்பு current;
	}

	template<typename P>
	Cursor cursor(மாறிலி P &path) {
		திரும்பு cursor(path.begin(), path.end());
	}
};


கட்டமைப்பு ModuleItem {
	எண்ணிக்கை class Type {
		Wire,
		Cell,
	};
	Type type;
	வெறுமை *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() மாறிலி { திரும்பு type == Type::Wire; }
	bool is_cell() மாறிலி { திரும்பு type == Type::Cell; }

	Wire *wire() மாறிலி { திரும்பு type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() மாறிலி { திரும்பு type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(மாறிலி ModuleItem &other) மாறிலி { திரும்பு ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(ptr); திரும்பு h; }
};

நிலையான உள்வரி வெறுமை log_dump_val_worker(typename Id_மரம்<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
நிலையான உள்வரி வெறுமை log_dump_val_worker(மாறிலி typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(மாறிலி O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	க்கு (auto மாறிலி &item : object->get_hdlname_attribute())
		path.தள்ளு_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	எனில் (path.empty() && name.isPublic())
		path.தள்ளு_back(name);
	எனில் (!path.empty() && synthetic) {
		path.எடு_back();
		path.தள்ளு_back(name);
	}
	திரும்பு path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(மாறிலி O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	எனில் (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		க்கு (auto மாறிலி &item : object->get_hdlname_attribute())
			path.தள்ளு_back(twines.add("\\" + item));
		எனில் (!path.empty()) {
			trailing = path.back();
			path.எடு_back();
		}
	} இல்லையெனில் எனில் (object->has_attribute(ID::hdlname)) {
		க்கு (auto மாறிலி &item : object->get_hdlname_attribute())
			path.தள்ளு_back(twines.add("\\" + item));
		எனில் (!path.empty()) {
			path.எடு_back();
		}
	} இல்லையெனில் {
		க்கு (auto மாறிலி &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.தள்ளு_back(twines.add("\\" + item));
	}
	திரும்பு {path, trailing};
}

கட்டமைப்பு ModuleHdlnameIndex {
	வகைவரை Id_மரம்<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_மரம்<ModuleItem> மரம்;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

தனிப்பட்ட:
	template<typename I, typename Filter>
	வெறுமை index_பொருட்கள்(I begin, I end, Filter filter);

பொது:
	// Index all wires and cells of the module
	வெறுமை index();

	// Index all wires of the module
	வெறுமை index_wires();

	// Index all cells of the module
	வெறுமை index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	வெறுமை index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		திரும்பு {மரம்.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(மரம்.cursor(pair.first));
		result.தள்ளு_back(object->get_src_attribute());
		திரும்பு result;
	}
};

எண்ணிக்கை class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(மாறிலி RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
