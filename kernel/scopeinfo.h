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

#અન્તર્ભાવ <vector>
#અન્તર્ભાવ <algorithm>

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_વૃક્ષ
{
જાહેર:
	સંરચના Cursor;

સુરક્ષિત:
	Id_વૃક્ષ *parent = nullptr;
	IdString scope_name;
	પૂર્ણાંક depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
જાહેર: // XXX
	dict<IdString, std::unique_ptr<Id_વૃક્ષ>> subtrees;

	template<typename P, typename T_ref>
	સ્થિર Cursor do_ઉમેરો(Id_વૃક્ષ *વૃક્ષ, P begin, P end, T_ref &&કિંમત)
	{
		log_assert(begin != end && "path must be non-empty");
		જ્યારે (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			વૃક્ષ->names.ઉમેરો(name);
			જો (begin == end) {
				વૃક્ષ->entries.emplace(name, std::forward<T_ref>(કિંમત));
				પાછા Cursor(વૃક્ષ, name);
			}
			auto &unique = વૃક્ષ->subtrees[name];
			જો (!unique) {
				unique.reset(new Id_વૃક્ષ);
				unique->scope_name = name;
				unique->parent = વૃક્ષ;
				unique->depth = વૃક્ષ->depth + 1;
			}
			વૃક્ષ = unique.get();
		}
	}

જાહેર:
	Id_વૃક્ષ() = મૂળભૂત;
	Id_વૃક્ષ(અચલ Id_વૃક્ષ &) = delete;
	Id_વૃક્ષ(Id_વૃક્ષ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	સંરચના Cursor
	{
		friend class Id_વૃક્ષ;
	સુરક્ષિત:
	જાહેર:
		Id_વૃક્ષ *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_વૃક્ષ *target, IdString scope_name) : target(target), scope_name(scope_name) {
			જો (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_વૃક્ષ *વૃક્ષ = nullptr;
			જો (scope_name.empty()) {
				વૃક્ષ = target;
			} નહીંતર {
				auto found = target->subtrees.find(scope_name);
				જો (found != target->subtrees.end()) {
					વૃક્ષ = found->second.get();
				} નહીંતર {
					પાછા Cursor();
				}
			}
			જો (વૃક્ષ->names.empty()) {
				પાછા Cursor();
			}
			પાછા Cursor(વૃક્ષ, *વૃક્ષ->names.begin());
		}

		Cursor do_next_sibling() {
			જો (scope_name.empty())
				પાછા Cursor();
			auto found = target->names.find(scope_name);
			જો (found == target->names.end())
				પાછા Cursor();
			++found;
			જો (found == target->names.end())
				પાછા Cursor();
			પાછા Cursor(target, *found);
		}

		Cursor do_parent() {
			જો (scope_name.empty())
				પાછા Cursor();
			જો (target->parent != nullptr)
				પાછા Cursor(target->parent, target->scope_name);
			પાછા Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			જો (next.valid())
				પાછા next;
			જ્યારે (current.valid()) {
				જો (next.valid())
					પાછા next;
				next = current.do_next_sibling();
				જો (next.valid())
					પાછા next;
				current = current.do_parent();
			}
			પાછા current;
		}

		Cursor do_child(IdString name) {
			Id_વૃક્ષ *વૃક્ષ = nullptr;
			જો (scope_name.empty()) {
				વૃક્ષ = target;
			} નહીંતર {
				auto found = target->subtrees.find(scope_name);
				જો (found != target->subtrees.end()) {
					વૃક્ષ = found->second.get();
				} નહીંતર {
					પાછા Cursor();
				}
			}
			auto found = વૃક્ષ->names.find(name);
			જો (found == વૃક્ષ->names.end()) {
				પાછા Cursor();
			}
			પાછા Cursor(વૃક્ષ, *found);
		}

	જાહેર:
		bool operator==(અચલ Cursor &other) અચલ {
			પાછા target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(અચલ Cursor &other) અચલ {
			પાછા !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) અચલ
		{
			h.eat(scope_name);
			h.eat(target);
			પાછા h;
		}

		bool valid() અચલ {
			પાછા target != nullptr;
		}

		પૂર્ણાંક depth() અચલ {
			log_assert(valid());
			પાછા target->depth + !scope_name.empty();
		}

		bool is_મૂળ() અચલ {
			પાછા target != nullptr && scope_name.empty();
		}

		bool has_entry() અચલ {
			log_assert(valid());
			પાછા !scope_name.empty() && target->entries.ગણતરી(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			પાછા target->entries.at(scope_name);
		}

		શૂન્ય assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			જો (scope_name.empty())
				પાછા;
			out_path.ધકેલો_back(scope_name);
			Id_વૃક્ષ *current = target;
			જ્યારે (current->parent) {
				out_path.ધકેલો_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			પાછા result;
		}

		std::string path_str(અચલ RTLIL::Design *design) {
			std::string result;
			માટે (અચલ auto &item : path()) {
				જો (!result.empty())
					result.ધકેલો_back(' ');
				result += design->twines().unescaped_str(item);
			}
			પાછા result;
		}

		Cursor first_child() {
			log_assert(valid());
			પાછા do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			પાછા do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			પાછા do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			પાછા do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			જ્યારે (current != other) {
				જો (!current.valid() || !other.valid())
					પાછા Cursor();
				પૂર્ણાંક delta = current.depth() - other.depth();
				જો (delta >= 0)
					current = current.do_parent();
				જો (delta <= 0)
					other = other.do_parent();
			}
			પાછા current;
		}
	};

	template<typename P>
	Cursor ઉમેરો(P begin, P end, અચલ T &કિંમત) {
		પાછા do_ઉમેરો(this, begin, end, કિંમત);
	}

	template<typename P>
	Cursor ઉમેરો(P begin, P end, T &&કિંમત) {
		પાછા do_ઉમેરો(this, begin, end, std::move(કિંમત));
	}

	template<typename P>
	Cursor ઉમેરો(અચલ P &path, અચલ T &કિંમત) {
		પાછા do_ઉમેરો(this, path.begin(), path.end(), કિંમત);
	}

	template<typename P>
	Cursor ઉમેરો(અચલ P &path, T &&કિંમત) {
		પાછા do_ઉમેરો(this, path.begin(), path.end(), std::move(કિંમત));
	}

	Cursor cursor() {
		પાછા parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		માટે (; begin != end; ++begin) {
			current = current.do_child(*begin);
			જો (!current.valid())
				તોડો;
		}
		પાછા current;
	}

	template<typename P>
	Cursor cursor(અચલ P &path) {
		પાછા cursor(path.begin(), path.end());
	}
};


સંરચના ModuleItem {
	ગણના class Type {
		Wire,
		Cell,
	};
	Type type;
	શૂન્ય *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() અચલ { પાછા type == Type::Wire; }
	bool is_cell() અચલ { પાછા type == Type::Cell; }

	Wire *wire() અચલ { પાછા type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() અચલ { પાછા type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(અચલ ModuleItem &other) અચલ { પાછા ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(ptr); પાછા h; }
};

સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(typename Id_વૃક્ષ<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(અચલ typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(અચલ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	માટે (auto અચલ &item : object->get_hdlname_attribute())
		path.ધકેલો_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	જો (path.empty() && name.isPublic())
		path.ધકેલો_back(name);
	જો (!path.empty() && synthetic) {
		path.કાઢો_back();
		path.ધકેલો_back(name);
	}
	પાછા path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(અચલ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	જો (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		માટે (auto અચલ &item : object->get_hdlname_attribute())
			path.ધકેલો_back(twines.add("\\" + item));
		જો (!path.empty()) {
			trailing = path.back();
			path.કાઢો_back();
		}
	} નહીંતર જો (object->has_attribute(ID::hdlname)) {
		માટે (auto અચલ &item : object->get_hdlname_attribute())
			path.ધકેલો_back(twines.add("\\" + item));
		જો (!path.empty()) {
			path.કાઢો_back();
		}
	} નહીંતર {
		માટે (auto અચલ &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ધકેલો_back(twines.add("\\" + item));
	}
	પાછા {path, trailing};
}

સંરચના ModuleHdlnameIndex {
	પ્રકારવ્યાખ્યા Id_વૃક્ષ<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_વૃક્ષ<ModuleItem> વૃક્ષ;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

ખાનગી:
	template<typename I, typename Filter>
	શૂન્ય index_વસ્તુઓ(I begin, I end, Filter filter);

જાહેર:
	// Index all wires and cells of the module
	શૂન્ય index();

	// Index all wires of the module
	શૂન્ય index_wires();

	// Index all cells of the module
	શૂન્ય index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	શૂન્ય index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		પાછા {વૃક્ષ.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(વૃક્ષ.cursor(pair.first));
		result.ધકેલો_back(object->get_src_attribute());
		પાછા result;
	}
};

ગણના class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(અચલ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
