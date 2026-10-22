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

#অন্তর্ভাব <vector>
#অন্তর্ভাব <algorithm>

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_বৃক্ষ
{
সার্বজনিক:
	সংরচনা Cursor;

রক্ষিত:
	Id_বৃক্ষ *parent = nullptr;
	IdString scope_name;
	পূর্ণাঙ্ক depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
সার্বজনিক: // XXX
	dict<IdString, std::unique_ptr<Id_বৃক্ষ>> subtrees;

	template<typename P, typename T_ref>
	স্থির Cursor do_ঢোকাও(Id_বৃক্ষ *বৃক্ষ, P begin, P end, T_ref &&মান)
	{
		log_assert(begin != end && "path must be non-empty");
		যাবত্ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			বৃক্ষ->names.ঢোকাও(name);
			যদি (begin == end) {
				বৃক্ষ->entries.emplace(name, std::forward<T_ref>(মান));
				প্রত্যাগম Cursor(বৃক্ষ, name);
			}
			auto &unique = বৃক্ষ->subtrees[name];
			যদি (!unique) {
				unique.reset(new Id_বৃক্ষ);
				unique->scope_name = name;
				unique->parent = বৃক্ষ;
				unique->depth = বৃক্ষ->depth + 1;
			}
			বৃক্ষ = unique.get();
		}
	}

সার্বজনিক:
	Id_বৃক্ষ() = স্বভাব;
	Id_বৃক্ষ(ধ্রুব Id_বৃক্ষ &) = delete;
	Id_বৃক্ষ(Id_বৃক্ষ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	সংরচনা Cursor
	{
		friend class Id_বৃক্ষ;
	রক্ষিত:
	সার্বজনিক:
		Id_বৃক্ষ *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_বৃক্ষ *target, IdString scope_name) : target(target), scope_name(scope_name) {
			যদি (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_বৃক্ষ *বৃক্ষ = nullptr;
			যদি (scope_name.empty()) {
				বৃক্ষ = target;
			} অন্যথা {
				auto found = target->subtrees.find(scope_name);
				যদি (found != target->subtrees.end()) {
					বৃক্ষ = found->second.get();
				} অন্যথা {
					প্রত্যাগম Cursor();
				}
			}
			যদি (বৃক্ষ->names.empty()) {
				প্রত্যাগম Cursor();
			}
			প্রত্যাগম Cursor(বৃক্ষ, *বৃক্ষ->names.begin());
		}

		Cursor do_next_sibling() {
			যদি (scope_name.empty())
				প্রত্যাগম Cursor();
			auto found = target->names.find(scope_name);
			যদি (found == target->names.end())
				প্রত্যাগম Cursor();
			++found;
			যদি (found == target->names.end())
				প্রত্যাগম Cursor();
			প্রত্যাগম Cursor(target, *found);
		}

		Cursor do_parent() {
			যদি (scope_name.empty())
				প্রত্যাগম Cursor();
			যদি (target->parent != nullptr)
				প্রত্যাগম Cursor(target->parent, target->scope_name);
			প্রত্যাগম Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			যদি (next.valid())
				প্রত্যাগম next;
			যাবত্ (current.valid()) {
				যদি (next.valid())
					প্রত্যাগম next;
				next = current.do_next_sibling();
				যদি (next.valid())
					প্রত্যাগম next;
				current = current.do_parent();
			}
			প্রত্যাগম current;
		}

		Cursor do_child(IdString name) {
			Id_বৃক্ষ *বৃক্ষ = nullptr;
			যদি (scope_name.empty()) {
				বৃক্ষ = target;
			} অন্যথা {
				auto found = target->subtrees.find(scope_name);
				যদি (found != target->subtrees.end()) {
					বৃক্ষ = found->second.get();
				} অন্যথা {
					প্রত্যাগম Cursor();
				}
			}
			auto found = বৃক্ষ->names.find(name);
			যদি (found == বৃক্ষ->names.end()) {
				প্রত্যাগম Cursor();
			}
			প্রত্যাগম Cursor(বৃক্ষ, *found);
		}

	সার্বজনিক:
		bool operator==(ধ্রুব Cursor &other) ধ্রুব {
			প্রত্যাগম target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ধ্রুব Cursor &other) ধ্রুব {
			প্রত্যাগম !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব
		{
			h.eat(scope_name);
			h.eat(target);
			প্রত্যাগম h;
		}

		bool valid() ধ্রুব {
			প্রত্যাগম target != nullptr;
		}

		পূর্ণাঙ্ক depth() ধ্রুব {
			log_assert(valid());
			প্রত্যাগম target->depth + !scope_name.empty();
		}

		bool is_মূল() ধ্রুব {
			প্রত্যাগম target != nullptr && scope_name.empty();
		}

		bool has_entry() ধ্রুব {
			log_assert(valid());
			প্রত্যাগম !scope_name.empty() && target->entries.গুনতি(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			প্রত্যাগম target->entries.at(scope_name);
		}

		শূন্য assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			যদি (scope_name.empty())
				প্রত্যাগম;
			out_path.ঠেলো_back(scope_name);
			Id_বৃক্ষ *current = target;
			যাবত্ (current->parent) {
				out_path.ঠেলো_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			প্রত্যাগম result;
		}

		std::string path_str(ধ্রুব RTLIL::Design *design) {
			std::string result;
			প্রতি (ধ্রুব auto &item : path()) {
				যদি (!result.empty())
					result.ঠেলো_back(' ');
				result += design->twines().unescaped_str(item);
			}
			প্রত্যাগম result;
		}

		Cursor first_child() {
			log_assert(valid());
			প্রত্যাগম do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			প্রত্যাগম do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			প্রত্যাগম do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			প্রত্যাগম do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			যাবত্ (current != other) {
				যদি (!current.valid() || !other.valid())
					প্রত্যাগম Cursor();
				পূর্ণাঙ্ক delta = current.depth() - other.depth();
				যদি (delta >= 0)
					current = current.do_parent();
				যদি (delta <= 0)
					other = other.do_parent();
			}
			প্রত্যাগম current;
		}
	};

	template<typename P>
	Cursor ঢোকাও(P begin, P end, ধ্রুব T &মান) {
		প্রত্যাগম do_ঢোকাও(this, begin, end, মান);
	}

	template<typename P>
	Cursor ঢোকাও(P begin, P end, T &&মান) {
		প্রত্যাগম do_ঢোকাও(this, begin, end, std::move(মান));
	}

	template<typename P>
	Cursor ঢোকাও(ধ্রুব P &path, ধ্রুব T &মান) {
		প্রত্যাগম do_ঢোকাও(this, path.begin(), path.end(), মান);
	}

	template<typename P>
	Cursor ঢোকাও(ধ্রুব P &path, T &&মান) {
		প্রত্যাগম do_ঢোকাও(this, path.begin(), path.end(), std::move(মান));
	}

	Cursor cursor() {
		প্রত্যাগম parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		প্রতি (; begin != end; ++begin) {
			current = current.do_child(*begin);
			যদি (!current.valid())
				ভঙ্গ;
		}
		প্রত্যাগম current;
	}

	template<typename P>
	Cursor cursor(ধ্রুব P &path) {
		প্রত্যাগম cursor(path.begin(), path.end());
	}
};


সংরচনা ModuleItem {
	গণনা class Type {
		Wire,
		Cell,
	};
	Type type;
	শূন্য *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ধ্রুব { প্রত্যাগম type == Type::Wire; }
	bool is_cell() ধ্রুব { প্রত্যাগম type == Type::Cell; }

	Wire *wire() ধ্রুব { প্রত্যাগম type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ধ্রুব { প্রত্যাগম type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ধ্রুব ModuleItem &other) ধ্রুব { প্রত্যাগম ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(ptr); প্রত্যাগম h; }
};

স্থির অন্তঃস্থ শূন্য log_dump_val_worker(typename Id_বৃক্ষ<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(ধ্রুব typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ধ্রুব O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	প্রতি (auto ধ্রুব &item : object->get_hdlname_attribute())
		path.ঠেলো_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	যদি (path.empty() && name.isPublic())
		path.ঠেলো_back(name);
	যদি (!path.empty() && synthetic) {
		path.বের_করো_back();
		path.ঠেলো_back(name);
	}
	প্রত্যাগম path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ধ্রুব O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	যদি (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		প্রতি (auto ধ্রুব &item : object->get_hdlname_attribute())
			path.ঠেলো_back(twines.add("\\" + item));
		যদি (!path.empty()) {
			trailing = path.back();
			path.বের_করো_back();
		}
	} অন্যথা যদি (object->has_attribute(ID::hdlname)) {
		প্রতি (auto ধ্রুব &item : object->get_hdlname_attribute())
			path.ঠেলো_back(twines.add("\\" + item));
		যদি (!path.empty()) {
			path.বের_করো_back();
		}
	} অন্যথা {
		প্রতি (auto ধ্রুব &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ঠেলো_back(twines.add("\\" + item));
	}
	প্রত্যাগম {path, trailing};
}

সংরচনা ModuleHdlnameIndex {
	প্রকারনির্দেশ Id_বৃক্ষ<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_বৃক্ষ<ModuleItem> বৃক্ষ;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

গুপ্ত:
	template<typename I, typename Filter>
	শূন্য index_বস্তুসমূহ(I begin, I end, Filter filter);

সার্বজনিক:
	// Index all wires and cells of the module
	শূন্য index();

	// Index all wires of the module
	শূন্য index_wires();

	// Index all cells of the module
	শূন্য index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	শূন্য index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		প্রত্যাগম {বৃক্ষ.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(বৃক্ষ.cursor(pair.first));
		result.ঠেলো_back(object->get_src_attribute());
		প্রত্যাগম result;
	}
};

গণনা class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ধ্রুব RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ধ্রুব RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ধ্রুব RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
