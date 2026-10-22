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

#यदि_अपरिभाषितम् SCOPEINFO_H
#परिभाषय SCOPEINFO_H

#अन्तर्भाव <vector>
#अन्तर्भाव <algorithm>

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

प्रतिरूपम्<typename T>
class Id_वृक्षः
{
सार्वजनिक:
	संरचना Cursor;

रक्षित:
	Id_वृक्षः *parent = nullptr;
	IdString scope_name;
	पूर्णाङ्क गभीरता = 0;

	pool<IdString> नामानि;
	dict<IdString, T> entries;
सार्वजनिक: // XXX
	dict<IdString, std::unique_ptr<Id_वृक्षः>> subtrees;

	प्रतिरूपम्<typename P, typename T_ref>
	स्थिर Cursor do_निवेशय(Id_वृक्षः *वृक्षः, P begin, P अन्तः, T_ref &&मानम्)
	{
		log_assert(begin != अन्तः && "path must be non-empty");
		यावत् (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.रिक्तानि());
			वृक्षः->नामानि.निवेशय(name);
			यदि (begin == अन्तः) {
				वृक्षः->entries.emplace(name, std::forward<T_ref>(मानम्));
				प्रत्यागम Cursor(वृक्षः, name);
			}
			स्वचालित &unique = वृक्षः->subtrees[name];
			यदि (!unique) {
				unique.reset(new Id_वृक्षः);
				unique->scope_name = name;
				unique->parent = वृक्षः;
				unique->गभीरता = वृक्षः->गभीरता + 1;
			}
			वृक्षः = unique.get();
		}
	}

सार्वजनिक:
	Id_वृक्षः() = स्वभाव;
	Id_वृक्षः(ध्रुव Id_वृक्षः &) = delete;
	Id_वृक्षः(Id_वृक्षः &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	संरचना Cursor
	{
		मित्रम् class Id_वृक्षः;
	रक्षित:
	सार्वजनिक:
		Id_वृक्षः *target;
		IdString scope_name;

		Cursor() : target(nullptr) {}
		Cursor(Id_वृक्षः *target, IdString scope_name) : target(target), scope_name(scope_name) {
			यदि (scope_name.रिक्तानि())
				log_assert(target->parent == nullptr);
		}

		Cursor do_first_child() {
			Id_वृक्षः *वृक्षः = nullptr;
			यदि (scope_name.रिक्तानि()) {
				वृक्षः = target;
			} अन्यथा {
				स्वचालित प्राप्तम् = target->subtrees.find(scope_name);
				यदि (प्राप्तम् != target->subtrees.अन्तः()) {
					वृक्षः = प्राप्तम्->second.get();
				} अन्यथा {
					प्रत्यागम Cursor();
				}
			}
			यदि (वृक्षः->नामानि.रिक्तानि()) {
				प्रत्यागम Cursor();
			}
			प्रत्यागम Cursor(वृक्षः, *वृक्षः->नामानि.begin());
		}

		Cursor do_अग्र_sibling() {
			यदि (scope_name.रिक्तानि())
				प्रत्यागम Cursor();
			स्वचालित प्राप्तम् = target->नामानि.find(scope_name);
			यदि (प्राप्तम् == target->नामानि.अन्तः())
				प्रत्यागम Cursor();
			++प्राप्तम्;
			यदि (प्राप्तम् == target->नामानि.अन्तः())
				प्रत्यागम Cursor();
			प्रत्यागम Cursor(target, *प्राप्तम्);
		}

		Cursor do_parent() {
			यदि (scope_name.रिक्तानि())
				प्रत्यागम Cursor();
			यदि (target->parent != nullptr)
				प्रत्यागम Cursor(target->parent, target->scope_name);
			प्रत्यागम Cursor(target, IdString());
		}

		Cursor do_अग्र_पूर्वक्रमे() {
			Cursor current = *this;
			Cursor अग्र = current.do_first_child();
			यदि (अग्र.valid())
				प्रत्यागम अग्र;
			यावत् (current.valid()) {
				यदि (अग्र.valid())
					प्रत्यागम अग्र;
				अग्र = current.do_अग्र_sibling();
				यदि (अग्र.valid())
					प्रत्यागम अग्र;
				current = current.do_parent();
			}
			प्रत्यागम current;
		}

		Cursor do_child(IdString name) {
			Id_वृक्षः *वृक्षः = nullptr;
			यदि (scope_name.रिक्तानि()) {
				वृक्षः = target;
			} अन्यथा {
				स्वचालित प्राप्तम् = target->subtrees.find(scope_name);
				यदि (प्राप्तम् != target->subtrees.अन्तः()) {
					वृक्षः = प्राप्तम्->second.get();
				} अन्यथा {
					प्रत्यागम Cursor();
				}
			}
			स्वचालित प्राप्तम् = वृक्षः->नामानि.find(name);
			यदि (प्राप्तम् == वृक्षः->नामानि.अन्तः()) {
				प्रत्यागम Cursor();
			}
			प्रत्यागम Cursor(वृक्षः, *प्राप्तम्);
		}

	सार्वजनिक:
		bool संक्रियकः==(ध्रुव Cursor &other) ध्रुव {
			प्रत्यागम target == other.target && scope_name == other.scope_name;
		}
		bool संक्रियकः!=(ध्रुव Cursor &other) ध्रुव {
			प्रत्यागम !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव
		{
			h.eat(scope_name);
			h.eat(target);
			प्रत्यागम h;
		}

		bool valid() ध्रुव {
			प्रत्यागम target != nullptr;
		}

		पूर्णाङ्क गभीरता() ध्रुव {
			log_assert(valid());
			प्रत्यागम target->गभीरता + !scope_name.रिक्तानि();
		}

		bool is_मूलम्() ध्रुव {
			प्रत्यागम target != nullptr && scope_name.रिक्तानि();
		}

		bool has_प्रविष्टिः() ध्रुव {
			log_assert(valid());
			प्रत्यागम !scope_name.रिक्तानि() && target->entries.गणनम्(scope_name);
		}

		T &प्रविष्टिः() {
			log_assert(!scope_name.रिक्तानि());
			प्रत्यागम target->entries.at(scope_name);
		}

		शून्य नियोजय_मार्गः_गन्तव्यम्(std::सदिशम्<IdString> &बहिर्गमनम्_मार्गः) {
			log_assert(valid());
			बहिर्गमनम्_मार्गः.clear();
			यदि (scope_name.रिक्तानि())
				प्रत्यागम;
			बहिर्गमनम्_मार्गः.प्रक्षिप_पृष्ठभागः(scope_name);
			Id_वृक्षः *current = target;
			यावत् (current->parent) {
				बहिर्गमनम्_मार्गः.प्रक्षिप_पृष्ठभागः(current->scope_name);
				current = current->parent;
			}
			std::reverse(बहिर्गमनम्_मार्गः.begin(), बहिर्गमनम्_मार्गः.अन्तः());
		}

		std::सदिशम्<IdString> मार्गः_2() {
			std::सदिशम्<IdString> परिणाम;
			नियोजय_मार्गः_गन्तव्यम्(परिणाम);
			प्रत्यागम परिणाम;
		}

		std::string मार्गः_str(ध्रुव RTLIL::Design *design) {
			std::string परिणाम;
			प्रति (ध्रुव स्वचालित &item : मार्गः_2()) {
				यदि (!परिणाम.रिक्तानि())
					परिणाम.प्रक्षिप_पृष्ठभागः(' ');
				परिणाम += design->twines().unescaped_str(item);
			}
			प्रत्यागम परिणाम;
		}

		Cursor first_child() {
			log_assert(valid());
			प्रत्यागम do_first_child();
		}

		Cursor अग्र_पूर्वक्रमे() {
			log_assert(valid());
			प्रत्यागम do_अग्र_पूर्वक्रमे();
		}

		Cursor parent() {
			log_assert(valid());
			प्रत्यागम do_parent();
		}

		Cursor child(IdString name) {
			log_assert(valid());
			प्रत्यागम do_child(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			यावत् (current != other) {
				यदि (!current.valid() || !other.valid())
					प्रत्यागम Cursor();
				पूर्णाङ्क delta = current.गभीरता() - other.गभीरता();
				यदि (delta >= 0)
					current = current.do_parent();
				यदि (delta <= 0)
					other = other.do_parent();
			}
			प्रत्यागम current;
		}
	};

	प्रतिरूपम्<typename P>
	Cursor निवेशय(P begin, P अन्तः, ध्रुव T &मानम्) {
		प्रत्यागम do_निवेशय(this, begin, अन्तः, मानम्);
	}

	प्रतिरूपम्<typename P>
	Cursor निवेशय(P begin, P अन्तः, T &&मानम्) {
		प्रत्यागम do_निवेशय(this, begin, अन्तः, std::move(मानम्));
	}

	प्रतिरूपम्<typename P>
	Cursor निवेशय(ध्रुव P &मार्गः_2, ध्रुव T &मानम्) {
		प्रत्यागम do_निवेशय(this, मार्गः_2.begin(), मार्गः_2.अन्तः(), मानम्);
	}

	प्रतिरूपम्<typename P>
	Cursor निवेशय(ध्रुव P &मार्गः_2, T &&मानम्) {
		प्रत्यागम do_निवेशय(this, मार्गः_2.begin(), मार्गः_2.अन्तः(), std::move(मानम्));
	}

	Cursor cursor() {
		प्रत्यागम parent ? Cursor(this->parent, this->scope_name) : Cursor(this, IdString());
	}

	प्रतिरूपम्<typename P>
	Cursor cursor(P begin, P अन्तः) {
		Cursor current = cursor();
		प्रति (; begin != अन्तः; ++begin) {
			current = current.do_child(*begin);
			यदि (!current.valid())
				भङ्ग;
		}
		प्रत्यागम current;
	}

	प्रतिरूपम्<typename P>
	Cursor cursor(ध्रुव P &मार्गः_2) {
		प्रत्यागम cursor(मार्गः_2.begin(), मार्गः_2.अन्तः());
	}
};


संरचना ModuleItem {
	गणना class Type {
		Wire,
		Cell,
	};
	Type type;
	शून्य *ptr;

	ModuleItem(Wire *wire) : type(Type::Wire), ptr(wire) {}
	ModuleItem(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ध्रुव { प्रत्यागम type == Type::Wire; }
	bool is_cell() ध्रुव { प्रत्यागम type == Type::Cell; }

	Wire *wire() ध्रुव { प्रत्यागम type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ध्रुव { प्रत्यागम type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool संक्रियकः==(ध्रुव ModuleItem &other) ध्रुव { प्रत्यागम ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(ptr); प्रत्यागम h; }
};

स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(typename Id_वृक्षः<ModuleItem>::Cursor cursor ) { लघुगणकः("%p ", cursor.target); log_dump_val_कार्यकर्ता(cursor.scope_name); }

प्रतिरूपम्<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(ध्रुव typename std::unique_ptr<T> &cursor ) { लघुगणकः("unique %p", cursor.get()); }

प्रतिरूपम्<typename O>
std::सदिशम्<IdString> parse_hdlname(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::सदिशम्<IdString> मार्गः_2;
	प्रति (स्वचालित ध्रुव &item : object->get_hdlname_attribute())
		मार्गः_2.प्रक्षिप_पृष्ठभागः(twines.सङ्कलय("\\" + item));
	bool synthetic = !name.is_लौकिकम्() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	यदि (मार्गः_2.रिक्तानि() && name.is_लौकिकम्())
		मार्गः_2.प्रक्षिप_पृष्ठभागः(name);
	यदि (!मार्गः_2.रिक्तानि() && synthetic) {
		मार्गः_2.निष्कासय_पृष्ठभागः();
		मार्गः_2.प्रक्षिप_पृष्ठभागः(name);
	}
	प्रत्यागम मार्गः_2;
}

प्रतिरूपम्<typename O>
std::pair<std::सदिशम्<IdString>, IdString> parse_scopename(ध्रुव O* object)
{
	TwinePool &twines = object->design()->twines();
	std::सदिशम्<IdString> मार्गः_2;
	IdString trailing = object->name;
	यदि (trailing.is_लौकिकम्() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		प्रति (स्वचालित ध्रुव &item : object->get_hdlname_attribute())
			मार्गः_2.प्रक्षिप_पृष्ठभागः(twines.सङ्कलय("\\" + item));
		यदि (!मार्गः_2.रिक्तानि()) {
			trailing = मार्गः_2.पृष्ठभागः();
			मार्गः_2.निष्कासय_पृष्ठभागः();
		}
	} अन्यथा यदि (object->has_attribute(ID::hdlname)) {
		प्रति (स्वचालित ध्रुव &item : object->get_hdlname_attribute())
			मार्गः_2.प्रक्षिप_पृष्ठभागः(twines.सङ्कलय("\\" + item));
		यदि (!मार्गः_2.रिक्तानि()) {
			मार्गः_2.निष्कासय_पृष्ठभागः();
		}
	} अन्यथा {
		प्रति (स्वचालित ध्रुव &item : विभज_tokens(object->get_string_attribute(ID(scopename)), " "))
			मार्गः_2.प्रक्षिप_पृष्ठभागः(twines.सङ्कलय("\\" + item));
	}
	प्रत्यागम {मार्गः_2, trailing};
}

संरचना Module_Hdlname_सूचकांक {
	प्रकारनिर्देश Id_वृक्षः<ModuleItem>::Cursor Cursor;

	RTLIL::Module *module;
	Id_वृक्षः<ModuleItem> वृक्षः;
	dict<ModuleItem, Cursor> lookup;

	Module_Hdlname_सूचकांक(RTLIL::Module *module) : module(module) {}

गुप्त:
	प्रतिरूपम्<typename I, typename Filter>
	शून्य सूचकांक_वस्तूनि(I begin, I अन्तः, Filter filter);

सार्वजनिक:
	// Index all wires and cells of the module
	शून्य सूचकांक();

	// Index all wires of the module
	शून्य सूचकांक_wires();

	// Index all cells of the module
	शून्य सूचकांक_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	शून्य सूचकांक_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	प्रतिरूपम्<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		स्वचालित pair = parse_scopename(object);
		प्रत्यागम {वृक्षः.cursor(pair.first), pair.second};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::सदिशम्<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	प्रतिरूपम्<typename O>
	std::सदिशम्<std::string> sources(O *object) {
		स्वचालित pair = parse_scopename(object);
		std::सदिशम्<std::string> परिणाम = scope_sources(वृक्षः.cursor(pair.first));
		परिणाम.प्रक्षिप_पृष्ठभागः(object->get_src_attribute());
		प्रत्यागम परिणाम;
	}
};

गणना class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_गुणाः(ध्रुव RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
