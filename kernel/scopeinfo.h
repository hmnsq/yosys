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
	puur_naa_nka depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
public: // XXX
	dict<IdString, std::unique_ptr<IdTree>> subtrees;

	template<typename P, typename T_ref>
	static Cursor do_insert(IdTree *tree, P begin, P end, T_ref &&value)
	{
		log_assert(begin != end && "path must be non-empty");
		ᮞᮨᮓᮨᮜ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			tree->names.insert(name);
			ᮚᮦᮔ᮪ (begin == end) {
				tree->entries.emplace(name, std::forward<T_ref>(value));
				ᮝᮜᮤ Cursor(tree, name);
			}
			auto &unique = tree->subtrees[name];
			ᮚᮦᮔ᮪ (!unique) {
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
			ᮚᮦᮔ᮪ (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			IdTree *tree = nullptr;
			ᮚᮦᮔ᮪ (scope_name.empty()) {
				tree = target;
			} ᮜᮤᮚ {
				auto found = target->subtrees.find(scope_name);
				ᮚᮦᮔ᮪ (found != target->subtrees.end()) {
					tree = found->second.get();
				} ᮜᮤᮚ {
					ᮝᮜᮤ Cursor();
				}
			}
			ᮚᮦᮔ᮪ (tree->names.empty()) {
				ᮝᮜᮤ Cursor();
			}
			ᮝᮜᮤ Cursor(tree, *tree->names.begin());
		}

		Cursor do_next_sibling() {
			ᮚᮦᮔ᮪ (scope_name.empty())
				ᮝᮜᮤ Cursor();
			auto found = target->names.find(scope_name);
			ᮚᮦᮔ᮪ (found == target->names.end())
				ᮝᮜᮤ Cursor();
			++found;
			ᮚᮦᮔ᮪ (found == target->names.end())
				ᮝᮜᮤ Cursor();
			ᮝᮜᮤ Cursor(target, *found);
		}

		Cursor do_parent() {
			ᮚᮦᮔ᮪ (scope_name.empty())
				ᮝᮜᮤ Cursor();
			ᮚᮦᮔ᮪ (target->parent != nullptr)
				ᮝᮜᮤ Cursor(target->parent, target->scope_name);
			ᮝᮜᮤ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			ᮚᮦᮔ᮪ (next.valid())
				ᮝᮜᮤ next;
			ᮞᮨᮓᮨᮜ (current.valid()) {
				ᮚᮦᮔ᮪ (next.valid())
					ᮝᮜᮤ next;
				next = current.do_next_sibling();
				ᮚᮦᮔ᮪ (next.valid())
					ᮝᮜᮤ next;
				current = current.do_parent();
			}
			ᮝᮜᮤ current;
		}

		Cursor do_child(IdString name) {
			IdTree *tree = nullptr;
			ᮚᮦᮔ᮪ (scope_name.empty()) {
				tree = target;
			} ᮜᮤᮚ {
				auto found = target->subtrees.find(scope_name);
				ᮚᮦᮔ᮪ (found != target->subtrees.end()) {
					tree = found->second.get();
				} ᮜᮤᮚ {
					ᮝᮜᮤ Cursor();
				}
			}
			auto found = tree->names.find(name);
			ᮚᮦᮔ᮪ (found == tree->names.end()) {
				ᮝᮜᮤ Cursor();
			}
			ᮝᮜᮤ Cursor(tree, *found);
		}

	public:
		bool operator==(const Cursor &other) const {
			ᮝᮜᮤ target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(const Cursor &other) const {
			ᮝᮜᮤ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const
		{
			h.eat(scope_name);
			h.eat(target);
			ᮝᮜᮤ h;
		}

		bool valid() const {
			ᮝᮜᮤ target != nullptr;
		}

		puur_naa_nka depth() const {
			log_assert(valid());
			ᮝᮜᮤ target->depth + !scope_name.empty();
		}

		bool is_root() const {
			ᮝᮜᮤ target != nullptr && scope_name.empty();
		}

		bool has_entry() const {
			log_assert(valid());
			ᮝᮜᮤ !scope_name.empty() && target->entries.count(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			ᮝᮜᮤ target->entries.at(scope_name);
		}

		void assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			ᮚᮦᮔ᮪ (scope_name.empty())
				ᮝᮜᮤ;
			out_path.push_back(scope_name);
			IdTree *current = target;
			ᮞᮨᮓᮨᮜ (current->parent) {
				out_path.push_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			ᮝᮜᮤ result;
		}

		std::string path_str(const RTLIL::Design *design) {
			std::string result;
			for (const auto &item : path()) {
				ᮚᮦᮔ᮪ (!result.empty())
					result.push_back(' ');
				result += design->twines().unescaped_str(item);
			}
			ᮝᮜᮤ result;
		}

		Cursor first_child() {
			log_assert(valid());
			ᮝᮜᮤ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			ᮝᮜᮤ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			ᮝᮜᮤ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			ᮝᮜᮤ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ᮞᮨᮓᮨᮜ (current != other) {
				ᮚᮦᮔ᮪ (!current.valid() || !other.valid())
					ᮝᮜᮤ Cursor();
				puur_naa_nka delta = current.depth() - other.depth();
				ᮚᮦᮔ᮪ (delta >= 0)
					current = current.do_parent();
				ᮚᮦᮔ᮪ (delta <= 0)
					other = other.do_parent();
			}
			ᮝᮜᮤ current;
		}
	};

	template<typename P>
	Cursor insert(P begin, P end, const T &value) {
		ᮝᮜᮤ do_insert(this, begin, end, value);
	}

	template<typename P>
	Cursor insert(P begin, P end, T &&value) {
		ᮝᮜᮤ do_insert(this, begin, end, std::move(value));
	}

	template<typename P>
	Cursor insert(const P &path, const T &value) {
		ᮝᮜᮤ do_insert(this, path.begin(), path.end(), value);
	}

	template<typename P>
	Cursor insert(const P &path, T &&value) {
		ᮝᮜᮤ do_insert(this, path.begin(), path.end(), std::move(value));
	}

	Cursor cursor() {
		ᮝᮜᮤ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		for (; begin != end; ++begin) {
			current = current.do_child(*begin);
			ᮚᮦᮔ᮪ (!current.valid())
				bha_nga;
		}
		ᮝᮜᮤ current;
	}

	template<typename P>
	Cursor cursor(const P &path) {
		ᮝᮜᮤ cursor(path.begin(), path.end());
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

	bool is_wire() const { ᮝᮜᮤ type == Type::Wire; }
	bool is_cell() const { ᮝᮜᮤ type == Type::Cell; }

	Wire *wire() const { ᮝᮜᮤ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() const { ᮝᮜᮤ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(const ModuleItem &other) const { ᮝᮜᮤ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(ptr); ᮝᮜᮤ h; }
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
	for (auto const &item : object->get_hdlname_attribute())
		path.push_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ᮚᮦᮔ᮪ (path.empty() && name.isPublic())
		path.push_back(name);
	ᮚᮦᮔ᮪ (!path.empty() && synthetic) {
		path.pop_back();
		path.push_back(name);
	}
	ᮝᮜᮤ path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(const O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	ᮚᮦᮔ᮪ (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		for (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		ᮚᮦᮔ᮪ (!path.empty()) {
			trailing = path.back();
			path.pop_back();
		}
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (object->has_attribute(ID::hdlname)) {
		for (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		ᮚᮦᮔ᮪ (!path.empty()) {
			path.pop_back();
		}
	} ᮜᮤᮚ {
		for (auto const &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.push_back(twines.add("\\" + item));
	}
	ᮝᮜᮤ {path, trailing};
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
		ᮝᮜᮤ {tree.cursor(pair.first), pair.second};
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
		ᮝᮜᮤ result;
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
