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

template<typename T>
class Id_वृक्षः
{
सार्वजनिक:
	संरचना Cursor;

रक्षित:
	Id_वृक्षः *parent = nullptr;
	IdString scope_name;
	पूर्णाङ्क depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
सार्वजनिक: // XXX
	dict<IdString, std::unique_ptr<Id_वृक्षः>> subtrees;

	template<typename P, typename T_ref>
	स्थिर Cursor do_निवेशय(Id_वृक्षः *वृक्षः, P begin, P end, T_ref &&मानम्)
	{
		log_assert(begin != end && "path must be non-empty");
		याव (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			वृक्षः->names.निवेशय(name);
			सचे (begin == end) {
				वृक्षः->entries.emplace(name, std::forward<T_ref>(मानम्));
				पटिनिवत्त Cursor(वृक्षः, name);
			}
			auto &unique = वृक्षः->subtrees[name];
			सचे (!unique) {
				unique.reset(new Id_वृक्षः);
				unique->scope_name = name;
				unique->parent = वृक्षः;
				unique->depth = वृक्षः->depth + 1;
			}
			वृक्षः = unique.get();
		}
	}

सार्वजनिक:
	Id_वृक्षः() = स्वभाव;
	Id_वृक्षः(ध्रुव Id_वृक्षः &) = delete;
	Id_वृक्षः(Id_वृक्षः &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	संरचना Cursor
	{
		friend class Id_वृक्षः;
	रक्षित:
	सार्वजनिक:
		Id_वृक्षः *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_वृक्षः *target, IdString scope_name) : target(target), scope_name(scope_name) {
			सचे (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_वृक्षः *वृक्षः = nullptr;
			सचे (scope_name.empty()) {
				वृक्षः = target;
			} अञ्ञथा {
				auto found = target->subtrees.find(scope_name);
				सचे (found != target->subtrees.end()) {
					वृक्षः = found->second.get();
				} अञ्ञथा {
					पटिनिवत्त Cursor();
				}
			}
			सचे (वृक्षः->names.empty()) {
				पटिनिवत्त Cursor();
			}
			पटिनिवत्त Cursor(वृक्षः, *वृक्षः->names.begin());
		}

		Cursor do_next_sibling() {
			सचे (scope_name.empty())
				पटिनिवत्त Cursor();
			auto found = target->names.find(scope_name);
			सचे (found == target->names.end())
				पटिनिवत्त Cursor();
			++found;
			सचे (found == target->names.end())
				पटिनिवत्त Cursor();
			पटिनिवत्त Cursor(target, *found);
		}

		Cursor do_parent() {
			सचे (scope_name.empty())
				पटिनिवत्त Cursor();
			सचे (target->parent != nullptr)
				पटिनिवत्त Cursor(target->parent, target->scope_name);
			पटिनिवत्त Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			सचे (next.valid())
				पटिनिवत्त next;
			याव (current.valid()) {
				सचे (next.valid())
					पटिनिवत्त next;
				next = current.do_next_sibling();
				सचे (next.valid())
					पटिनिवत्त next;
				current = current.do_parent();
			}
			पटिनिवत्त current;
		}

		Cursor do_child(IdString name) {
			Id_वृक्षः *वृक्षः = nullptr;
			सचे (scope_name.empty()) {
				वृक्षः = target;
			} अञ्ञथा {
				auto found = target->subtrees.find(scope_name);
				सचे (found != target->subtrees.end()) {
					वृक्षः = found->second.get();
				} अञ्ञथा {
					पटिनिवत्त Cursor();
				}
			}
			auto found = वृक्षः->names.find(name);
			सचे (found == वृक्षः->names.end()) {
				पटिनिवत्त Cursor();
			}
			पटिनिवत्त Cursor(वृक्षः, *found);
		}

	सार्वजनिक:
		bool operator==(ध्रुव Cursor &other) ध्रुव {
			पटिनिवत्त target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ध्रुव Cursor &other) ध्रुव {
			पटिनिवत्त !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव
		{
			h.eat(scope_name);
			h.eat(target);
			पटिनिवत्त h;
		}

		bool valid() ध्रुव {
			पटिनिवत्त target != nullptr;
		}

		पूर्णाङ्क depth() ध्रुव {
			log_assert(valid());
			पटिनिवत्त target->depth + !scope_name.empty();
		}

		bool is_मूलम्() ध्रुव {
			पटिनिवत्त target != nullptr && scope_name.empty();
		}

		bool has_entry() ध्रुव {
			log_assert(valid());
			पटिनिवत्त !scope_name.empty() && target->entries.गणनम्(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			पटिनिवत्त target->entries.at(scope_name);
		}

		शून्य assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			सचे (scope_name.empty())
				पटिनिवत्त;
			out_path.प्रक्षिप_back(scope_name);
			Id_वृक्षः *current = target;
			याव (current->parent) {
				out_path.प्रक्षिप_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			पटिनिवत्त result;
		}

		std::string path_str(ध्रुव RTLIL::Design *design) {
			std::string result;
			अत्थाय (ध्रुव auto &item : path()) {
				सचे (!result.empty())
					result.प्रक्षिप_back(' ');
				result += design->twines().unescaped_str(item);
			}
			पटिनिवत्त result;
		}

		Cursor first_child() {
			log_assert(valid());
			पटिनिवत्त do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			पटिनिवत्त do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			पटिनिवत्त do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			पटिनिवत्त do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			याव (current != other) {
				सचे (!current.valid() || !other.valid())
					पटिनिवत्त Cursor();
				पूर्णाङ्क delta = current.depth() - other.depth();
				सचे (delta >= 0)
					current = current.do_parent();
				सचे (delta <= 0)
					other = other.do_parent();
			}
			पटिनिवत्त current;
		}
	};

	template<typename P>
	Cursor निवेशय(P begin, P end, ध्रुव T &मानम्) {
		पटिनिवत्त do_निवेशय(this, begin, end, मानम्);
	}

	template<typename P>
	Cursor निवेशय(P begin, P end, T &&मानम्) {
		पटिनिवत्त do_निवेशय(this, begin, end, std::move(मानम्));
	}

	template<typename P>
	Cursor निवेशय(ध्रुव P &path, ध्रुव T &मानम्) {
		पटिनिवत्त do_निवेशय(this, path.begin(), path.end(), मानम्);
	}

	template<typename P>
	Cursor निवेशय(ध्रुव P &path, T &&मानम्) {
		पटिनिवत्त do_निवेशय(this, path.begin(), path.end(), std::move(मानम्));
	}

	Cursor cursor() {
		पटिनिवत्त parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		अत्थाय (; begin != end; ++begin) {
			current = current.do_child(*begin);
			सचे (!current.valid())
				भङ्ग;
		}
		पटिनिवत्त current;
	}

	template<typename P>
	Cursor cursor(ध्रुव P &path) {
		पटिनिवत्त cursor(path.begin(), path.end());
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

	bool is_wire() ध्रुव { पटिनिवत्त type == Type::Wire; }
	bool is_cell() ध्रुव { पटिनिवत्त type == Type::Cell; }

	Wire *wire() ध्रुव { पटिनिवत्त type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ध्रुव { पटिनिवत्त type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ध्रुव ModuleItem &other) ध्रुव { पटिनिवत्त ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(ptr); पटिनिवत्त h; }
};

स्थिर अन्तःस्थ शून्य log_dump_val_worker(typename Id_वृक्षः<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(ध्रुव typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	अत्थाय (auto ध्रुव &item : object->get_hdlname_attribute())
		path.प्रक्षिप_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	सचे (path.empty() && name.isPublic())
		path.प्रक्षिप_back(name);
	सचे (!path.empty() && synthetic) {
		path.निष्कासय_back();
		path.प्रक्षिप_back(name);
	}
	पटिनिवत्त path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	सचे (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		अत्थाय (auto ध्रुव &item : object->get_hdlname_attribute())
			path.प्रक्षिप_back(twines.add("\\" + item));
		सचे (!path.empty()) {
			trailing = path.back();
			path.निष्कासय_back();
		}
	} अञ्ञथा सचे (object->has_attribute(ID::hdlname)) {
		अत्थाय (auto ध्रुव &item : object->get_hdlname_attribute())
			path.प्रक्षिप_back(twines.add("\\" + item));
		सचे (!path.empty()) {
			path.निष्कासय_back();
		}
	} अञ्ञथा {
		अत्थाय (auto ध्रुव &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.प्रक्षिप_back(twines.add("\\" + item));
	}
	पटिनिवत्त {path, trailing};
}

संरचना ModuleHdlnameIndex {
	प्रकारनिर्देश Id_वृक्षः<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_वृक्षः<ModuleItem> वृक्षः;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

गुप्त:
	template<typename I, typename Filter>
	शून्य index_वस्तूनि(I begin, I end, Filter filter);

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
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		पटिनिवत्त {वृक्षः.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(वृक्षः.cursor(pair.first));
		result.प्रक्षिप_back(object->get_src_attribute());
		पटिनिवत्त result;
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
