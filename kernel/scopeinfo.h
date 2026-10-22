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
class Id_वृक्ष
{
सार्वजनिक:
	संरचना Cursor;

संरक्षित:
	Id_वृक्ष *parent = nullptr;
	IdString scope_name;
	पूर्णांक depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
सार्वजनिक: // XXX
	dict<IdString, std::unique_ptr<Id_वृक्ष>> subtrees;

	ढाँचा<typename P, typename T_ref>
	स्थिर Cursor do_घाला(Id_वृक्ष *वृक्ष, P begin, P end, T_ref &&मूल्य)
	{
		log_assert(begin != end && "path must be non-empty");
		पर्यंत (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			वृक्ष->names.घाला(name);
			जर (begin == end) {
				वृक्ष->entries.emplace(name, std::forward<T_ref>(मूल्य));
				परत Cursor(वृक्ष, name);
			}
			स्वतः &unique = वृक्ष->subtrees[name];
			जर (!unique) {
				unique.reset(new Id_वृक्ष);
				unique->scope_name = name;
				unique->parent = वृक्ष;
				unique->depth = वृक्ष->depth + 1;
			}
			वृक्ष = unique.get();
		}
	}

सार्वजनिक:
	Id_वृक्ष() = पूर्वनिर्धारित;
	Id_वृक्ष(स्थिरांक Id_वृक्ष &) = delete;
	Id_वृक्ष(Id_वृक्ष &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	संरचना Cursor
	{
		मित्र class Id_वृक्ष;
	संरक्षित:
	सार्वजनिक:
		Id_वृक्ष *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_वृक्ष *target, IdString scope_name) : target(target), scope_name(scope_name) {
			जर (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_वृक्ष *वृक्ष = nullptr;
			जर (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				जर (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					परत Cursor();
				}
			}
			जर (वृक्ष->names.empty()) {
				परत Cursor();
			}
			परत Cursor(वृक्ष, *वृक्ष->names.begin());
		}

		Cursor do_next_sibling() {
			जर (scope_name.empty())
				परत Cursor();
			स्वतः found = target->names.find(scope_name);
			जर (found == target->names.end())
				परत Cursor();
			++found;
			जर (found == target->names.end())
				परत Cursor();
			परत Cursor(target, *found);
		}

		Cursor do_parent() {
			जर (scope_name.empty())
				परत Cursor();
			जर (target->parent != nullptr)
				परत Cursor(target->parent, target->scope_name);
			परत Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			जर (next.valid())
				परत next;
			पर्यंत (current.valid()) {
				जर (next.valid())
					परत next;
				next = current.do_next_sibling();
				जर (next.valid())
					परत next;
				current = current.do_parent();
			}
			परत current;
		}

		Cursor do_child(IdString name) {
			Id_वृक्ष *वृक्ष = nullptr;
			जर (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				जर (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					परत Cursor();
				}
			}
			स्वतः found = वृक्ष->names.find(name);
			जर (found == वृक्ष->names.end()) {
				परत Cursor();
			}
			परत Cursor(वृक्ष, *found);
		}

	सार्वजनिक:
		bool चालक==(स्थिरांक Cursor &other) स्थिरांक {
			परत target == other.target && scope_name == other.scope_name;
		}
		bool चालक!=(स्थिरांक Cursor &other) स्थिरांक {
			परत !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक
		{
			h.eat(scope_name);
			h.eat(target);
			परत h;
		}

		bool valid() स्थिरांक {
			परत target != nullptr;
		}

		पूर्णांक depth() स्थिरांक {
			log_assert(valid());
			परत target->depth + !scope_name.empty();
		}

		bool is_मूळ() स्थिरांक {
			परत target != nullptr && scope_name.empty();
		}

		bool has_entry() स्थिरांक {
			log_assert(valid());
			परत !scope_name.empty() && target->entries.मोजणी(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			परत target->entries.at(scope_name);
		}

		शून्य assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			जर (scope_name.empty())
				परत;
			out_path.ढकला_back(scope_name);
			Id_वृक्ष *current = target;
			पर्यंत (current->parent) {
				out_path.ढकला_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			परत result;
		}

		std::string path_str(स्थिरांक RTLIL::Design *design) {
			std::string result;
			साठी (स्थिरांक स्वतः &item : path()) {
				जर (!result.empty())
					result.ढकला_back(' ');
				result += design->twines().unescaped_str(item);
			}
			परत result;
		}

		Cursor first_child() {
			log_assert(valid());
			परत do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			परत do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			परत do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			परत do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			पर्यंत (current != other) {
				जर (!current.valid() || !other.valid())
					परत Cursor();
				पूर्णांक delta = current.depth() - other.depth();
				जर (delta >= 0)
					current = current.do_parent();
				जर (delta <= 0)
					other = other.do_parent();
			}
			परत current;
		}
	};

	ढाँचा<typename P>
	Cursor घाला(P begin, P end, स्थिरांक T &मूल्य) {
		परत do_घाला(this, begin, end, मूल्य);
	}

	ढाँचा<typename P>
	Cursor घाला(P begin, P end, T &&मूल्य) {
		परत do_घाला(this, begin, end, std::move(मूल्य));
	}

	ढाँचा<typename P>
	Cursor घाला(स्थिरांक P &path, स्थिरांक T &मूल्य) {
		परत do_घाला(this, path.begin(), path.end(), मूल्य);
	}

	ढाँचा<typename P>
	Cursor घाला(स्थिरांक P &path, T &&मूल्य) {
		परत do_घाला(this, path.begin(), path.end(), std::move(मूल्य));
	}

	Cursor cursor() {
		परत parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	ढाँचा<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		साठी (; begin != end; ++begin) {
			current = current.do_child(*begin);
			जर (!current.valid())
				थांबा;
		}
		परत current;
	}

	ढाँचा<typename P>
	Cursor cursor(स्थिरांक P &path) {
		परत cursor(path.begin(), path.end());
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

	bool is_wire() स्थिरांक { परत type == Type::Wire; }
	bool is_cell() स्थिरांक { परत type == Type::Cell; }

	Wire *wire() स्थिरांक { परत type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() स्थिरांक { परत type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool चालक==(स्थिरांक ModuleItem &other) स्थिरांक { परत ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(ptr); परत h; }
};

स्थिर ओळीत शून्य log_dump_val_worker(typename Id_वृक्ष<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

ढाँचा<typename T>
स्थिर ओळीत शून्य log_dump_val_worker(स्थिरांक typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

ढाँचा<typename O>
std::vector<IdString> parse_hdlname(स्थिरांक O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	साठी (स्वतः स्थिरांक &item : object->get_hdlname_attribute())
		path.ढकला_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	जर (path.empty() && name.isPublic())
		path.ढकला_back(name);
	जर (!path.empty() && synthetic) {
		path.काढा_back();
		path.ढकला_back(name);
	}
	परत path;
}

ढाँचा<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(स्थिरांक O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	जर (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		साठी (स्वतः स्थिरांक &item : object->get_hdlname_attribute())
			path.ढकला_back(twines.add("\\" + item));
		जर (!path.empty()) {
			trailing = path.back();
			path.काढा_back();
		}
	} अन्यथा जर (object->has_attribute(ID::hdlname)) {
		साठी (स्वतः स्थिरांक &item : object->get_hdlname_attribute())
			path.ढकला_back(twines.add("\\" + item));
		जर (!path.empty()) {
			path.काढा_back();
		}
	} अन्यथा {
		साठी (स्वतः स्थिरांक &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ढकला_back(twines.add("\\" + item));
	}
	परत {path, trailing};
}

संरचना ModuleHdlnameIndex {
	प्रकारव्याख्या Id_वृक्ष<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_वृक्ष<ModuleItem> वृक्ष;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

खाजगी:
	ढाँचा<typename I, typename Filter>
	शून्य index_वस्तुसंच(I begin, I end, Filter filter);

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
		परत {वृक्ष.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(वृक्ष.cursor(pair.first));
		result.ढकला_back(object->get_src_attribute());
		परत result;
	}
};

गणना class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(स्थिरांक RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
