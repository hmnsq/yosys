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

#अन्तर्भाव <vector>
#अन्तर्भाव <algorithm>

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename T>
class Id_रूख
{
सार्वजनिक:
	संरचना Cursor;

रक्षित:
	Id_रूख *parent = nullptr;
	IdString scope_name;
	पूर्णाङ्क depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
सार्वजनिक: // XXX
	dict<IdString, std::unique_ptr<Id_रूख>> subtrees;

	ढाँचा<typename P, typename T_ref>
	स्थिर Cursor do_घुसाउ(Id_रूख *रूख, P begin, P end, T_ref &&मान)
	{
		log_assert(begin != end && "path must be non-empty");
		जबसम्म (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			रूख->names.घुसाउ(name);
			यदि (begin == end) {
				रूख->entries.emplace(name, std::forward<T_ref>(मान));
				फर्काऊ Cursor(रूख, name);
			}
			स्वतः &unique = रूख->subtrees[name];
			यदि (!unique) {
				unique.reset(new Id_रूख);
				unique->scope_name = name;
				unique->parent = रूख;
				unique->depth = रूख->depth + 1;
			}
			रूख = unique.get();
		}
	}

सार्वजनिक:
	Id_रूख() = स्वभाव;
	Id_रूख(अचर Id_रूख &) = delete;
	Id_रूख(Id_रूख &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	संरचना Cursor
	{
		मित्र class Id_रूख;
	रक्षित:
	सार्वजनिक:
		Id_रूख *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_रूख *target, IdString scope_name) : target(target), scope_name(scope_name) {
			यदि (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_रूख *रूख = nullptr;
			यदि (scope_name.empty()) {
				रूख = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				यदि (found != target->subtrees.end()) {
					रूख = found->second.get();
				} अन्यथा {
					फर्काऊ Cursor();
				}
			}
			यदि (रूख->names.empty()) {
				फर्काऊ Cursor();
			}
			फर्काऊ Cursor(रूख, *रूख->names.begin());
		}

		Cursor do_next_sibling() {
			यदि (scope_name.empty())
				फर्काऊ Cursor();
			स्वतः found = target->names.find(scope_name);
			यदि (found == target->names.end())
				फर्काऊ Cursor();
			++found;
			यदि (found == target->names.end())
				फर्काऊ Cursor();
			फर्काऊ Cursor(target, *found);
		}

		Cursor do_parent() {
			यदि (scope_name.empty())
				फर्काऊ Cursor();
			यदि (target->parent != nullptr)
				फर्काऊ Cursor(target->parent, target->scope_name);
			फर्काऊ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			यदि (next.valid())
				फर्काऊ next;
			जबसम्म (current.valid()) {
				यदि (next.valid())
					फर्काऊ next;
				next = current.do_next_sibling();
				यदि (next.valid())
					फर्काऊ next;
				current = current.do_parent();
			}
			फर्काऊ current;
		}

		Cursor do_child(IdString name) {
			Id_रूख *रूख = nullptr;
			यदि (scope_name.empty()) {
				रूख = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				यदि (found != target->subtrees.end()) {
					रूख = found->second.get();
				} अन्यथा {
					फर्काऊ Cursor();
				}
			}
			स्वतः found = रूख->names.find(name);
			यदि (found == रूख->names.end()) {
				फर्काऊ Cursor();
			}
			फर्काऊ Cursor(रूख, *found);
		}

	सार्वजनिक:
		bool चालक==(अचर Cursor &other) अचर {
			फर्काऊ target == other.target && scope_name == other.scope_name;
		}
		bool चालक!=(अचर Cursor &other) अचर {
			फर्काऊ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) अचर
		{
			h.eat(scope_name);
			h.eat(target);
			फर्काऊ h;
		}

		bool valid() अचर {
			फर्काऊ target != nullptr;
		}

		पूर्णाङ्क depth() अचर {
			log_assert(valid());
			फर्काऊ target->depth + !scope_name.empty();
		}

		bool is_जरा() अचर {
			फर्काऊ target != nullptr && scope_name.empty();
		}

		bool has_entry() अचर {
			log_assert(valid());
			फर्काऊ !scope_name.empty() && target->entries.गन्ती(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			फर्काऊ target->entries.at(scope_name);
		}

		शून्य assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			यदि (scope_name.empty())
				फर्काऊ;
			out_path.थप_back(scope_name);
			Id_रूख *current = target;
			जबसम्म (current->parent) {
				out_path.थप_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			फर्काऊ result;
		}

		std::string path_str(अचर RTLIL::Design *design) {
			std::string result;
			लागि (अचर स्वतः &item : path()) {
				यदि (!result.empty())
					result.थप_back(' ');
				result += design->twines().unescaped_str(item);
			}
			फर्काऊ result;
		}

		Cursor first_child() {
			log_assert(valid());
			फर्काऊ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			फर्काऊ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			फर्काऊ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			फर्काऊ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			जबसम्म (current != other) {
				यदि (!current.valid() || !other.valid())
					फर्काऊ Cursor();
				पूर्णाङ्क delta = current.depth() - other.depth();
				यदि (delta >= 0)
					current = current.do_parent();
				यदि (delta <= 0)
					other = other.do_parent();
			}
			फर्काऊ current;
		}
	};

	ढाँचा<typename P>
	Cursor घुसाउ(P begin, P end, अचर T &मान) {
		फर्काऊ do_घुसाउ(this, begin, end, मान);
	}

	ढाँचा<typename P>
	Cursor घुसाउ(P begin, P end, T &&मान) {
		फर्काऊ do_घुसाउ(this, begin, end, std::move(मान));
	}

	ढाँचा<typename P>
	Cursor घुसाउ(अचर P &path, अचर T &मान) {
		फर्काऊ do_घुसाउ(this, path.begin(), path.end(), मान);
	}

	ढाँचा<typename P>
	Cursor घुसाउ(अचर P &path, T &&मान) {
		फर्काऊ do_घुसाउ(this, path.begin(), path.end(), std::move(मान));
	}

	Cursor cursor() {
		फर्काऊ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	ढाँचा<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		लागि (; begin != end; ++begin) {
			current = current.do_child(*begin);
			यदि (!current.valid())
				रोक;
		}
		फर्काऊ current;
	}

	ढाँचा<typename P>
	Cursor cursor(अचर P &path) {
		फर्काऊ cursor(path.begin(), path.end());
	}
};


संरचना ModuleItem {
	गणना class Type {
		Wire,
		Cell,
	};
	Type type;
	शून्य *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() अचर { फर्काऊ type == Type::Wire; }
	bool is_cell() अचर { फर्काऊ type == Type::Cell; }

	Wire *wire() अचर { फर्काऊ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() अचर { फर्काऊ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool चालक==(अचर ModuleItem &other) अचर { फर्काऊ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(ptr); फर्काऊ h; }
};

स्थिर अन्तःस्थ शून्य log_dump_val_worker(typename Id_रूख<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

ढाँचा<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(अचर typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

ढाँचा<typename O>
std::vector<IdString> parse_hdlname(अचर O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	लागि (स्वतः अचर &item : object->get_hdlname_attribute())
		path.थप_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	यदि (path.empty() && name.isPublic())
		path.थप_back(name);
	यदि (!path.empty() && synthetic) {
		path.झिक_back();
		path.थप_back(name);
	}
	फर्काऊ path;
}

ढाँचा<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(अचर O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	यदि (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		लागि (स्वतः अचर &item : object->get_hdlname_attribute())
			path.थप_back(twines.add("\\" + item));
		यदि (!path.empty()) {
			trailing = path.back();
			path.झिक_back();
		}
	} अन्यथा यदि (object->has_attribute(ID::hdlname)) {
		लागि (स्वतः अचर &item : object->get_hdlname_attribute())
			path.थप_back(twines.add("\\" + item));
		यदि (!path.empty()) {
			path.झिक_back();
		}
	} अन्यथा {
		लागि (स्वतः अचर &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.थप_back(twines.add("\\" + item));
	}
	फर्काऊ {path, trailing};
}

संरचना ModuleHdlnameIndex {
	प्रकारनिर्देश Id_रूख<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_रूख<ModuleItem> रूख;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

गुप्त:
	ढाँचा<typename I, typename Filter>
	शून्य index_वस्तुहरू(I begin, I end, Filter filter);

सार्वजनिक:
	// Index all wires and cells of the module
	शून्य index();

	// Index all wires of the module
	शून्य index_wires();

	// Index all cells of the module
	शून्य index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	शून्य index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	ढाँचा<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		स्वतः pair = parse_scopename(object);
		फर्काऊ {रूख.cursor(pair.first), pair.second};
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
	ढाँचा<typename O>
	std::vector<std::string> sources(O *object) {
		स्वतः pair = parse_scopename(object);
		std::vector<std::string> result = scope_sources(रूख.cursor(pair.first));
		result.थप_back(object->get_src_attribute());
		फर्काऊ result;
	}
};

गणना class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(अचर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
