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
	struct Cursor;

protected:
	IdTree *parent = nullptr;
	IdString scope_name;
	整数 depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
public: // XXX
	dict<IdString, std::unique_ptr<IdTree>> subtrees;

	template<typename P, typename T_ref>
	static Cursor do_insert(IdTree *tree, P begin, P end, T_ref &&value)
	{
		log_assert(begin != end && "path must be non-empty");
		当 (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			tree->names.insert(name);
			如果 (begin == end) {
				tree->entries.emplace(name, std::forward<T_ref>(value));
				返回 Cursor(tree, name);
			}
			auto &unique = tree->subtrees[name];
			如果 (!unique) {
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
	struct Cursor
	{
		friend class IdTree;
	protected:
	public:
		IdTree *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(IdTree *target, IdString scope_name) : target(target), scope_name(scope_name) {
			如果 (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			IdTree *tree = nullptr;
			如果 (scope_name.empty()) {
				tree = target;
			} 否则 {
				auto found = target->subtrees.find(scope_name);
				如果 (found != target->subtrees.end()) {
					tree = found->second.get();
				} 否则 {
					返回 Cursor();
				}
			}
			如果 (tree->names.empty()) {
				返回 Cursor();
			}
			返回 Cursor(tree, *tree->names.begin());
		}

		Cursor do_next_sibling() {
			如果 (scope_name.empty())
				返回 Cursor();
			auto found = target->names.find(scope_name);
			如果 (found == target->names.end())
				返回 Cursor();
			++found;
			如果 (found == target->names.end())
				返回 Cursor();
			返回 Cursor(target, *found);
		}

		Cursor do_parent() {
			如果 (scope_name.empty())
				返回 Cursor();
			如果 (target->parent != nullptr)
				返回 Cursor(target->parent, target->scope_name);
			返回 Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			如果 (next.valid())
				返回 next;
			当 (current.valid()) {
				如果 (next.valid())
					返回 next;
				next = current.do_next_sibling();
				如果 (next.valid())
					返回 next;
				current = current.do_parent();
			}
			返回 current;
		}

		Cursor do_child(IdString name) {
			IdTree *tree = nullptr;
			如果 (scope_name.empty()) {
				tree = target;
			} 否则 {
				auto found = target->subtrees.find(scope_name);
				如果 (found != target->subtrees.end()) {
					tree = found->second.get();
				} 否则 {
					返回 Cursor();
				}
			}
			auto found = tree->names.find(name);
			如果 (found == tree->names.end()) {
				返回 Cursor();
			}
			返回 Cursor(tree, *found);
		}

	public:
		bool operator==(const Cursor &other) const {
			返回 target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(const Cursor &other) const {
			返回 !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const
		{
			h.eat(scope_name);
			h.eat(target);
			返回 h;
		}

		bool valid() const {
			返回 target != nullptr;
		}

		整数 depth() const {
			log_assert(valid());
			返回 target->depth + !scope_name.empty();
		}

		bool is_root() const {
			返回 target != nullptr && scope_name.empty();
		}

		bool has_entry() const {
			log_assert(valid());
			返回 !scope_name.empty() && target->entries.count(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			返回 target->entries.at(scope_name);
		}

		void assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			如果 (scope_name.empty())
				返回;
			out_path.push_back(scope_name);
			IdTree *current = target;
			当 (current->parent) {
				out_path.push_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			返回 result;
		}

		std::string path_str(const RTLIL::Design *design) {
			std::string result;
			为 (const auto &item : path()) {
				如果 (!result.empty())
					result.push_back(' ');
				result += design->twines().unescaped_str(item);
			}
			返回 result;
		}

		Cursor first_child() {
			log_assert(valid());
			返回 do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			返回 do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			返回 do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			返回 do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			当 (current != other) {
				如果 (!current.valid() || !other.valid())
					返回 Cursor();
				整数 delta = current.depth() - other.depth();
				如果 (delta >= 0)
					current = current.do_parent();
				如果 (delta <= 0)
					other = other.do_parent();
			}
			返回 current;
		}
	};

	template<typename P>
	Cursor insert(P begin, P end, const T &value) {
		返回 do_insert(this, begin, end, value);
	}

	template<typename P>
	Cursor insert(P begin, P end, T &&value) {
		返回 do_insert(this, begin, end, std::move(value));
	}

	template<typename P>
	Cursor insert(const P &path, const T &value) {
		返回 do_insert(this, path.begin(), path.end(), value);
	}

	template<typename P>
	Cursor insert(const P &path, T &&value) {
		返回 do_insert(this, path.begin(), path.end(), std::move(value));
	}

	Cursor cursor() {
		返回 parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		为 (; begin != end; ++begin) {
			current = current.do_child(*begin);
			如果 (!current.valid())
				break;
		}
		返回 current;
	}

	template<typename P>
	Cursor cursor(const P &path) {
		返回 cursor(path.begin(), path.end());
	}
};


struct ModuleItem {
	enum class Type {
		Wire,
		Cell,
	};
	Type type;
	void *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() const { 返回 type == Type::Wire; }
	bool is_cell() const { 返回 type == Type::Cell; }

	Wire *wire() const { 返回 type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() const { 返回 type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(const ModuleItem &other) const { 返回 ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(ptr); 返回 h; }
};

static inline void log_dump_val_worker(typename IdTree<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
static inline void log_dump_val_worker(const typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(const O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	为 (auto const &item : object->get_hdlname_attribute())
		path.push_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	如果 (path.empty() && name.isPublic())
		path.push_back(name);
	如果 (!path.empty() && synthetic) {
		path.pop_back();
		path.push_back(name);
	}
	返回 path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(const O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	如果 (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		为 (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		如果 (!path.empty()) {
			trailing = path.back();
			path.pop_back();
		}
	} 否则 如果 (object->has_attribute(ID::hdlname)) {
		为 (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		如果 (!path.empty()) {
			path.pop_back();
		}
	} 否则 {
		为 (auto const &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.push_back(twines.add("\\" + item));
	}
	返回 {path, trailing};
}

struct ModuleHdlnameIndex {
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
		返回 {tree.cursor(pair.first), pair.second};
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
		返回 result;
	}
};

enum class ScopeinfoAttrs {
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
