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
	số_nguyên depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
public: // XXX
	dict<IdString, std::unique_ptr<IdTree>> subtrees;

	template<typename P, typename T_ref>
	static Cursor do_insert(IdTree *tree, P begin, P end, T_ref &&value)
	{
		log_assert(begin != end && "path must be non-empty");
		trong_khi (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			tree->names.insert(name);
			nếu (begin == end) {
				tree->entries.emplace(name, std::forward<T_ref>(value));
				trả_về Cursor(tree, name);
			}
			auto &unique = tree->subtrees[name];
			nếu (!unique) {
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
			nếu (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			IdTree *tree = nullptr;
			nếu (scope_name.empty()) {
				tree = target;
			} không_thì {
				auto found = target->subtrees.find(scope_name);
				nếu (found != target->subtrees.end()) {
					tree = found->second.get();
				} không_thì {
					trả_về Cursor();
				}
			}
			nếu (tree->names.empty()) {
				trả_về Cursor();
			}
			trả_về Cursor(tree, *tree->names.begin());
		}

		Cursor do_next_sibling() {
			nếu (scope_name.empty())
				trả_về Cursor();
			auto found = target->names.find(scope_name);
			nếu (found == target->names.end())
				trả_về Cursor();
			++found;
			nếu (found == target->names.end())
				trả_về Cursor();
			trả_về Cursor(target, *found);
		}

		Cursor do_parent() {
			nếu (scope_name.empty())
				trả_về Cursor();
			nếu (target->parent != nullptr)
				trả_về Cursor(target->parent, target->scope_name);
			trả_về Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			nếu (next.valid())
				trả_về next;
			trong_khi (current.valid()) {
				nếu (next.valid())
					trả_về next;
				next = current.do_next_sibling();
				nếu (next.valid())
					trả_về next;
				current = current.do_parent();
			}
			trả_về current;
		}

		Cursor do_child(IdString name) {
			IdTree *tree = nullptr;
			nếu (scope_name.empty()) {
				tree = target;
			} không_thì {
				auto found = target->subtrees.find(scope_name);
				nếu (found != target->subtrees.end()) {
					tree = found->second.get();
				} không_thì {
					trả_về Cursor();
				}
			}
			auto found = tree->names.find(name);
			nếu (found == tree->names.end()) {
				trả_về Cursor();
			}
			trả_về Cursor(tree, *found);
		}

	public:
		bool operator==(const Cursor &other) const {
			trả_về target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(const Cursor &other) const {
			trả_về !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const
		{
			h.eat(scope_name);
			h.eat(target);
			trả_về h;
		}

		bool valid() const {
			trả_về target != nullptr;
		}

		số_nguyên depth() const {
			log_assert(valid());
			trả_về target->depth + !scope_name.empty();
		}

		bool is_root() const {
			trả_về target != nullptr && scope_name.empty();
		}

		bool has_entry() const {
			log_assert(valid());
			trả_về !scope_name.empty() && target->entries.count(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			trả_về target->entries.at(scope_name);
		}

		void assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			nếu (scope_name.empty())
				trả_về;
			out_path.push_back(scope_name);
			IdTree *current = target;
			trong_khi (current->parent) {
				out_path.push_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			trả_về result;
		}

		std::string path_str(const RTLIL::Design *design) {
			std::string result;
			cho (const auto &item : path()) {
				nếu (!result.empty())
					result.push_back(' ');
				result += design->twines().unescaped_str(item);
			}
			trả_về result;
		}

		Cursor first_child() {
			log_assert(valid());
			trả_về do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			trả_về do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			trả_về do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			trả_về do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			trong_khi (current != other) {
				nếu (!current.valid() || !other.valid())
					trả_về Cursor();
				số_nguyên delta = current.depth() - other.depth();
				nếu (delta >= 0)
					current = current.do_parent();
				nếu (delta <= 0)
					other = other.do_parent();
			}
			trả_về current;
		}
	};

	template<typename P>
	Cursor insert(P begin, P end, const T &value) {
		trả_về do_insert(this, begin, end, value);
	}

	template<typename P>
	Cursor insert(P begin, P end, T &&value) {
		trả_về do_insert(this, begin, end, std::move(value));
	}

	template<typename P>
	Cursor insert(const P &path, const T &value) {
		trả_về do_insert(this, path.begin(), path.end(), value);
	}

	template<typename P>
	Cursor insert(const P &path, T &&value) {
		trả_về do_insert(this, path.begin(), path.end(), std::move(value));
	}

	Cursor cursor() {
		trả_về parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		cho (; begin != end; ++begin) {
			current = current.do_child(*begin);
			nếu (!current.valid())
				break;
		}
		trả_về current;
	}

	template<typename P>
	Cursor cursor(const P &path) {
		trả_về cursor(path.begin(), path.end());
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

	bool is_wire() const { trả_về type == Type::Wire; }
	bool is_cell() const { trả_về type == Type::Cell; }

	Wire *wire() const { trả_về type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() const { trả_về type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(const ModuleItem &other) const { trả_về ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(ptr); trả_về h; }
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
	cho (auto const &item : object->get_hdlname_attribute())
		path.push_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	nếu (path.empty() && name.isPublic())
		path.push_back(name);
	nếu (!path.empty() && synthetic) {
		path.pop_back();
		path.push_back(name);
	}
	trả_về path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(const O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	nếu (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		cho (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		nếu (!path.empty()) {
			trailing = path.back();
			path.pop_back();
		}
	} không_thì nếu (object->has_attribute(ID::hdlname)) {
		cho (auto const &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		nếu (!path.empty()) {
			path.pop_back();
		}
	} không_thì {
		cho (auto const &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.push_back(twines.add("\\" + item));
	}
	trả_về {path, trailing};
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
		trả_về {tree.cursor(pair.first), pair.second};
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
		trả_về result;
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
