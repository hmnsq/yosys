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

#अगर_अघोषित SCOPEINFO_H
#घोषणा SCOPEINFO_H

#समावेश <vector>
#समावेश <algorithm>

#समावेश "kernel/yosys.h"
#समावेश "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename T>
class Id_वृक्ष
{
खुला:
	काष्ठा Cursor;

रक्षित:
	Id_वृक्ष *parent = nullptr;
	IdString scope_name;
	पूर्णांक depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
खुला: // XXX
	dict<IdString, std::unique_ptr<Id_वृक्ष>> subtrees;

	ढाँचा<typename P, typename T_ref>
	अटल Cursor do_जोड़ो(Id_वृक्ष *वृक्ष, P begin, P end, T_ref &&मूल्य)
	{
		log_assert(begin != end && "path must be non-empty");
		जबतक (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			वृक्ष->names.जोड़ो(name);
			अगर (begin == end) {
				वृक्ष->entries.emplace(name, std::forward<T_ref>(मूल्य));
				वापस Cursor(वृक्ष, name);
			}
			स्वतः &unique = वृक्ष->subtrees[name];
			अगर (!unique) {
				unique.reset(new Id_वृक्ष);
				unique->scope_name = name;
				unique->parent = वृक्ष;
				unique->depth = वृक्ष->depth + 1;
			}
			वृक्ष = unique.get();
		}
	}

खुला:
	Id_वृक्ष() = शेष;
	Id_वृक्ष(स्थिर Id_वृक्ष &) = delete;
	Id_वृक्ष(Id_वृक्ष &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	काष्ठा Cursor
	{
		मित्र class Id_वृक्ष;
	रक्षित:
	खुला:
		Id_वृक्ष *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_वृक्ष *target, IdString scope_name) : target(target), scope_name(scope_name) {
			अगर (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_वृक्ष *वृक्ष = nullptr;
			अगर (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				अगर (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					वापस Cursor();
				}
			}
			अगर (वृक्ष->names.empty()) {
				वापस Cursor();
			}
			वापस Cursor(वृक्ष, *वृक्ष->names.begin());
		}

		Cursor do_अग्र_sibling() {
			अगर (scope_name.empty())
				वापस Cursor();
			स्वतः found = target->names.find(scope_name);
			अगर (found == target->names.end())
				वापस Cursor();
			++found;
			अगर (found == target->names.end())
				वापस Cursor();
			वापस Cursor(target, *found);
		}

		Cursor do_parent() {
			अगर (scope_name.empty())
				वापस Cursor();
			अगर (target->parent != nullptr)
				वापस Cursor(target->parent, target->scope_name);
			वापस Cursor(target, IdString());
		}

		Cursor do_अग्र_preorder() {
			Cursor current = *this;
			Cursor अग्र = current.do_first_child();
			अगर (अग्र.valid())
				वापस अग्र;
			जबतक (current.valid()) {
				अगर (अग्र.valid())
					वापस अग्र;
				अग्र = current.do_अग्र_sibling();
				अगर (अग्र.valid())
					वापस अग्र;
				current = current.do_parent();
			}
			वापस current;
		}

		Cursor do_child(IdString name) {
			Id_वृक्ष *वृक्ष = nullptr;
			अगर (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				अगर (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					वापस Cursor();
				}
			}
			स्वतः found = वृक्ष->names.find(name);
			अगर (found == वृक्ष->names.end()) {
				वापस Cursor();
			}
			वापस Cursor(वृक्ष, *found);
		}

	खुला:
		bool चालक==(स्थिर Cursor &other) स्थिर {
			वापस target == other.target && scope_name == other.scope_name;
		}
		bool चालक!=(स्थिर Cursor &other) स्थिर {
			वापस !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर
		{
			h.eat(scope_name);
			h.eat(target);
			वापस h;
		}

		bool valid() स्थिर {
			वापस target != nullptr;
		}

		पूर्णांक depth() स्थिर {
			log_assert(valid());
			वापस target->depth + !scope_name.empty();
		}

		bool is_मूल() स्थिर {
			वापस target != nullptr && scope_name.empty();
		}

		bool has_entry() स्थिर {
			log_assert(valid());
			वापस !scope_name.empty() && target->entries.गिनती(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			वापस target->entries.at(scope_name);
		}

		व्योम assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			अगर (scope_name.empty())
				वापस;
			out_path.धकेलो_back(scope_name);
			Id_वृक्ष *current = target;
			जबतक (current->parent) {
				out_path.धकेलो_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> परिणाम;
			assign_path_to(परिणाम);
			वापस परिणाम;
		}

		std::string path_str(स्थिर RTLIL::Design *design) {
			std::string परिणाम;
			क्रम (स्थिर स्वतः &item : path()) {
				अगर (!परिणाम.empty())
					परिणाम.धकेलो_back(' ');
				परिणाम += design->twines().unescaped_str(item);
			}
			वापस परिणाम;
		}

		Cursor first_child() {
			log_assert(valid());
			वापस do_first_child();
		}

		Cursor अग्र_preorder() {
			log_assert(valid());
			वापस do_अग्र_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			वापस do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			वापस do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			जबतक (current != other) {
				अगर (!current.valid() || !other.valid())
					वापस Cursor();
				पूर्णांक delta = current.depth() - other.depth();
				अगर (delta >= 0)
					current = current.do_parent();
				अगर (delta <= 0)
					other = other.do_parent();
			}
			वापस current;
		}
	};

	ढाँचा<typename P>
	Cursor जोड़ो(P begin, P end, स्थिर T &मूल्य) {
		वापस do_जोड़ो(this, begin, end, मूल्य);
	}

	ढाँचा<typename P>
	Cursor जोड़ो(P begin, P end, T &&मूल्य) {
		वापस do_जोड़ो(this, begin, end, std::move(मूल्य));
	}

	ढाँचा<typename P>
	Cursor जोड़ो(स्थिर P &path, स्थिर T &मूल्य) {
		वापस do_जोड़ो(this, path.begin(), path.end(), मूल्य);
	}

	ढाँचा<typename P>
	Cursor जोड़ो(स्थिर P &path, T &&मूल्य) {
		वापस do_जोड़ो(this, path.begin(), path.end(), std::move(मूल्य));
	}

	Cursor cursor() {
		वापस parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	ढाँचा<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		क्रम (; begin != end; ++begin) {
			current = current.do_child(*begin);
			अगर (!current.valid())
				अवरोध;
		}
		वापस current;
	}

	ढाँचा<typename P>
	Cursor cursor(स्थिर P &path) {
		वापस cursor(path.begin(), path.end());
	}
};


काष्ठा ModuleItem {
	क्रमागत class Type {
		Wire,
		Cell,
	};
	Type type;
	व्योम *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() स्थिर { वापस type == Type::Wire; }
	bool is_cell() स्थिर { वापस type == Type::Cell; }

	Wire *wire() स्थिर { वापस type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() स्थिर { वापस type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool चालक==(स्थिर ModuleItem &other) स्थिर { वापस ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(ptr); वापस h; }
};

अटल अंतरभूत व्योम log_dump_val_worker(typename Id_वृक्ष<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

ढाँचा<typename T>
अटल अंतरभूत व्योम log_dump_val_worker(स्थिर typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

ढाँचा<typename O>
std::vector<IdString> parse_hdlname(स्थिर O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	क्रम (स्वतः स्थिर &item : object->get_hdlname_attribute())
		path.धकेलो_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	अगर (path.empty() && name.isPublic())
		path.धकेलो_back(name);
	अगर (!path.empty() && synthetic) {
		path.निकालो_back();
		path.धकेलो_back(name);
	}
	वापस path;
}

ढाँचा<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(स्थिर O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	अगर (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		क्रम (स्वतः स्थिर &item : object->get_hdlname_attribute())
			path.धकेलो_back(twines.add("\\" + item));
		अगर (!path.empty()) {
			trailing = path.back();
			path.निकालो_back();
		}
	} अन्यथा अगर (object->has_attribute(ID::hdlname)) {
		क्रम (स्वतः स्थिर &item : object->get_hdlname_attribute())
			path.धकेलो_back(twines.add("\\" + item));
		अगर (!path.empty()) {
			path.निकालो_back();
		}
	} अन्यथा {
		क्रम (स्वतः स्थिर &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.धकेलो_back(twines.add("\\" + item));
	}
	वापस {path, trailing};
}

काष्ठा Module_Hdlname_सूचकांक {
	प्रकार Id_वृक्ष<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_वृक्ष<ModuleItem> वृक्ष;
	dict<ModuleItem, Cursor> lookup;

	Module_Hdlname_सूचकांक(RTLIL::Module *module) : module(module) {}

निजी:
	ढाँचा<typename I, typename Filter>
	व्योम सूचकांक_वस्तुएँ(I begin, I end, Filter filter);

खुला:
	// Index all wires and cells of the module
	व्योम सूचकांक();

	// Index all wires of the module
	व्योम सूचकांक_wires();

	// Index all cells of the module
	व्योम सूचकांक_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	व्योम सूचकांक_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	ढाँचा<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		स्वतः pair = parse_scopename(object);
		वापस {वृक्ष.cursor(pair.first), pair.second};
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
		std::vector<std::string> परिणाम = scope_sources(वृक्ष.cursor(pair.first));
		परिणाम.धकेलो_back(object->get_src_attribute());
		वापस परिणाम;
	}
};

क्रमागत class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(स्थिर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(स्थिर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(स्थिर RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#पूर्ण_अगर
