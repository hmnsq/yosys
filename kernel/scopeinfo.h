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

रक्षित:
	Id_वृक्ष *parent = nullptr;
	IdString scope_name;
	पूर्णाङ्क depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
सार्वजनिक: // XXX
	dict<IdString, std::unique_ptr<Id_वृक्ष>> subtrees;

	ढाँचा<typename P, typename T_ref>
	स्थिर Cursor do_जोड़ो(Id_वृक्ष *वृक्ष, P begin, P end, T_ref &&मूल्य)
	{
		log_assert(begin != end && "path must be non-empty");
		यावत् (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			वृक्ष->names.जोड़ो(name);
			यदि (begin == end) {
				वृक्ष->entries.emplace(name, std::forward<T_ref>(मूल्य));
				प्रत्यागम Cursor(वृक्ष, name);
			}
			स्वतः &unique = वृक्ष->subtrees[name];
			यदि (!unique) {
				unique.reset(new Id_वृक्ष);
				unique->scope_name = name;
				unique->parent = वृक्ष;
				unique->depth = वृक्ष->depth + 1;
			}
			वृक्ष = unique.get();
		}
	}

सार्वजनिक:
	Id_वृक्ष() = स्वभाव;
	Id_वृक्ष(ध्रुव Id_वृक्ष &) = delete;
	Id_वृक्ष(Id_वृक्ष &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	संरचना Cursor
	{
		मित्र class Id_वृक्ष;
	रक्षित:
	सार्वजनिक:
		Id_वृक्ष *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_वृक्ष *target, IdString scope_name) : target(target), scope_name(scope_name) {
			यदि (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_वृक्ष *वृक्ष = nullptr;
			यदि (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				यदि (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					प्रत्यागम Cursor();
				}
			}
			यदि (वृक्ष->names.empty()) {
				प्रत्यागम Cursor();
			}
			प्रत्यागम Cursor(वृक्ष, *वृक्ष->names.begin());
		}

		Cursor do_next_sibling() {
			यदि (scope_name.empty())
				प्रत्यागम Cursor();
			स्वतः found = target->names.find(scope_name);
			यदि (found == target->names.end())
				प्रत्यागम Cursor();
			++found;
			यदि (found == target->names.end())
				प्रत्यागम Cursor();
			प्रत्यागम Cursor(target, *found);
		}

		Cursor do_parent() {
			यदि (scope_name.empty())
				प्रत्यागम Cursor();
			यदि (target->parent != nullptr)
				प्रत्यागम Cursor(target->parent, target->scope_name);
			प्रत्यागम Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			यदि (next.valid())
				प्रत्यागम next;
			यावत् (current.valid()) {
				यदि (next.valid())
					प्रत्यागम next;
				next = current.do_next_sibling();
				यदि (next.valid())
					प्रत्यागम next;
				current = current.do_parent();
			}
			प्रत्यागम current;
		}

		Cursor do_child(IdString name) {
			Id_वृक्ष *वृक्ष = nullptr;
			यदि (scope_name.empty()) {
				वृक्ष = target;
			} अन्यथा {
				स्वतः found = target->subtrees.find(scope_name);
				यदि (found != target->subtrees.end()) {
					वृक्ष = found->second.get();
				} अन्यथा {
					प्रत्यागम Cursor();
				}
			}
			स्वतः found = वृक्ष->names.find(name);
			यदि (found == वृक्ष->names.end()) {
				प्रत्यागम Cursor();
			}
			प्रत्यागम Cursor(वृक्ष, *found);
		}

	सार्वजनिक:
		bool चालक==(ध्रुव Cursor &other) ध्रुव {
			प्रत्यागम target == other.target && scope_name == other.scope_name;
		}
		bool चालक!=(ध्रुव Cursor &other) ध्रुव {
			प्रत्यागम !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव
		{
			h.eat(scope_name);
			h.eat(target);
			प्रत्यागम h;
		}

		bool valid() ध्रुव {
			प्रत्यागम target != nullptr;
		}

		पूर्णाङ्क depth() ध्रुव {
			log_assert(valid());
			प्रत्यागम target->depth + !scope_name.empty();
		}

		bool is_मूल() ध्रुव {
			प्रत्यागम target != nullptr && scope_name.empty();
		}

		bool has_entry() ध्रुव {
			log_assert(valid());
			प्रत्यागम !scope_name.empty() && target->entries.गिनती(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			प्रत्यागम target->entries.at(scope_name);
		}

		शून्य assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			यदि (scope_name.empty())
				प्रत्यागम;
			out_path.धकेलो_back(scope_name);
			Id_वृक्ष *current = target;
			यावत् (current->parent) {
				out_path.धकेलो_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			प्रत्यागम result;
		}

		std::string path_str(ध्रुव RTLIL::Design *design) {
			std::string result;
			प्रति (ध्रुव स्वतः &item : path()) {
				यदि (!result.empty())
					result.धकेलो_back(' ');
				result += design->twines().unescaped_str(item);
			}
			प्रत्यागम result;
		}

		Cursor first_child() {
			log_assert(valid());
			प्रत्यागम do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			प्रत्यागम do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			प्रत्यागम do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			प्रत्यागम do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			यावत् (current != other) {
				यदि (!current.valid() || !other.valid())
					प्रत्यागम Cursor();
				पूर्णाङ्क delta = current.depth() - other.depth();
				यदि (delta >= 0)
					current = current.do_parent();
				यदि (delta <= 0)
					other = other.do_parent();
			}
			प्रत्यागम current;
		}
	};

	ढाँचा<typename P>
	Cursor जोड़ो(P begin, P end, ध्रुव T &मूल्य) {
		प्रत्यागम do_जोड़ो(this, begin, end, मूल्य);
	}

	ढाँचा<typename P>
	Cursor जोड़ो(P begin, P end, T &&मूल्य) {
		प्रत्यागम do_जोड़ो(this, begin, end, std::move(मूल्य));
	}

	ढाँचा<typename P>
	Cursor जोड़ो(ध्रुव P &path, ध्रुव T &मूल्य) {
		प्रत्यागम do_जोड़ो(this, path.begin(), path.end(), मूल्य);
	}

	ढाँचा<typename P>
	Cursor जोड़ो(ध्रुव P &path, T &&मूल्य) {
		प्रत्यागम do_जोड़ो(this, path.begin(), path.end(), std::move(मूल्य));
	}

	Cursor cursor() {
		प्रत्यागम parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	ढाँचा<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		प्रति (; begin != end; ++begin) {
			current = current.do_child(*begin);
			यदि (!current.valid())
				भङ्ग;
		}
		प्रत्यागम current;
	}

	ढाँचा<typename P>
	Cursor cursor(ध्रुव P &path) {
		प्रत्यागम cursor(path.begin(), path.end());
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

	bool is_wire() ध्रुव { प्रत्यागम type == Type::Wire; }
	bool is_cell() ध्रुव { प्रत्यागम type == Type::Cell; }

	Wire *wire() ध्रुव { प्रत्यागम type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ध्रुव { प्रत्यागम type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool चालक==(ध्रुव ModuleItem &other) ध्रुव { प्रत्यागम ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(ptr); प्रत्यागम h; }
};

स्थिर अन्तःस्थ शून्य log_dump_val_worker(typename Id_वृक्ष<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

ढाँचा<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(ध्रुव typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

ढाँचा<typename O>
std::vector<IdString> parse_hdlname(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	प्रति (स्वतः ध्रुव &item : object->get_hdlname_attribute())
		path.धकेलो_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	यदि (path.empty() && name.isPublic())
		path.धकेलो_back(name);
	यदि (!path.empty() && synthetic) {
		path.निकालो_back();
		path.धकेलो_back(name);
	}
	प्रत्यागम path;
}

ढाँचा<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	यदि (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		प्रति (स्वतः ध्रुव &item : object->get_hdlname_attribute())
			path.धकेलो_back(twines.add("\\" + item));
		यदि (!path.empty()) {
			trailing = path.back();
			path.निकालो_back();
		}
	} अन्यथा यदि (object->has_attribute(ID::hdlname)) {
		प्रति (स्वतः ध्रुव &item : object->get_hdlname_attribute())
			path.धकेलो_back(twines.add("\\" + item));
		यदि (!path.empty()) {
			path.निकालो_back();
		}
	} अन्यथा {
		प्रति (स्वतः ध्रुव &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.धकेलो_back(twines.add("\\" + item));
	}
	प्रत्यागम {path, trailing};
}

संरचना ModuleHdlnameIndex {
	प्रकारनिर्देश Id_वृक्ष<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_वृक्ष<ModuleItem> वृक्ष;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

गुप्त:
	ढाँचा<typename I, typename Filter>
	शून्य index_वस्तुएँ(I begin, I end, Filter filter);

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
		प्रत्यागम {वृक्ष.cursor(pair.first), pair.second};
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
		result.धकेलो_back(object->get_src_attribute());
		प्रत्यागम result;
	}
};

गणना class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
