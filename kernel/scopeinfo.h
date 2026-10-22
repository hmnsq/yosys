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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCOPEINFO_H
#ವ್ಯಾಖ್ಯಾನಿಸು SCOPEINFO_H

#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <algorithm>

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

ಮಾದರಿ<typename T>
class Id_ವೃಕ್ಷ
{
ಸಾರ್ವಜನಿಕ:
	ರಚನೆ Cursor;

ಸಂರಕ್ಷಿತ:
	Id_ವೃಕ್ಷ *ಪೋಷಕ = nullptr;
	IdString scope_name;
	ಪೂರ್ಣಾಂಕ ಆಳ = 0;

	pool<IdString> ಹೆಸರುಗಳು;
	dict<IdString, T> entries;
ಸಾರ್ವಜನಿಕ: // XXX
	dict<IdString, std::unique_ptr<Id_ವೃಕ್ಷ>> subtrees;

	ಮಾದರಿ<typename P, typename T_ref>
	ಸ್ಥಿರ Cursor do_ಸೇರಿಸು(Id_ವೃಕ್ಷ *ವೃಕ್ಷ, P begin, P ಕೊನೆ, T_ref &&ಮೌಲ್ಯ)
	{
		log_assert(begin != ಕೊನೆ && "path must be non-empty");
		ಆಗ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.ಖಾಲಿಗಳು());
			ವೃಕ್ಷ->ಹೆಸರುಗಳು.ಸೇರಿಸು(name);
			ಆದರೆ (begin == ಕೊನೆ) {
				ವೃಕ್ಷ->entries.emplace(name, std::forward<T_ref>(ಮೌಲ್ಯ));
				ಹಿಂತಿರುಗಿ Cursor(ವೃಕ್ಷ, name);
			}
			ಸ್ವಯಂಚಾಲಿತ &unique = ವೃಕ್ಷ->subtrees[name];
			ಆದರೆ (!unique) {
				unique.reset(new Id_ವೃಕ್ಷ);
				unique->scope_name = name;
				unique->ಪೋಷಕ = ವೃಕ್ಷ;
				unique->ಆಳ = ವೃಕ್ಷ->ಆಳ + 1;
			}
			ವೃಕ್ಷ = unique.get();
		}
	}

ಸಾರ್ವಜನಿಕ:
	Id_ವೃಕ್ಷ() = ಪೂರ್ವನಿಯೋಜಿತ;
	Id_ವೃಕ್ಷ(ಸ್ಥಿರಾಂಕ Id_ವೃಕ್ಷ &) = delete;
	Id_ವೃಕ್ಷ(Id_ವೃಕ್ಷ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ರಚನೆ Cursor
	{
		ಮಿತ್ರ class Id_ವೃಕ್ಷ;
	ಸಂರಕ್ಷಿತ:
	ಸಾರ್ವಜನಿಕ:
		Id_ವೃಕ್ಷ *ಗುರಿ_2;
		IdString scope_name;

		Cursor() : ಗುರಿ_2(nullptr) {}
		Cursor(Id_ವೃಕ್ಷ *ಗುರಿ_2, IdString scope_name) : ಗುರಿ_2(ಗುರಿ_2), scope_name(scope_name) {
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು())
				log_assert(ಗುರಿ_2->ಪೋಷಕ == nullptr);
		}

		Cursor do_ಮೊದಲ_ಮಗು() {
			Id_ವೃಕ್ಷ *ವೃಕ್ಷ = nullptr;
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು()) {
				ವೃಕ್ಷ = ಗುರಿ_2;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ಗುರಿ_2->subtrees.ಕಂಡುಹಿಡಿ(scope_name);
				ಆದರೆ (ಸಿಕ್ಕಿತು != ಗುರಿ_2->subtrees.ಕೊನೆ()) {
					ವೃಕ್ಷ = ಸಿಕ್ಕಿತು->ಎರಡನೇ.get();
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಹಿಂತಿರುಗಿ Cursor();
				}
			}
			ಆದರೆ (ವೃಕ್ಷ->ಹೆಸರುಗಳು.ಖಾಲಿಗಳು()) {
				ಹಿಂತಿರುಗಿ Cursor();
			}
			ಹಿಂತಿರುಗಿ Cursor(ವೃಕ್ಷ, *ವೃಕ್ಷ->ಹೆಸರುಗಳು.begin());
		}

		Cursor do_ಮುಂದಿನ_sibling() {
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ Cursor();
			ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ಗುರಿ_2->ಹೆಸರುಗಳು.ಕಂಡುಹಿಡಿ(scope_name);
			ಆದರೆ (ಸಿಕ್ಕಿತು == ಗುರಿ_2->ಹೆಸರುಗಳು.ಕೊನೆ())
				ಹಿಂತಿರುಗಿ Cursor();
			++ಸಿಕ್ಕಿತು;
			ಆದರೆ (ಸಿಕ್ಕಿತು == ಗುರಿ_2->ಹೆಸರುಗಳು.ಕೊನೆ())
				ಹಿಂತಿರುಗಿ Cursor();
			ಹಿಂತಿರುಗಿ Cursor(ಗುರಿ_2, *ಸಿಕ್ಕಿತು);
		}

		Cursor do_ಪೋಷಕ() {
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ Cursor();
			ಆದರೆ (ಗುರಿ_2->ಪೋಷಕ != nullptr)
				ಹಿಂತಿರುಗಿ Cursor(ಗುರಿ_2->ಪೋಷಕ, ಗುರಿ_2->scope_name);
			ಹಿಂತಿರುಗಿ Cursor(ಗುರಿ_2, IdString());
		}

		Cursor do_ಮುಂದಿನ_ಪೂರ್ವಕ್ರಮ() {
			Cursor current = *this;
			Cursor ಮುಂದಿನ = current.do_ಮೊದಲ_ಮಗು();
			ಆದರೆ (ಮುಂದಿನ.valid())
				ಹಿಂತಿರುಗಿ ಮುಂದಿನ;
			ಆಗ (current.valid()) {
				ಆದರೆ (ಮುಂದಿನ.valid())
					ಹಿಂತಿರುಗಿ ಮುಂದಿನ;
				ಮುಂದಿನ = current.do_ಮುಂದಿನ_sibling();
				ಆದರೆ (ಮುಂದಿನ.valid())
					ಹಿಂತಿರುಗಿ ಮುಂದಿನ;
				current = current.do_ಪೋಷಕ();
			}
			ಹಿಂತಿರುಗಿ current;
		}

		Cursor do_ಮಗು(IdString name) {
			Id_ವೃಕ್ಷ *ವೃಕ್ಷ = nullptr;
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು()) {
				ವೃಕ್ಷ = ಗುರಿ_2;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ಗುರಿ_2->subtrees.ಕಂಡುಹಿಡಿ(scope_name);
				ಆದರೆ (ಸಿಕ್ಕಿತು != ಗುರಿ_2->subtrees.ಕೊನೆ()) {
					ವೃಕ್ಷ = ಸಿಕ್ಕಿತು->ಎರಡನೇ.get();
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಹಿಂತಿರುಗಿ Cursor();
				}
			}
			ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ವೃಕ್ಷ->ಹೆಸರುಗಳು.ಕಂಡುಹಿಡಿ(name);
			ಆದರೆ (ಸಿಕ್ಕಿತು == ವೃಕ್ಷ->ಹೆಸರುಗಳು.ಕೊನೆ()) {
				ಹಿಂತಿರುಗಿ Cursor();
			}
			ಹಿಂತಿರುಗಿ Cursor(ವೃಕ್ಷ, *ಸಿಕ್ಕಿತು);
		}

	ಸಾರ್ವಜನಿಕ:
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Cursor &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಗುರಿ_2 == other.ಗುರಿ_2 && scope_name == other.scope_name;
		}
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Cursor &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
		{
			h.eat(scope_name);
			h.eat(ಗುರಿ_2);
			ಹಿಂತಿರುಗಿ h;
		}

		bool valid() ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಗುರಿ_2 != nullptr;
		}

		ಪೂರ್ಣಾಂಕ ಆಳ() ಸ್ಥಿರಾಂಕ {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ ಗುರಿ_2->ಆಳ + !scope_name.ಖಾಲಿಗಳು();
		}

		bool is_ಬೇರು() ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ಗುರಿ_2 != nullptr && scope_name.ಖಾಲಿಗಳು();
		}

		bool has_ನಮೂದು() ಸ್ಥಿರಾಂಕ {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ !scope_name.ಖಾಲಿಗಳು() && ಗುರಿ_2->entries.ಎಣಿಕೆ_2(scope_name);
		}

		T &ನಮೂದು() {
			log_assert(!scope_name.ಖಾಲಿಗಳು());
			ಹಿಂತಿರುಗಿ ಗುರಿ_2->entries.at(scope_name);
		}

		ಶೂನ್ಯ ನಿಯೋಜಿಸು_ಮಾರ್ಗ_ಗಮ್ಯ(std::ಸದಿಶ<IdString> &ಹೊರಹೋಗು_ಮಾರ್ಗ) {
			log_assert(valid());
			ಹೊರಹೋಗು_ಮಾರ್ಗ.clear();
			ಆದರೆ (scope_name.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ;
			ಹೊರಹೋಗು_ಮಾರ್ಗ.ತಳ್ಳು_ಹಿಂಭಾಗ(scope_name);
			Id_ವೃಕ್ಷ *current = ಗುರಿ_2;
			ಆಗ (current->ಪೋಷಕ) {
				ಹೊರಹೋಗು_ಮಾರ್ಗ.ತಳ್ಳು_ಹಿಂಭಾಗ(current->scope_name);
				current = current->ಪೋಷಕ;
			}
			std::ಹಿಮ್ಮುಖ(ಹೊರಹೋಗು_ಮಾರ್ಗ.begin(), ಹೊರಹೋಗು_ಮಾರ್ಗ.ಕೊನೆ());
		}

		std::ಸದಿಶ<IdString> ಮಾರ್ಗ_2() {
			std::ಸದಿಶ<IdString> ಫಲಿತಾಂಶ;
			ನಿಯೋಜಿಸು_ಮಾರ್ಗ_ಗಮ್ಯ(ಫಲಿತಾಂಶ);
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}

		std::string ಮಾರ್ಗ_str(ಸ್ಥಿರಾಂಕ RTLIL::Design *design) {
			std::string ಫಲಿತಾಂಶ;
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : ಮಾರ್ಗ_2()) {
				ಆದರೆ (!ಫಲಿತಾಂಶ.ಖಾಲಿಗಳು())
					ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(' ');
				ಫಲಿತಾಂಶ += design->twines().unescaped_str(ಅಂಶ);
			}
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}

		Cursor ಮೊದಲ_ಮಗು() {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ do_ಮೊದಲ_ಮಗು();
		}

		Cursor ಮುಂದಿನ_ಪೂರ್ವಕ್ರಮ() {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ do_ಮುಂದಿನ_ಪೂರ್ವಕ್ರಮ();
		}

		Cursor ಪೋಷಕ() {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ do_ಪೋಷಕ();
		}

		Cursor ಮಗು(IdString name) {
			log_assert(valid());
			ಹಿಂತಿರುಗಿ do_ಮಗು(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ಆಗ (current != other) {
				ಆದರೆ (!current.valid() || !other.valid())
					ಹಿಂತಿರುಗಿ Cursor();
				ಪೂರ್ಣಾಂಕ delta = current.ಆಳ() - other.ಆಳ();
				ಆದರೆ (delta >= 0)
					current = current.do_ಪೋಷಕ();
				ಆದರೆ (delta <= 0)
					other = other.do_ಪೋಷಕ();
			}
			ಹಿಂತಿರುಗಿ current;
		}
	};

	ಮಾದರಿ<typename P>
	Cursor ಸೇರಿಸು(P begin, P ಕೊನೆ, ಸ್ಥಿರಾಂಕ T &ಮೌಲ್ಯ) {
		ಹಿಂತಿರುಗಿ do_ಸೇರಿಸು(this, begin, ಕೊನೆ, ಮೌಲ್ಯ);
	}

	ಮಾದರಿ<typename P>
	Cursor ಸೇರಿಸು(P begin, P ಕೊನೆ, T &&ಮೌಲ್ಯ) {
		ಹಿಂತಿರುಗಿ do_ಸೇರಿಸು(this, begin, ಕೊನೆ, std::move(ಮೌಲ್ಯ));
	}

	ಮಾದರಿ<typename P>
	Cursor ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ P &ಮಾರ್ಗ_2, ಸ್ಥಿರಾಂಕ T &ಮೌಲ್ಯ) {
		ಹಿಂತಿರುಗಿ do_ಸೇರಿಸು(this, ಮಾರ್ಗ_2.begin(), ಮಾರ್ಗ_2.ಕೊನೆ(), ಮೌಲ್ಯ);
	}

	ಮಾದರಿ<typename P>
	Cursor ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ P &ಮಾರ್ಗ_2, T &&ಮೌಲ್ಯ) {
		ಹಿಂತಿರುಗಿ do_ಸೇರಿಸು(this, ಮಾರ್ಗ_2.begin(), ಮಾರ್ಗ_2.ಕೊನೆ(), std::move(ಮೌಲ್ಯ));
	}

	Cursor cursor() {
		ಹಿಂತಿರುಗಿ ಪೋಷಕ ? Cursor(this->ಪೋಷಕ, this->scope_name) : Cursor(this, IdString());
	}

	ಮಾದರಿ<typename P>
	Cursor cursor(P begin, P ಕೊನೆ) {
		Cursor current = cursor();
		ಗಾಗಿ (; begin != ಕೊನೆ; ++begin) {
			current = current.do_ಮಗು(*begin);
			ಆದರೆ (!current.valid())
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ current;
	}

	ಮಾದರಿ<typename P>
	Cursor cursor(ಸ್ಥಿರಾಂಕ P &ಮಾರ್ಗ_2) {
		ಹಿಂತಿರುಗಿ cursor(ಮಾರ್ಗ_2.begin(), ಮಾರ್ಗ_2.ಕೊನೆ());
	}
};


ರಚನೆ Module_ಅಂಶ {
	ಎಣಿಕೆ class Type {
		Wire,
		Cell,
	};
	Type type;
	ಶೂನ್ಯ *ptr;

	Module_ಅಂಶ(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_ಅಂಶ(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type == Type::Wire; }
	bool is_cell() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type == Type::Cell; }

	Wire *wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Module_ಅಂಶ &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(ptr); ಹಿಂತಿರುಗಿ h; }
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(typename Id_ವೃಕ್ಷ<Module_ಅಂಶ>::Cursor cursor ) { ಲಘುಗಣಕ("%p ", cursor.ಗುರಿ_2); log_dump_val_ಕೆಲಸಗಾರ(cursor.scope_name); }

ಮಾದರಿ<typename T>
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ typename std::unique_ptr<T> &cursor ) { ಲಘುಗಣಕ("unique %p", cursor.get()); }

ಮಾದರಿ<typename O>
std::ಸದಿಶ<IdString> parse_hdlname(ಸ್ಥಿರಾಂಕ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::ಸದಿಶ<IdString> ಮಾರ್ಗ_2;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಶ : object->get_hdlname_attribute())
		ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(twines.ಕೂಡಿಸು("\\" + ಅಂಶ));
	bool synthetic = !name.is_ಬಹಿರಂಗ() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ಆದರೆ (ಮಾರ್ಗ_2.ಖಾಲಿಗಳು() && name.is_ಬಹಿರಂಗ())
		ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(name);
	ಆದರೆ (!ಮಾರ್ಗ_2.ಖಾಲಿಗಳು() && synthetic) {
		ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
		ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(name);
	}
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_2;
}

ಮಾದರಿ<typename O>
std::pair<std::ಸದಿಶ<IdString>, IdString> parse_scopename(ಸ್ಥಿರಾಂಕ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::ಸದಿಶ<IdString> ಮಾರ್ಗ_2;
	IdString trailing = object->name;
	ಆದರೆ (trailing.is_ಬಹಿರಂಗ() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಶ : object->get_hdlname_attribute())
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(twines.ಕೂಡಿಸು("\\" + ಅಂಶ));
		ಆದರೆ (!ಮಾರ್ಗ_2.ಖಾಲಿಗಳು()) {
			trailing = ಮಾರ್ಗ_2.ಹಿಂಭಾಗ();
			ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (object->has_attribute(ID::hdlname)) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಶ : object->get_hdlname_attribute())
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(twines.ಕೂಡಿಸು("\\" + ಅಂಶ));
		ಆದರೆ (!ಮಾರ್ಗ_2.ಖಾಲಿಗಳು()) {
			ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಶ : ವಿಭಜಿಸು_ಟೋಕನುಗಳು(object->get_string_attribute(ID(scopename)), " "))
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(twines.ಕೂಡಿಸು("\\" + ಅಂಶ));
	}
	ಹಿಂತಿರುಗಿ {ಮಾರ್ಗ_2, trailing};
}

ರಚನೆ Module_Hdlname_ಸೂಚ್ಯಂಕ {
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Id_ವೃಕ್ಷ<Module_ಅಂಶ>::Cursor Cursor;

	RTLIL::Module *module;
	Id_ವೃಕ್ಷ<Module_ಅಂಶ> ವೃಕ್ಷ;
	dict<Module_ಅಂಶ, Cursor> lookup;

	Module_Hdlname_ಸೂಚ್ಯಂಕ(RTLIL::Module *module) : module(module) {}

ಖಾಸಗಿ:
	ಮಾದರಿ<typename I, typename Filter>
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_ವಸ್ತುಗಳು(I begin, I ಕೊನೆ, Filter filter);

ಸಾರ್ವಜನಿಕ:
	// Index all wires and cells of the module
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ();

	// Index all wires of the module
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_wires();

	// Index all cells of the module
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	ಮಾದರಿ<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		ಸ್ವಯಂಚಾಲಿತ pair = parse_scopename(object);
		ಹಿಂತಿರುಗಿ {ವೃಕ್ಷ.cursor(pair.ಮೊದಲ), pair.ಎರಡನೇ};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::ಸದಿಶ<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	ಮಾದರಿ<typename O>
	std::ಸದಿಶ<std::string> sources(O *object) {
		ಸ್ವಯಂಚಾಲಿತ pair = parse_scopename(object);
		std::ಸದಿಶ<std::string> ಫಲಿತಾಂಶ = scope_sources(ವೃಕ್ಷ.cursor(pair.ಮೊದಲ));
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(object->get_src_attribute());
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}
};

ಎಣಿಕೆ class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ಗುಣಲಕ್ಷಣಗಳು(ಸ್ಥಿರಾಂಕ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
