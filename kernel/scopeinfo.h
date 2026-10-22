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

#অন্তর্ভাৱ <vector>
#অন্তর্ভাৱ <algorithm>

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_বৃক্ষ
{
সার্ৱজনিক:
	সংরচনা Cursor;

রক্ষিত:
	Id_বৃক্ষ *parent = nullptr;
	IdString scope_name;
	পূর্ণাঙ্ক depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
সার্ৱজনিক: // XXX
	dict<IdString, std::unique_ptr<Id_বৃক্ষ>> subtrees;

	template<typename P, typename T_ref>
	স্থির Cursor do_ঢোকাও(Id_বৃক্ষ *বৃক্ষ, P begin, P end, T_ref &&মান)
	{
		log_assert(begin != end && "path must be non-empty");
		যেতিয়ালৈ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			বৃক্ষ->names.ঢোকাও(name);
			যদি (begin == end) {
				বৃক্ষ->entries.emplace(name, std::forward<T_ref>(মান));
				ঘূৰাই Cursor(বৃক্ষ, name);
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

সার্ৱজনিক:
	Id_বৃক্ষ() = স্ৱভাৱ;
	Id_বৃক্ষ(ধ্রুৱ Id_বৃক্ষ &) = delete;
	Id_বৃক্ষ(Id_বৃক্ষ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	সংরচনা Cursor
	{
		friend class Id_বৃক্ষ;
	রক্ষিত:
	সার্ৱজনিক:
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
					ঘূৰাই Cursor();
				}
			}
			যদি (বৃক্ষ->names.empty()) {
				ঘূৰাই Cursor();
			}
			ঘূৰাই Cursor(বৃক্ষ, *বৃক্ষ->names.begin());
		}

		Cursor do_next_sibling() {
			যদি (scope_name.empty())
				ঘূৰাই Cursor();
			auto found = target->names.find(scope_name);
			যদি (found == target->names.end())
				ঘূৰাই Cursor();
			++found;
			যদি (found == target->names.end())
				ঘূৰাই Cursor();
			ঘূৰাই Cursor(target, *found);
		}

		Cursor do_parent() {
			যদি (scope_name.empty())
				ঘূৰাই Cursor();
			যদি (target->parent != nullptr)
				ঘূৰাই Cursor(target->parent, target->scope_name);
			ঘূৰাই Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			যদি (next.valid())
				ঘূৰাই next;
			যেতিয়ালৈ (current.valid()) {
				যদি (next.valid())
					ঘূৰাই next;
				next = current.do_next_sibling();
				যদি (next.valid())
					ঘূৰাই next;
				current = current.do_parent();
			}
			ঘূৰাই current;
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
					ঘূৰাই Cursor();
				}
			}
			auto found = বৃক্ষ->names.find(name);
			যদি (found == বৃক্ষ->names.end()) {
				ঘূৰাই Cursor();
			}
			ঘূৰাই Cursor(বৃক্ষ, *found);
		}

	সার্ৱজনিক:
		bool operator==(ধ্রুৱ Cursor &other) ধ্রুৱ {
			ঘূৰাই target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ধ্রুৱ Cursor &other) ধ্রুৱ {
			ঘূৰাই !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ
		{
			h.eat(scope_name);
			h.eat(target);
			ঘূৰাই h;
		}

		bool valid() ধ্রুৱ {
			ঘূৰাই target != nullptr;
		}

		পূর্ণাঙ্ক depth() ধ্রুৱ {
			log_assert(valid());
			ঘূৰাই target->depth + !scope_name.empty();
		}

		bool is_মূল() ধ্রুৱ {
			ঘূৰাই target != nullptr && scope_name.empty();
		}

		bool has_entry() ধ্রুৱ {
			log_assert(valid());
			ঘূৰাই !scope_name.empty() && target->entries.গুনতি(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			ঘূৰাই target->entries.at(scope_name);
		}

		শূন্য assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			যদি (scope_name.empty())
				ঘূৰাই;
			out_path.ঠেলো_back(scope_name);
			Id_বৃক্ষ *current = target;
			যেতিয়ালৈ (current->parent) {
				out_path.ঠেলো_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			ঘূৰাই result;
		}

		std::string path_str(ধ্রুৱ RTLIL::Design *design) {
			std::string result;
			বাবে (ধ্রুৱ auto &item : path()) {
				যদি (!result.empty())
					result.ঠেলো_back(' ');
				result += design->twines().unescaped_str(item);
			}
			ঘূৰাই result;
		}

		Cursor first_child() {
			log_assert(valid());
			ঘূৰাই do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			ঘূৰাই do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			ঘূৰাই do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			ঘূৰাই do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			যেতিয়ালৈ (current != other) {
				যদি (!current.valid() || !other.valid())
					ঘূৰাই Cursor();
				পূর্ণাঙ্ক delta = current.depth() - other.depth();
				যদি (delta >= 0)
					current = current.do_parent();
				যদি (delta <= 0)
					other = other.do_parent();
			}
			ঘূৰাই current;
		}
	};

	template<typename P>
	Cursor ঢোকাও(P begin, P end, ধ্রুৱ T &মান) {
		ঘূৰাই do_ঢোকাও(this, begin, end, মান);
	}

	template<typename P>
	Cursor ঢোকাও(P begin, P end, T &&মান) {
		ঘূৰাই do_ঢোকাও(this, begin, end, std::move(মান));
	}

	template<typename P>
	Cursor ঢোকাও(ধ্রুৱ P &path, ধ্রুৱ T &মান) {
		ঘূৰাই do_ঢোকাও(this, path.begin(), path.end(), মান);
	}

	template<typename P>
	Cursor ঢোকাও(ধ্রুৱ P &path, T &&মান) {
		ঘূৰাই do_ঢোকাও(this, path.begin(), path.end(), std::move(মান));
	}

	Cursor cursor() {
		ঘূৰাই parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		বাবে (; begin != end; ++begin) {
			current = current.do_child(*begin);
			যদি (!current.valid())
				ভঙ্গ;
		}
		ঘূৰাই current;
	}

	template<typename P>
	Cursor cursor(ধ্রুৱ P &path) {
		ঘূৰাই cursor(path.begin(), path.end());
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

	bool is_wire() ধ্রুৱ { ঘূৰাই type == Type::Wire; }
	bool is_cell() ধ্রুৱ { ঘূৰাই type == Type::Cell; }

	Wire *wire() ধ্রুৱ { ঘূৰাই type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ধ্রুৱ { ঘূৰাই type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ধ্রুৱ ModuleItem &other) ধ্রুৱ { ঘূৰাই ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ { h.eat(ptr); ঘূৰাই h; }
};

স্থির অন্তঃস্থ শূন্য log_dump_val_worker(typename Id_বৃক্ষ<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(ধ্রুৱ typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ধ্রুৱ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	বাবে (auto ধ্রুৱ &item : object->get_hdlname_attribute())
		path.ঠেলো_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	যদি (path.empty() && name.isPublic())
		path.ঠেলো_back(name);
	যদি (!path.empty() && synthetic) {
		path.বেৰ_কৰো_back();
		path.ঠেলো_back(name);
	}
	ঘূৰাই path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ধ্রুৱ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	যদি (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		বাবে (auto ধ্রুৱ &item : object->get_hdlname_attribute())
			path.ঠেলো_back(twines.add("\\" + item));
		যদি (!path.empty()) {
			trailing = path.back();
			path.বেৰ_কৰো_back();
		}
	} অন্যথা যদি (object->has_attribute(ID::hdlname)) {
		বাবে (auto ধ্রুৱ &item : object->get_hdlname_attribute())
			path.ঠেলো_back(twines.add("\\" + item));
		যদি (!path.empty()) {
			path.বেৰ_কৰো_back();
		}
	} অন্যথা {
		বাবে (auto ধ্রুৱ &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ঠেলো_back(twines.add("\\" + item));
	}
	ঘূৰাই {path, trailing};
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

সার্ৱজনিক:
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
		ঘূৰাই {বৃক্ষ.cursor(pair.first), pair.second};
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
		ঘূৰাই result;
	}
};

গণনা class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ধ্রুৱ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
