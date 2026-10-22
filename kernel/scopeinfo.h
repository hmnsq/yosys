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

#അന്തര്ഭാവ <vector>
#അന്തര്ഭാവ <algorithm>

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

template<typename T>
class Id_വൃക്ഷം
{
പൊതു:
	ഘടന Cursor;

സംരക്ഷിത:
	Id_വൃക്ഷം *parent = nullptr;
	IdString scope_name;
	പൂർണ്ണസംഖ്യ depth = 0;

	pool<IdString> names;
	dict<IdString, T> entries;
പൊതു: // XXX
	dict<IdString, std::unique_ptr<Id_വൃക്ഷം>> subtrees;

	template<typename P, typename T_ref>
	സ്ഥിതം Cursor do_തിരുകുക(Id_വൃക്ഷം *വൃക്ഷം, P begin, P end, T_ref &&മൂല്യം)
	{
		log_assert(begin != end && "path must be non-empty");
		ആകുമ്പോൾ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.empty());
			വൃക്ഷം->names.തിരുകുക(name);
			എങ്കിൽ (begin == end) {
				വൃക്ഷം->entries.emplace(name, std::forward<T_ref>(മൂല്യം));
				തിരികെ Cursor(വൃക്ഷം, name);
			}
			auto &unique = വൃക്ഷം->subtrees[name];
			എങ്കിൽ (!unique) {
				unique.reset(new Id_വൃക്ഷം);
				unique->scope_name = name;
				unique->parent = വൃക്ഷം;
				unique->depth = വൃക്ഷം->depth + 1;
			}
			വൃക്ഷം = unique.get();
		}
	}

പൊതു:
	Id_വൃക്ഷം() = സ്ഥിരസ്ഥിതി;
	Id_വൃക്ഷം(സ്ഥിരം Id_വൃക്ഷം &) = delete;
	Id_വൃക്ഷം(Id_വൃക്ഷം &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ഘടന Cursor
	{
		friend class Id_വൃക്ഷം;
	സംരക്ഷിത:
	പൊതു:
		Id_വൃക്ഷം *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_വൃക്ഷം *target, IdString scope_name) : target(target), scope_name(scope_name) {
			എങ്കിൽ (scope_name.empty())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_വൃക്ഷം *വൃക്ഷം = nullptr;
			എങ്കിൽ (scope_name.empty()) {
				വൃക്ഷം = target;
			} അല്ലെങ്കിൽ {
				auto found = target->subtrees.find(scope_name);
				എങ്കിൽ (found != target->subtrees.end()) {
					വൃക്ഷം = found->second.get();
				} അല്ലെങ്കിൽ {
					തിരികെ Cursor();
				}
			}
			എങ്കിൽ (വൃക്ഷം->names.empty()) {
				തിരികെ Cursor();
			}
			തിരികെ Cursor(വൃക്ഷം, *വൃക്ഷം->names.begin());
		}

		Cursor do_next_sibling() {
			എങ്കിൽ (scope_name.empty())
				തിരികെ Cursor();
			auto found = target->names.find(scope_name);
			എങ്കിൽ (found == target->names.end())
				തിരികെ Cursor();
			++found;
			എങ്കിൽ (found == target->names.end())
				തിരികെ Cursor();
			തിരികെ Cursor(target, *found);
		}

		Cursor do_parent() {
			എങ്കിൽ (scope_name.empty())
				തിരികെ Cursor();
			എങ്കിൽ (target->parent != nullptr)
				തിരികെ Cursor(target->parent, target->scope_name);
			തിരികെ Cursor(target, IdString());
		}

		Cursor do_next_preorder() {
			Cursor current = *this;
			Cursor next = current.do_first_child();
			എങ്കിൽ (next.valid())
				തിരികെ next;
			ആകുമ്പോൾ (current.valid()) {
				എങ്കിൽ (next.valid())
					തിരികെ next;
				next = current.do_next_sibling();
				എങ്കിൽ (next.valid())
					തിരികെ next;
				current = current.do_parent();
			}
			തിരികെ current;
		}

		Cursor do_child(IdString name) {
			Id_വൃക്ഷം *വൃക്ഷം = nullptr;
			എങ്കിൽ (scope_name.empty()) {
				വൃക്ഷം = target;
			} അല്ലെങ്കിൽ {
				auto found = target->subtrees.find(scope_name);
				എങ്കിൽ (found != target->subtrees.end()) {
					വൃക്ഷം = found->second.get();
				} അല്ലെങ്കിൽ {
					തിരികെ Cursor();
				}
			}
			auto found = വൃക്ഷം->names.find(name);
			എങ്കിൽ (found == വൃക്ഷം->names.end()) {
				തിരികെ Cursor();
			}
			തിരികെ Cursor(വൃക്ഷം, *found);
		}

	പൊതു:
		bool operator==(സ്ഥിരം Cursor &other) സ്ഥിരം {
			തിരികെ target == other.target && scope_name == other.scope_name;
		}
		bool operator!=(സ്ഥിരം Cursor &other) സ്ഥിരം {
			തിരികെ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം
		{
			h.eat(scope_name);
			h.eat(target);
			തിരികെ h;
		}

		bool valid() സ്ഥിരം {
			തിരികെ target != nullptr;
		}

		പൂർണ്ണസംഖ്യ depth() സ്ഥിരം {
			log_assert(valid());
			തിരികെ target->depth + !scope_name.empty();
		}

		bool is_വേര്() സ്ഥിരം {
			തിരികെ target != nullptr && scope_name.empty();
		}

		bool has_entry() സ്ഥിരം {
			log_assert(valid());
			തിരികെ !scope_name.empty() && target->entries.എണ്ണം(scope_name);
		}

		T &entry() {
			log_assert(!scope_name.empty());
			തിരികെ target->entries.at(scope_name);
		}

		ശൂന്യം assign_path_to(std::vector<IdString> &out_path) {
			log_assert(valid());
			out_path.clear();
			എങ്കിൽ (scope_name.empty())
				തിരികെ;
			out_path.തള്ളുക_back(scope_name);
			Id_വൃക്ഷം *current = target;
			ആകുമ്പോൾ (current->parent) {
				out_path.തള്ളുക_back(current->scope_name);
				current = current->parent;
			}
			std::reverse(out_path.begin(), out_path.end());
		}

		std::vector<IdString> path() {
			std::vector<IdString> result;
			assign_path_to(result);
			തിരികെ result;
		}

		std::string path_str(സ്ഥിരം RTLIL::Design *design) {
			std::string result;
			വേണ്ടി (സ്ഥിരം auto &item : path()) {
				എങ്കിൽ (!result.empty())
					result.തള്ളുക_back(' ');
				result += design->twines().unescaped_str(item);
			}
			തിരികെ result;
		}

		Cursor first_child() {
			log_assert(valid());
			തിരികെ do_first_child();
		}

		Cursor next_preorder() {
			log_assert(valid());
			തിരികെ do_next_preorder();
		}

		Cursor parent() {
			log_assert(valid());
			തിരികെ do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			തിരികെ do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ആകുമ്പോൾ (current != other) {
				എങ്കിൽ (!current.valid() || !other.valid())
					തിരികെ Cursor();
				പൂർണ്ണസംഖ്യ delta = current.depth() - other.depth();
				എങ്കിൽ (delta >= 0)
					current = current.do_parent();
				എങ്കിൽ (delta <= 0)
					other = other.do_parent();
			}
			തിരികെ current;
		}
	};

	template<typename P>
	Cursor തിരുകുക(P begin, P end, സ്ഥിരം T &മൂല്യം) {
		തിരികെ do_തിരുകുക(this, begin, end, മൂല്യം);
	}

	template<typename P>
	Cursor തിരുകുക(P begin, P end, T &&മൂല്യം) {
		തിരികെ do_തിരുകുക(this, begin, end, std::move(മൂല്യം));
	}

	template<typename P>
	Cursor തിരുകുക(സ്ഥിരം P &path, സ്ഥിരം T &മൂല്യം) {
		തിരികെ do_തിരുകുക(this, path.begin(), path.end(), മൂല്യം);
	}

	template<typename P>
	Cursor തിരുകുക(സ്ഥിരം P &path, T &&മൂല്യം) {
		തിരികെ do_തിരുകുക(this, path.begin(), path.end(), std::move(മൂല്യം));
	}

	Cursor cursor() {
		തിരികെ parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	template<typename P>
	Cursor cursor(P begin, P end) {
		Cursor current = cursor();
		വേണ്ടി (; begin != end; ++begin) {
			current = current.do_child(*begin);
			എങ്കിൽ (!current.valid())
				നിർത്തുക;
		}
		തിരികെ current;
	}

	template<typename P>
	Cursor cursor(സ്ഥിരം P &path) {
		തിരികെ cursor(path.begin(), path.end());
	}
};


ഘടന ModuleItem {
	എണ്ണൽ class Type {
		Wire,
		Cell,
	};
	Type type;
	ശൂന്യം *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() സ്ഥിരം { തിരികെ type == Type::Wire; }
	bool is_cell() സ്ഥിരം { തിരികെ type == Type::Cell; }

	Wire *wire() സ്ഥിരം { തിരികെ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() സ്ഥിരം { തിരികെ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool operator==(സ്ഥിരം ModuleItem &other) സ്ഥിരം { തിരികെ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(ptr); തിരികെ h; }
};

സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(typename Id_വൃക്ഷം<ModuleItem>::Cursor cursor ) { log("%p ", cursor.target); log_dump_val_worker(cursor.scope_name); }

template<typename T>
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(സ്ഥിരം typename std::unique_ptr<T> &cursor ) { log("unique %p", cursor.get()); }

template<typename O>
std::vector<IdString> parse_hdlname(സ്ഥിരം O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::vector<IdString> path;
	വേണ്ടി (auto സ്ഥിരം &item : object->get_hdlname_attribute())
		path.തള്ളുക_back(twines.add("\\" + item));
	bool synthetic = !name.isPublic() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	എങ്കിൽ (path.empty() && name.isPublic())
		path.തള്ളുക_back(name);
	എങ്കിൽ (!path.empty() && synthetic) {
		path.എടുക്കുക_back();
		path.തള്ളുക_back(name);
	}
	തിരികെ path;
}

template<typename O>
std::pair<std::vector<IdString>, IdString> parse_scopename(സ്ഥിരം O* object)
{
	TwinePool &twines = object->design()->twines();
	std::vector<IdString> path;
	IdString trailing = object->name;
	എങ്കിൽ (trailing.isPublic() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		വേണ്ടി (auto സ്ഥിരം &item : object->get_hdlname_attribute())
			path.തള്ളുക_back(twines.add("\\" + item));
		എങ്കിൽ (!path.empty()) {
			trailing = path.back();
			path.എടുക്കുക_back();
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (object->has_attribute(ID::hdlname)) {
		വേണ്ടി (auto സ്ഥിരം &item : object->get_hdlname_attribute())
			path.തള്ളുക_back(twines.add("\\" + item));
		എങ്കിൽ (!path.empty()) {
			path.എടുക്കുക_back();
		}
	} അല്ലെങ്കിൽ {
		വേണ്ടി (auto സ്ഥിരം &item : split_tokens(object->get_string_attribute(ID(scopename)), " "))
			path.തള്ളുക_back(twines.add("\\" + item));
	}
	തിരികെ {path, trailing};
}

ഘടന ModuleHdlnameIndex {
	തരനിർവചനം Id_വൃക്ഷം<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_വൃക്ഷം<ModuleItem> വൃക്ഷം;
	dict<ModuleItem, Cursor> lookup;

	ModuleHdlnameIndex(RTLIL::Module *module) : module(module) {}

സ്വകാര്യ:
	template<typename I, typename Filter>
	ശൂന്യം index_ഇനങ്ങൾ(I begin, I end, Filter filter);

പൊതു:
	// Index all wires and cells of the module
	ശൂന്യം index();

	// Index all wires of the module
	ശൂന്യം index_wires();

	// Index all cells of the module
	ശൂന്യം index_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	ശൂന്യം index_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	template<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		auto pair = parse_scopename(object);
		തിരികെ {വൃക്ഷം.cursor(pair.first), pair.second};
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
		std::vector<std::string> result = scope_sources(വൃക്ഷം.cursor(pair.first));
		result.തള്ളുക_back(object->get_src_attribute());
		തിരികെ result;
	}
};

എണ്ണൽ class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_attributes(സ്ഥിരം RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_END

#endif
