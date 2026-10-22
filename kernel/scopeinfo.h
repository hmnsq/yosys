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

#ضمن <vector>
#ضمن <algorithm>

#ضمن "kernel/yosys.h"
#ضمن "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class IdTree
{
عام:
	بنية Cursor;

محمي:
	IdTree *parent = nullptr;
	IdString scope_name;
	صحيح depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
عام: // XXX
	dict<IdString, std::unique_ptr<IdTree>> subtrees;

	template<typename P, typename T_ref>
	ساكن Cursor do_insert(IdTree *tree, P begin, P end, T_ref &&value)
	{
		log_assert(begin != end && "path must be non-empty");
		כלעוד (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			tree->names.insert(name);
			אם (begin == end) {
				tree->entries.emplace(name, std::forward<T_ref>(value));
				החזר Cursor(tree, name);
			}
			auto &unique = tree->subtrees[name];
			אם (!unique) {
				unique.reset(new IdTree);
				unique->scope_name = name;
				unique->parent = tree;
				unique->depth = tree->depth + 1;
			}
			tree = unique.get();
		}
	}

عام:
	IdTree() = افتراضي;
	IdTree(ثابت IdTree &) = delete;
	IdTree(IdTree &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	بنية Cursor
	{
		friend class IdTree;
	محمي:
	عام:
		IdTree *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(IdTree *target, IdString scope_name) : target(target), scope_name(scope_name) {
			אם (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			IdTree *tree = nullptr;
			אם (scope_name.empty()) {
				tree = target;
			} אחרת {
				auto found = target->subtrees.find(scope_name);
				אם (found != target->subtrees.end()) {
					tree = found->second.get();
				} אחרת {
					החזר Cursor();
				}
			}
			אם (tree->names.empty()) {
				החזר Cursor();
			}
			החזר Cursor(tree, *tree->names.begin());
		}

		Cursor do_next_sibling() {
			אם (scope_name.empty())
				החזר Cursor();
			auto found = target->names.find(scope_name);
			אם (found == target->names.end())
				החזר Cursor();
			++found;
			אם (found == target->names.end())
				החזר Cursor();
			החזר Cursor(target, *found);
		}

		Cursor do_parent() {
			אם (scope_name.empty())
				החזר Cursor();
			אם (target->parent != nullptr)
				החזר Cursor(target->parent, target->scope_name);
			החזר Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			אם (next.valid())
				החזר next;
			כלעוד (current.valid()) {
				אם (next.valid())
					החזר next;
				next = current.do_next_sibling();
				אם (next.valid())
					החזר next;
				current = current.do_parent();
			}
			החזר current;
		}

		Cursor do_child(IdString name) {
			IdTree *tree = nullptr;
			אם (scope_name.empty()) {
				tree = target;
			} אחרת {
				auto found = target->subtrees.find(scope_name);
				אם (found != target->subtrees.end()) {
					tree = found->second.get();
				} אחרת {
					החזר Cursor();
				}
			}
			auto found = tree->names.find(name);
			אם (found == tree->names.end()) {
				החזר Cursor();
			}
			החזר Cursor(tree, *found);
		}

	عام:
		bool operator==(ثابت Cursor &other) ثابت {
			החזר target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ثابت Cursor &other) ثابت {
			החזר !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت
		{
			h.eat(scope_name);
			h.eat(target);
			החזר h;
		}

		bool valid() ثابت {
			החזר target != nullptr;
		}

		صحيح depth() ثابت {
			log_assert(valid());
			החזר target->depth + !scope_name.empty();
		}

		bool is_root() ثابت {
			החזר target != nullptr && scope_name.empty();
		}

		bool has_entry() ثابت {
			log_assert(valid());
			החזר !scope_name.empty() && target->entries.count(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			החזר target->entries.at(scope_name);
		}

		فراغ assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			אם (scope_name.empty())
				החזר;
			out_path.push_back(scope_name);
			IdTree *current = target;
			כלעוד (current->parent) {
				out_path.push_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			החזר result;
		}

		std::string path_str(ثابت RTLIL::Design *design) {
			std::string result;
			עבור (ثابت auto &item : path()) {
				אם (!result.empty())
					result.push_back(' ');
				result += design->twines().unescaped_str(item);
			}
			החזר result;
		}

		Cursor first_child() {
			log_assert(valid());
			החזר do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			החזר do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			החזר do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			החזר do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			כלעוד (current != other) {
				אם (!current.valid() || !other.valid())
					החזר Cursor();
				صحيح delta = current.depth() - other.depth();
				אם (delta >= 0)
					current = current.do_parent();
				אם (delta <= 0)
					other = other.do_parent();
			}
			החזר current;
		}
	};

	template<typename P>
	Cursor insert(P begin, P end, ثابت T &value) {
		החזר do_insert(this, begin, end, value);
	}

	template<typename P>
	Cursor insert(P begin, P end, T &&value) {
		החזר do_insert(this, begin, end, std::move(value));
	}

	template<typename P>
	Cursor insert(ثابت P &path, ثابت T &value) {
		החזר do_insert(this, path.begin(), path.end(), value);
	}

	template<typename P>
	Cursor insert(ثابت P &path, T &&value) {
		החזר do_insert(this, path.begin(), path.end(), std::move(value));
	}

	Cursor cursor() {
		החזר parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		עבור (; begin != end; ++begin) {
			current = current.do_child(*begin);
			אם (!current.valid())
				اكسر;
		}
		החזר current;
	}

	template<typename P>
	Cursor cursor(ثابت P &path) {
		החזר cursor(path.begin(), path.end());
	}
};


بنية ModuleItem {
	تعداد class Type {
		Wire,
		Cell,
	};
	Type type;
	فراغ *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ثابت { החזר type == Type::Wire; }
	bool is_cell() ثابت { החזר type == Type::Cell; }

	Wire *wire() ثابت { החזר type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ثابت { החזר type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ثابت ModuleItem &other) ثابت { החזר ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(ptr); החזר h; }
};

ساكن مضمن فراغ log_dump_val_worker(typename IdTree<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
ساكن مضمن فراغ log_dump_val_worker(ثابت typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	עבור (auto ثابت &item : object->get_hdlname_attribute())
		path.push_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	אם (path.empty() && name.isPublic())
		path.push_back(name);
	אם (!path.empty() && synthetic) {
		path.pop_back();
		path.push_back(name);
	}
	החזר path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ثابت O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	אם (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		עבור (auto ثابت &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		אם (!path.empty()) {
			trailing = path.back();
			path.pop_back();
		}
	} אחרת אם (object->has_attribute(ID::hdlname)) {
		עבור (auto ثابت &item : object->get_hdlname_attribute())
			path.push_back(twines.add("\\" + item));
		אם (!path.empty()) {
			path.pop_back();
		}
	} אחרת {
		עבור (auto ثابت &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.push_back(twines.add("\\" + item));
	}
	החזר {path, trailing};
}

بنية ModuleHdlnameIndex {
	تعريفنوع IdTree<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	IdTree<ModuleItem> tree;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

خاص:
	template<typename I, typename Filter>
	فراغ index_items(I begin, I end, Filter filter);

عام:
	// Index all wires and cells of the module
	فراغ index();

	// Index all wires of the module
	فراغ index_wires();

	// Index all cells of the module
	فراغ index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	فراغ index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		החזר {tree.cursor(pair.first), pair.second};
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
		החזר result;
	}
};

تعداد class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ثابت RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
