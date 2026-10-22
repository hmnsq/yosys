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

#ଅନ୍ତର୍ଭାଵ <vector>
#ଅନ୍ତର୍ଭାଵ <algorithm>

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_ବୃକ୍ଷ
{
ସର୍ବସାଧାରଣ:
	ସଂରଚନା Cursor;

ସୁରକ୍ଷିତ:
	Id_ବୃକ୍ଷ *parent = nullptr;
	IdString scope_name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
ସର୍ବସାଧାରଣ: // XXX
	dict<IdString, std::unique_ptr<Id_ବୃକ୍ଷ>> subtrees;

	template<typename P, typename T_ref>
	ସ୍ଥିର Cursor do_ଭର୍ତ୍ତି(Id_ବୃକ୍ଷ *ବୃକ୍ଷ, P begin, P end, T_ref &&ମୂଲ୍ୟ)
	{
		log_assert(begin != end && "path must be non-empty");
		ଯେତେବେଳେ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			ବୃକ୍ଷ->names.ଭର୍ତ୍ତି(name);
			ଯଦି (begin == end) {
				ବୃକ୍ଷ->entries.emplace(name, std::forward<T_ref>(ମୂଲ୍ୟ));
				ଫେରନ୍ତୁ Cursor(ବୃକ୍ଷ, name);
			}
			auto &unique = ବୃକ୍ଷ->subtrees[name];
			ଯଦି (!unique) {
				unique.reset(new Id_ବୃକ୍ଷ);
				unique->scope_name = name;
				unique->parent = ବୃକ୍ଷ;
				unique->depth = ବୃକ୍ଷ->depth + 1;
			}
			ବୃକ୍ଷ = unique.get();
		}
	}

ସର୍ବସାଧାରଣ:
	Id_ବୃକ୍ଷ() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	Id_ବୃକ୍ଷ(ଅଚଳ Id_ବୃକ୍ଷ &) = delete;
	Id_ବୃକ୍ଷ(Id_ବୃକ୍ଷ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ସଂରଚନା Cursor
	{
		friend class Id_ବୃକ୍ଷ;
	ସୁରକ୍ଷିତ:
	ସର୍ବସାଧାରଣ:
		Id_ବୃକ୍ଷ *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_ବୃକ୍ଷ *target, IdString scope_name) : target(target), scope_name(scope_name) {
			ଯଦି (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_ବୃକ୍ଷ *ବୃକ୍ଷ = nullptr;
			ଯଦି (scope_name.empty()) {
				ବୃକ୍ଷ = target;
			} ଅନ୍ୟଥା {
				auto found = target->subtrees.find(scope_name);
				ଯଦି (found != target->subtrees.end()) {
					ବୃକ୍ଷ = found->second.get();
				} ଅନ୍ୟଥା {
					ଫେରନ୍ତୁ Cursor();
				}
			}
			ଯଦି (ବୃକ୍ଷ->names.empty()) {
				ଫେରନ୍ତୁ Cursor();
			}
			ଫେରନ୍ତୁ Cursor(ବୃକ୍ଷ, *ବୃକ୍ଷ->names.begin());
		}

		Cursor do_next_sibling() {
			ଯଦି (scope_name.empty())
				ଫେରନ୍ତୁ Cursor();
			auto found = target->names.find(scope_name);
			ଯଦି (found == target->names.end())
				ଫେରନ୍ତୁ Cursor();
			++found;
			ଯଦି (found == target->names.end())
				ଫେରନ୍ତୁ Cursor();
			ଫେରନ୍ତୁ Cursor(target, *found);
		}

		Cursor do_parent() {
			ଯଦି (scope_name.empty())
				ଫେରନ୍ତୁ Cursor();
			ଯଦି (target->parent != nullptr)
				ଫେରନ୍ତୁ Cursor(target->parent, target->scope_name);
			ଫେରନ୍ତୁ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			ଯଦି (next.valid())
				ଫେରନ୍ତୁ next;
			ଯେତେବେଳେ (current.valid()) {
				ଯଦି (next.valid())
					ଫେରନ୍ତୁ next;
				next = current.do_next_sibling();
				ଯଦି (next.valid())
					ଫେରନ୍ତୁ next;
				current = current.do_parent();
			}
			ଫେରନ୍ତୁ current;
		}

		Cursor do_child(IdString name) {
			Id_ବୃକ୍ଷ *ବୃକ୍ଷ = nullptr;
			ଯଦି (scope_name.empty()) {
				ବୃକ୍ଷ = target;
			} ଅନ୍ୟଥା {
				auto found = target->subtrees.find(scope_name);
				ଯଦି (found != target->subtrees.end()) {
					ବୃକ୍ଷ = found->second.get();
				} ଅନ୍ୟଥା {
					ଫେରନ୍ତୁ Cursor();
				}
			}
			auto found = ବୃକ୍ଷ->names.find(name);
			ଯଦି (found == ବୃକ୍ଷ->names.end()) {
				ଫେରନ୍ତୁ Cursor();
			}
			ଫେରନ୍ତୁ Cursor(ବୃକ୍ଷ, *found);
		}

	ସର୍ବସାଧାରଣ:
		bool operator==(ଅଚଳ Cursor &other) ଅଚଳ {
			ଫେରନ୍ତୁ target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(ଅଚଳ Cursor &other) ଅଚଳ {
			ଫେରନ୍ତୁ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ
		{
			h.eat(scope_name);
			h.eat(target);
			ଫେରନ୍ତୁ h;
		}

		bool valid() ଅଚଳ {
			ଫେରନ୍ତୁ target != nullptr;
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth() ଅଚଳ {
			log_assert(valid());
			ଫେରନ୍ତୁ target->depth + !scope_name.empty();
		}

		bool is_ମୂଳ() ଅଚଳ {
			ଫେରନ୍ତୁ target != nullptr && scope_name.empty();
		}

		bool has_entry() ଅଚଳ {
			log_assert(valid());
			ଫେରନ୍ତୁ !scope_name.empty() && target->entries.ଗଣନା_2(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			ଫେରନ୍ତୁ target->entries.at(scope_name);
		}

		ଶୂନ୍ୟ assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			ଯଦି (scope_name.empty())
				ଫେରନ୍ତୁ;
			out_path.ଠେଲ_back(scope_name);
			Id_ବୃକ୍ଷ *current = target;
			ଯେତେବେଳେ (current->parent) {
				out_path.ଠେଲ_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			ଫେରନ୍ତୁ result;
		}

		std::string path_str(ଅଚଳ RTLIL::Design *design) {
			std::string result;
			ପାଇଁ (ଅଚଳ auto &item : path()) {
				ଯଦି (!result.empty())
					result.ଠେଲ_back(' ');
				result += design->twines().unescaped_str(item);
			}
			ଫେରନ୍ତୁ result;
		}

		Cursor first_child() {
			log_assert(valid());
			ଫେରନ୍ତୁ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			ଫେରନ୍ତୁ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			ଫେରନ୍ତୁ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			ଫେରନ୍ତୁ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ଯେତେବେଳେ (current != other) {
				ଯଦି (!current.valid() || !other.valid())
					ଫେରନ୍ତୁ Cursor();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା delta = current.depth() - other.depth();
				ଯଦି (delta >= 0)
					current = current.do_parent();
				ଯଦି (delta <= 0)
					other = other.do_parent();
			}
			ଫେରନ୍ତୁ current;
		}
	};

	template<typename P>
	Cursor ଭର୍ତ୍ତି(P begin, P end, ଅଚଳ T &ମୂଲ୍ୟ) {
		ଫେରନ୍ତୁ do_ଭର୍ତ୍ତି(this, begin, end, ମୂଲ୍ୟ);
	}

	template<typename P>
	Cursor ଭର୍ତ୍ତି(P begin, P end, T &&ମୂଲ୍ୟ) {
		ଫେରନ୍ତୁ do_ଭର୍ତ୍ତି(this, begin, end, std::move(ମୂଲ୍ୟ));
	}

	template<typename P>
	Cursor ଭର୍ତ୍ତି(ଅଚଳ P &path, ଅଚଳ T &ମୂଲ୍ୟ) {
		ଫେରନ୍ତୁ do_ଭର୍ତ୍ତି(this, path.begin(), path.end(), ମୂଲ୍ୟ);
	}

	template<typename P>
	Cursor ଭର୍ତ୍ତି(ଅଚଳ P &path, T &&ମୂଲ୍ୟ) {
		ଫେରନ୍ତୁ do_ଭର୍ତ୍ତି(this, path.begin(), path.end(), std::move(ମୂଲ୍ୟ));
	}

	Cursor cursor() {
		ଫେରନ୍ତୁ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		ପାଇଁ (; begin != end; ++begin) {
			current = current.do_child(*begin);
			ଯଦି (!current.valid())
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ current;
	}

	template<typename P>
	Cursor cursor(ଅଚଳ P &path) {
		ଫେରନ୍ତୁ cursor(path.begin(), path.end());
	}
};


ସଂରଚନା ModuleItem {
	ଗଣନା class Type {
		Wire,
		Cell,
	};
	Type type;
	ଶୂନ୍ୟ *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ଅଚଳ { ଫେରନ୍ତୁ type == Type::Wire; }
	bool is_cell() ଅଚଳ { ଫେରନ୍ତୁ type == Type::Cell; }

	Wire *wire() ଅଚଳ { ଫେରନ୍ତୁ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ଅଚଳ { ଫେରନ୍ତୁ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(ଅଚଳ ModuleItem &other) ଅଚଳ { ଫେରନ୍ତୁ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(ptr); ଫେରନ୍ତୁ h; }
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(typename Id_ବୃକ୍ଷ<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଅଚଳ typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(ଅଚଳ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	ପାଇଁ (auto ଅଚଳ &item : object->get_hdlname_attribute())
		path.ଠେଲ_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ଯଦି (path.empty() && name.isPublic())
		path.ଠେଲ_back(name);
	ଯଦି (!path.empty() && synthetic) {
		path.କାଢ଼_back();
		path.ଠେଲ_back(name);
	}
	ଫେରନ୍ତୁ path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(ଅଚଳ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	ଯଦି (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		ପାଇଁ (auto ଅଚଳ &item : object->get_hdlname_attribute())
			path.ଠେଲ_back(twines.add("\\" + item));
		ଯଦି (!path.empty()) {
			trailing = path.back();
			path.କାଢ଼_back();
		}
	} ଅନ୍ୟଥା ଯଦି (object->has_attribute(ID::hdlname)) {
		ପାଇଁ (auto ଅଚଳ &item : object->get_hdlname_attribute())
			path.ଠେଲ_back(twines.add("\\" + item));
		ଯଦି (!path.empty()) {
			path.କାଢ଼_back();
		}
	} ଅନ୍ୟଥା {
		ପାଇଁ (auto ଅଚଳ &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.ଠେଲ_back(twines.add("\\" + item));
	}
	ଫେରନ୍ତୁ {path, trailing};
}

ସଂରଚନା ModuleHdlnameIndex {
	ପ୍ରକାରବ୍ୟାଖ୍ୟା Id_ବୃକ୍ଷ<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_ବୃକ୍ଷ<ModuleItem> ବୃକ୍ଷ;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

ବ୍ୟକ୍ତିଗତ:
	template<typename I, typename Filter>
	ଶୂନ୍ୟ index_ବସ୍ତୁଗୁଡ଼ିକ(I begin, I end, Filter filter);

ସର୍ବସାଧାରଣ:
	// Index all wires and cells of the module
	ଶୂନ୍ୟ index();

	// Index all wires of the module
	ଶୂନ୍ୟ index_wires();

	// Index all cells of the module
	ଶୂନ୍ୟ index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	ଶୂନ୍ୟ index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		ଫେରନ୍ତୁ {ବୃକ୍ଷ.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(ବୃକ୍ଷ.cursor(pair.first));
		result.ଠେଲ_back(object->get_src_attribute());
		ଫେରନ୍ତୁ result;
	}
};

ଗଣନା class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(ଅଚଳ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
