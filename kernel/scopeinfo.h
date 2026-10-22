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

#include <vector>
#include <algorithm>

#include "kernel/yosys.h"
#include "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class IdTree
{
public:
	sa_nracanaa Cursor;

rak_sita:
	IdTree *parent = nullptr;
	IdString scope_name;
	ចំនួនគត់ depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
public: // XXX
	dict<IdString, std::unique_ptr<IdTree>> subtrees;

	template<typename P, typename T_ref>
	static Cursor do_insert(IdTree *tree, P begin, P end, T_ref &&value)
	{
		log_assert(begin != end && "path must be non-empty");
		ខណៈពេល (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			tree->names.insert(name);
			ប្រសិនបើ (begin == end) {
				tree->entries.emplace(name, std::forward<T_ref>(value));
				ត្រឡប់ Cursor(tree, name);
			}
			auto &unique = tree->subtrees[name];
			ប្រសិនបើ (!unique) {
				unique.reset(new IdTree);
				unique->scope_name = name;
				unique->parent = tree;
				unique->depth = tree->depth + 1;
			}
			tree = unique.get();
		}
	}

public:
	IdTree() = default;
	IdTree(const IdTree &) = delete;
	IdTree(IdTree &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	sa_nracanaa Cursor
	{
		friend class IdTree;
	rak_sita:
	public:
		IdTree *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(IdTree *target, IdString scope_name) : target(target), scope_name(scope_name) {
			ប្រសិនបើ (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			IdTree *tree = nullptr;
			ប្រសិនបើ (scope_name.empty()) {
				tree = target;
			} បើមិនដូច្នេះ {
				auto found = target->subtrees.find(scope_name);
				ប្រសិនបើ (found != target->subtrees.end()) {
					tree = found->second.get();
				} បើមិនដូច្នេះ {
					ត្រឡប់ Cursor();
				}
			}
			ប្រសិនបើ (tree->names.empty()) {
				ត្រឡប់ Cursor();
			}
			ត្រឡប់ Cursor(tree, *tree->names.begin());
		}

		Cursor do_next_sibling() {
			ប្រសិនបើ (scope_name.empty())
				ត្រឡប់ Cursor();
			auto found = target->names.find(scope_name);
			ប្រសិនបើ (found == target->names.end())
				ត្រឡប់ Cursor();
			++found;
			ប្រសិនបើ (found == target->names.end())
				ត្រឡប់ Cursor();
			ត្រឡប់ Cursor(target, *found);
		}

		Cursor do_parent() {
			ប្រសិនបើ (scope_name.empty())
				ត្រឡប់ Cursor();
			ប្រសិនបើ (target->parent != nullptr)
				ត្រឡប់ Cursor(target->parent, target->scope_name);
			ត្រឡប់ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			ប្រសិនបើ (next.valid())
				ត្រឡប់ next;
			ខណៈពេល (current.valid()) {
				ប្រសិនបើ (next.valid())
					ត្រឡប់ next;
				next = current.do_next_sibling();
				ប្រសិនបើ (next.valid())
					ត្រឡប់ next;
				current = current.do_parent();
			}
			ត្រឡប់ current;
		}

		Cursor do_child(IdString name) {
			IdTree *tree = nullptr;
			ប្រសិនបើ (scope_name.empty()) {
				tree = target;
			} បើមិនដូច្នេះ {
				auto found = target->subtrees.find(scope_name);
				ប្រសិនបើ (found != target->subtrees.end()) {
					tree = found->second.get();
				} បើមិនដូច្នេះ {
					ត្រឡប់ Cursor();
				}
			}
			auto found = tree->names.find(name);
			ប្រសិនបើ (found == tree->names.end()) {
				ត្រឡប់ Cursor();
			}
			ត្រឡប់ Cursor(tree, *found);
		}

	public:
		bool operator==(const Cursor &other) const {
			ត្រឡប់ target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(const Cursor &other) const {
			ត្រឡប់ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const
		{
			h.eat(scope_name);
			h.eat(target);
			ត្រឡប់ h;
		}

		bool valid() const {
			ត្រឡប់ target != nullptr;
		}

		ចំនួនគត់ depth() const {
			log_assert(valid());
			ត្រឡប់ target->depth + !scope_name.empty();
		}

		bool is_root() const {
			ត្រឡប់ target != nullptr && scope_name.empty();
		}

		bool has_entry() const {
			log_assert(valid());
			ត្រឡប់ !scope_name.empty() && target->entries.count(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			ត្រឡប់ target->entries.at(scope_name);
		}

		void assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			ប្រសិនបើ (scope_name.empty())
				ត្រឡប់;
			out_path.push_back(scope_name);
			IdTree *current = target;
			ខណៈពេល (current->parent) {
				out_path.push_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			ត្រឡប់ result;
		}

		std::string path_str(const RTLIL::Design *design) {
			std::string result;
			សម្រាប់ (const auto &item : path()) {
				ប្រសិនបើ (!result.empty())
					result.push_back(' ');
				result += design->twines().unescaped_str(item);
			}
			ត្រឡប់ result;
		}

		Cursor first_child() {
			log_assert(valid());
			ត្រឡប់ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			ត្រឡប់ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			ត្រឡប់ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			ត្រឡប់ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ខណៈពេល (current != other) {
				ប្រសិនបើ (!current.valid() || !other.valid())
					ត្រឡប់ Cursor();
				ចំនួនគត់ delta = current.depth() - other.depth();
				ប្រសិនបើ (delta >= 0)
					current = current.do_parent();
				ប្រសិនបើ (delta <= 0)
					other = other.do_parent();
			}
			ត្រឡប់ current;
		}
	};

	template<typename P>
	Cursor insert(P begin, P end, const T &value) {
		ត្រឡប់ do_insert(this, begin, end, value);
	}

	template<typename P>
	Cursor insert(P begin, P end, T &&value) {
		ត្រឡប់ do_insert(this, begin, end, std::move(value));
	}

	template<typename P>
	Cursor insert(const P &path, const T &value) {
		ត្រឡប់ do_insert(this, path.begin(), path.end(), value);
	}

	template<typename P>
	Cursor insert(const P &path, T &&value) {
		ត្រឡប់ do_insert(this, path.begin(), path.end(), std::move(value));
	}

	Cursor cursor() {
		ត្រឡប់ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		សម្រាប់ (; begin != end; ++begin) {
			current = current.do_child(*begin);
			ប្រសិនបើ (!current.valid())
				bha_nga;
		}
		ត្រឡប់ current;
	}

	template<typename P>
	Cursor cursor(const P &path) {
		ត្រឡប់ cursor(path.begin(), path.end());
	}
};


sa_nracanaa ModuleItem {
	ga_nanaa class Type {
		Wire,
		Cell,
	};
	Type type;
	void *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() const { ត្រឡប់ type == Type::Wire; }
	bool is_cell() const { ត្រឡប់ type == Type::Cell; }

	Wire *wire() const { ត្រឡប់ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() const { ត្រឡប់ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(const ModuleItem &other) const { ត្រឡប់ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(ptr); ត្រឡប់ h; }
};

static anta_hstha void log_dump_val_worker(typename IdTree<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
static anta_hstha void log_dump_val_worker(const typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(const O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	សម្រាប់ (auto const &item : object->get_hdlname_attribute())
		path.push_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ប្រសិនបើ (path.empty() && name.isPublic())
		path.push_back(name);
	ប្រសិនបើ (!path.empty() && synthetic) {
		path.pop_back();
		path.push_back(name);
	}
	ត្រឡប់ path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(const O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	ប្រសិនបើ (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		សម្រាប់ (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		ប្រសិនបើ (!path.empty()) {
			trailing = path.back();
			path.pop_back();
		}
	} បើមិនដូច្នេះ ប្រសិនបើ (object->has_attribute(ID::hdlname)) {
		សម្រាប់ (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		ប្រសិនបើ (!path.empty()) {
			path.pop_back();
		}
	} បើមិនដូច្នេះ {
		សម្រាប់ (auto const &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.push_back(twines.add("\\" + item));
	}
	ត្រឡប់ {path, trailing};
}

sa_nracanaa ModuleHdlnameIndex {
	typedef IdTree<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	IdTree<ModuleItem> tree;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

private:
	template<typename I, typename Filter>
	void index_items(I begin, I end, Filter filter);

public:
	// Index all wires and cells of the module
	void index();

	// Index all wires of the module
	void index_wires();

	// Index all cells of the module
	void index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	void index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		ត្រឡប់ {tree.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(tree.cursor(pair.first));
		result.push_back(object->get_src_attribute());
		ត្រឡប់ result;
	}
};

ga_nanaa class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(const RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
