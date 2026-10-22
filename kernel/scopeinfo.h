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

#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <algorithm>

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_ವೃಕ್ಷ
{
ಸಾರ್ವಜನಿಕ:
	ಸಂರಚನಾ Cursor;

ರಕ್ಷಿತ:
	Id_ವೃಕ್ಷ *parent = nullptr;
	IdString scope_name;
	ಪೂರ್ಣಾಙ್ಕ depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
ಸಾರ್ವಜನಿಕ: // XXX
	dict<IdString, std::unique_ptr<Id_ವೃಕ್ಷ>> subtrees;

	template<typename P, typename T_ref>
	ಸ್ಥಿರ Cursor do_ಸೇರಿಸು(Id_ವೃಕ್ಷ *ವೃಕ್ಷ, P begin, P end, T_ref &&ಮೌಲ್ಯ)
	{
		log_assert(begin != end && "path must be non-empty");
		ಯಾವತ್ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			ವೃಕ್ಷ->names.ಸೇರಿಸು(name);
			ಯದಿ (begin == end) {
				ವೃಕ್ಷ->entries.emplace(name, std::forward<T_ref>(ಮೌಲ್ಯ));
				ಪ್ರತ್ಯಾಗಮ Cursor(ವೃಕ್ಷ, name);
			}
			auto &unique = ವೃಕ್ಷ->subtrees[name];
			ಯದಿ (!unique) {
				unique.reset(new Id_ವೃಕ್ಷ);
				unique->scope_name = name;
				unique->parent = ವೃಕ್ಷ;
				unique->depth = ವೃಕ್ಷ->depth + 1;
			}
			ವೃಕ್ಷ = unique.get();
		}
	}

ಸಾರ್ವಜನಿಕ:
	Id_ವೃಕ್ಷ() = ಸ್ವಭಾವ;
	Id_ವೃಕ್ಷ(ಧ್ರುವ Id_ವೃಕ್ಷ &) = delete;
	Id_ವೃಕ್ಷ(Id_ವೃಕ್ಷ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ಸಂರಚನಾ Cursor
	{
		friend class Id_ವೃಕ್ಷ;
	ರಕ್ಷಿತ:
	ಸಾರ್ವಜನಿಕ:
		Id_ವೃಕ್ಷ *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_ವೃಕ್ಷ *target, IdString scope_name) : target(target), scope_name(scope_name) {
			ಯದಿ (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_ವೃಕ್ಷ *ವೃಕ್ಷ = nullptr;
			ಯದಿ (scope_name.empty()) {
				ವೃಕ್ಷ = target;
			} ಅನ್ಯಥಾ {
				auto found = target->subtrees.find(scope_name);
				ಯದಿ (found != target->subtrees.end()) {
					ವೃಕ್ಷ = found->second.get();
				} ಅನ್ಯಥಾ {
					ಪ್ರತ್ಯಾಗಮ Cursor();
				}
			}
			ಯದಿ (ವೃಕ್ಷ->names.empty()) {
				ಪ್ರತ್ಯಾಗಮ Cursor();
			}
			ಪ್ರತ್ಯಾಗಮ Cursor(ವೃಕ್ಷ, *ವೃಕ್ಷ->names.begin());
		}

		Cursor do_next_sibling() {
			ಯದಿ (scope_name.empty())
				ಪ್ರತ್ಯಾಗಮ Cursor();
			auto found = target->names.find(scope_name);
			ಯದಿ (found == target->names.end())
				ಪ್ರತ್ಯಾಗಮ Cursor();
			++found;
			ಯದಿ (found == target->names.end())
				ಪ್ರತ್ಯಾಗಮ Cursor();
			ಪ್ರತ್ಯಾಗಮ Cursor(target, *found);
		}

		Cursor do_parent() {
			ಯದಿ (scope_name.empty())
				ಪ್ರತ್ಯಾಗಮ Cursor();
			ಯದಿ (target->parent != nullptr)
				ಪ್ರತ್ಯಾಗಮ Cursor(target->parent, target->scope_name);
			ಪ್ರತ್ಯಾಗಮ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			ಯದಿ (next.valid())
				ಪ್ರತ್ಯಾಗಮ next;
			ಯಾವತ್ (current.valid()) {
				ಯದಿ (next.valid())
					ಪ್ರತ್ಯಾಗಮ next;
				next = current.do_next_sibling();
				ಯದಿ (next.valid())
					ಪ್ರತ್ಯಾಗಮ next;
				current = current.do_parent();
			}
			ಪ್ರತ್ಯಾಗಮ current;
		}

		Cursor do_child(IdString name) {
			Id_ವೃಕ್ಷ *ವೃಕ್ಷ = nullptr;
			ಯದಿ (scope_name.empty()) {
				ವೃಕ್ಷ = target;
			} ಅನ್ಯಥಾ {
				auto found = target->subtrees.find(scope_name);
				ಯದಿ (found != target->subtrees.end()) {
					ವೃಕ್ಷ = found->second.get();
				} ಅನ್ಯಥಾ {
					ಪ್ರತ್ಯಾಗಮ Cursor();
				}
			}
			auto found = ವೃಕ್ಷ->names.find(name);
			ಯದಿ (found == ವೃಕ್ಷ->names.end()) {
				ಪ್ರತ್ಯಾಗಮ Cursor();
			}
			ಪ್ರತ್ಯಾಗಮ Cursor(ವೃಕ್ಷ, *found);
		}

	ಸಾರ್ವಜನಿಕ:
		bool operator==(ಧ್ರುವ Cursor &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ಧ್ರುವ Cursor &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ
		{
			h.eat(scope_name);
			h.eat(target);
			ಪ್ರತ್ಯಾಗಮ h;
		}

		bool valid() ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ target != nullptr;
		}

		ಪೂರ್ಣಾಙ್ಕ depth() ಧ್ರುವ {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ target->depth + !scope_name.empty();
		}

		bool is_ಬೇರು() ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ target != nullptr && scope_name.empty();
		}

		bool has_entry() ಧ್ರುವ {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ !scope_name.empty() && target->entries.ಎಣಿಕೆ(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			ಪ್ರತ್ಯಾಗಮ target->entries.at(scope_name);
		}

		ಶೂನ್ಯ assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			ಯದಿ (scope_name.empty())
				ಪ್ರತ್ಯಾಗಮ;
			out_path.ತಳ್ಳು_back(scope_name);
			Id_ವೃಕ್ಷ *current = target;
			ಯಾವತ್ (current->parent) {
				out_path.ತಳ್ಳು_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			ಪ್ರತ್ಯಾಗಮ result;
		}

		std::string path_str(ಧ್ರುವ RTLIL::Design *design) {
			std::string result;
			ಪ್ರತಿ (ಧ್ರುವ auto &item : path()) {
				ಯದಿ (!result.empty())
					result.ತಳ್ಳು_back(' ');
				result += design->twines().unescaped_str(item);
			}
			ಪ್ರತ್ಯಾಗಮ result;
		}

		Cursor first_child() {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			ಪ್ರತ್ಯಾಗಮ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ಯಾವತ್ (current != other) {
				ಯದಿ (!current.valid() || !other.valid())
					ಪ್ರತ್ಯಾಗಮ Cursor();
				ಪೂರ್ಣಾಙ್ಕ delta = current.depth() - other.depth();
				ಯದಿ (delta >= 0)
					current = current.do_parent();
				ಯದಿ (delta <= 0)
					other = other.do_parent();
			}
			ಪ್ರತ್ಯಾಗಮ current;
		}
	};

	template<typename P>
	Cursor ಸೇರಿಸು(P begin, P end, ಧ್ರುವ T &ಮೌಲ್ಯ) {
		ಪ್ರತ್ಯಾಗಮ do_ಸೇರಿಸು(this, begin, end, ಮೌಲ್ಯ);
	}

	template<typename P>
	Cursor ಸೇರಿಸು(P begin, P end, T &&ಮೌಲ್ಯ) {
		ಪ್ರತ್ಯಾಗಮ do_ಸೇರಿಸು(this, begin, end, std::move(ಮೌಲ್ಯ));
	}

	template<typename P>
	Cursor ಸೇರಿಸು(ಧ್ರುವ P &path, ಧ್ರುವ T &ಮೌಲ್ಯ) {
		ಪ್ರತ್ಯಾಗಮ do_ಸೇರಿಸು(this, path.begin(), path.end(), ಮೌಲ್ಯ);
	}

	template<typename P>
	Cursor ಸೇರಿಸು(ಧ್ರುವ P &path, T &&ಮೌಲ್ಯ) {
		ಪ್ರತ್ಯಾಗಮ do_ಸೇರಿಸು(this, path.begin(), path.end(), std::move(ಮೌಲ್ಯ));
	}

	Cursor cursor() {
		ಪ್ರತ್ಯಾಗಮ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		ಪ್ರತಿ (; begin != end; ++begin) {
			current = current.do_child(*begin);
			ಯದಿ (!current.valid())
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ current;
	}

	template<typename P>
	Cursor cursor(ಧ್ರುವ P &path) {
		ಪ್ರತ್ಯಾಗಮ cursor(path.begin(), path.end());
	}
};


ಸಂರಚನಾ ModuleItem {
	ಗಣನಾ class Type {
		Wire,
		Cell,
	};
	Type type;
	ಶೂನ್ಯ *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type == Type::Wire; }
	bool is_cell() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type == Type::Cell; }

	Wire *wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ಧ್ರುವ ModuleItem &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(ptr); ಪ್ರತ್ಯಾಗಮ h; }
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ log_dump_val_worker(typename Id_ವೃಕ್ಷ<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ log_dump_val_worker(ಧ್ರುವ typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ಧ್ರುವ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	ಪ್ರತಿ (auto ಧ್ರುವ &item : object->get_hdlname_attribute())
		path.ತಳ್ಳು_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ಯದಿ (path.empty() && name.isPublic())
		path.ತಳ್ಳು_back(name);
	ಯದಿ (!path.empty() && synthetic) {
		path.ತೆಗೆ_back();
		path.ತಳ್ಳು_back(name);
	}
	ಪ್ರತ್ಯಾಗಮ path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ಧ್ರುವ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	ಯದಿ (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		ಪ್ರತಿ (auto ಧ್ರುವ &item : object->get_hdlname_attribute())
			path.ತಳ್ಳು_back(twines.add("\\" + item));
		ಯದಿ (!path.empty()) {
			trailing = path.back();
			path.ತೆಗೆ_back();
		}
	} ಅನ್ಯಥಾ ಯದಿ (object->has_attribute(ID::hdlname)) {
		ಪ್ರತಿ (auto ಧ್ರುವ &item : object->get_hdlname_attribute())
			path.ತಳ್ಳು_back(twines.add("\\" + item));
		ಯದಿ (!path.empty()) {
			path.ತೆಗೆ_back();
		}
	} ಅನ್ಯಥಾ {
		ಪ್ರತಿ (auto ಧ್ರುವ &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ತಳ್ಳು_back(twines.add("\\" + item));
	}
	ಪ್ರತ್ಯಾಗಮ {path, trailing};
}

ಸಂರಚನಾ ModuleHdlnameIndex {
	ಪ್ರಕಾರನಿರ್ದೇಶ Id_ವೃಕ್ಷ<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_ವೃಕ್ಷ<ModuleItem> ವೃಕ್ಷ;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

ಗುಪ್ತ:
	template<typename I, typename Filter>
	ಶೂನ್ಯ index_ವಸ್ತುಗಳು(I begin, I end, Filter filter);

ಸಾರ್ವಜನಿಕ:
	// Index all wires and cells of the module
	ಶೂನ್ಯ index();

	// Index all wires of the module
	ಶೂನ್ಯ index_wires();

	// Index all cells of the module
	ಶೂನ್ಯ index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	ಶೂನ್ಯ index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		ಪ್ರತ್ಯಾಗಮ {ವೃಕ್ಷ.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(ವೃಕ್ಷ.cursor(pair.first));
		result.ತಳ್ಳು_back(object->get_src_attribute());
		ಪ್ರತ್ಯಾಗಮ result;
	}
};

ಗಣನಾ class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ಧ್ರುವ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
