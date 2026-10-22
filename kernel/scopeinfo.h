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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCOPEINFO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCOPEINFO_H

#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/celltypes.h"

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ<typename T>
class Id_ਰੁੱਖ
{
ਜਨਤਕ:
	ਬਣਤਰ Cursor;

ਸੁਰੱਖਿਅਤ:
	Id_ਰੁੱਖ *ਮਾਪਾ = nullptr;
	IdString scope_name;
	ਪੂਰਨ ਡੂੰਘਾਈ = 0;

	pool<IdString> ਨਾਂ;
	dict<IdString, T> entries;
ਜਨਤਕ: // XXX
	dict<IdString, std::unique_ptr<Id_ਰੁੱਖ>> subtrees;

	ਸਾਂਚਾ<typename P, typename T_ref>
	ਸਥਿਰ Cursor do_ਪਾਓ(Id_ਰੁੱਖ *ਰੁੱਖ, P begin, P ਸਿਰਾ, T_ref &&ਮੁੱਲ)
	{
		log_assert(begin != ਸਿਰਾ && "path must be non-empty");
		ਜਦੋਂ (true) {
			IdString name = *begin;
			++begin;
			log_assert(!name.ਖਾਲੀ_ਥਾਵਾਂ());
			ਰੁੱਖ->ਨਾਂ.ਪਾਓ(name);
			ਜੇ (begin == ਸਿਰਾ) {
				ਰੁੱਖ->entries.emplace(name, std::forward<T_ref>(ਮੁੱਲ));
				ਵਾਪਸ Cursor(ਰੁੱਖ, name);
			}
			ਸਵੈਚਾਲਿਤ &unique = ਰੁੱਖ->subtrees[name];
			ਜੇ (!unique) {
				unique.reset(new Id_ਰੁੱਖ);
				unique->scope_name = name;
				unique->ਮਾਪਾ = ਰੁੱਖ;
				unique->ਡੂੰਘਾਈ = ਰੁੱਖ->ਡੂੰਘਾਈ + 1;
			}
			ਰੁੱਖ = unique.get();
		}
	}

ਜਨਤਕ:
	Id_ਰੁੱਖ() = ਮੂਲ;
	Id_ਰੁੱਖ(ਅਚਲ Id_ਰੁੱਖ &) = delete;
	Id_ਰੁੱਖ(Id_ਰੁੱਖ &&) = delete;

	// A cursor remains valid as long as the (sub-)IdTree it points at is alive
	ਬਣਤਰ Cursor
	{
		ਮਿੱਤਰ class Id_ਰੁੱਖ;
	ਸੁਰੱਖਿਅਤ:
	ਜਨਤਕ:
		Id_ਰੁੱਖ *ਨਿਸ਼ਾਨਾ_2;
		IdString scope_name;

		Cursor() : ਨਿਸ਼ਾਨਾ_2(nullptr) {}
		Cursor(Id_ਰੁੱਖ *ਨਿਸ਼ਾਨਾ_2, IdString scope_name) : ਨਿਸ਼ਾਨਾ_2(ਨਿਸ਼ਾਨਾ_2), scope_name(scope_name) {
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ())
				log_assert(ਨਿਸ਼ਾਨਾ_2->ਮਾਪਾ == nullptr);
		}

		Cursor do_ਪਹਿਲਾ_ਬੱਚਾ() {
			Id_ਰੁੱਖ *ਰੁੱਖ = nullptr;
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਰੁੱਖ = ਨਿਸ਼ਾਨਾ_2;
			} ਨਹੀਂਤਾਂ {
				ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਨਿਸ਼ਾਨਾ_2->subtrees.ਲੱਭੋ(scope_name);
				ਜੇ (ਮਿਲਿਆ != ਨਿਸ਼ਾਨਾ_2->subtrees.ਸਿਰਾ()) {
					ਰੁੱਖ = ਮਿਲਿਆ->ਦੂਜਾ.get();
				} ਨਹੀਂਤਾਂ {
					ਵਾਪਸ Cursor();
				}
			}
			ਜੇ (ਰੁੱਖ->ਨਾਂ.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਵਾਪਸ Cursor();
			}
			ਵਾਪਸ Cursor(ਰੁੱਖ, *ਰੁੱਖ->ਨਾਂ.begin());
		}

		Cursor do_ਅਗਲਾ_sibling() {
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ Cursor();
			ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਨਿਸ਼ਾਨਾ_2->ਨਾਂ.ਲੱਭੋ(scope_name);
			ਜੇ (ਮਿਲਿਆ == ਨਿਸ਼ਾਨਾ_2->ਨਾਂ.ਸਿਰਾ())
				ਵਾਪਸ Cursor();
			++ਮਿਲਿਆ;
			ਜੇ (ਮਿਲਿਆ == ਨਿਸ਼ਾਨਾ_2->ਨਾਂ.ਸਿਰਾ())
				ਵਾਪਸ Cursor();
			ਵਾਪਸ Cursor(ਨਿਸ਼ਾਨਾ_2, *ਮਿਲਿਆ);
		}

		Cursor do_ਮਾਪਾ() {
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ Cursor();
			ਜੇ (ਨਿਸ਼ਾਨਾ_2->ਮਾਪਾ != nullptr)
				ਵਾਪਸ Cursor(ਨਿਸ਼ਾਨਾ_2->ਮਾਪਾ, ਨਿਸ਼ਾਨਾ_2->scope_name);
			ਵਾਪਸ Cursor(ਨਿਸ਼ਾਨਾ_2, IdString());
		}

		Cursor do_ਅਗਲਾ_ਪੂਰਵ_ਕ੍ਰਮ() {
			Cursor current = *this;
			Cursor ਅਗਲਾ = current.do_ਪਹਿਲਾ_ਬੱਚਾ();
			ਜੇ (ਅਗਲਾ.valid())
				ਵਾਪਸ ਅਗਲਾ;
			ਜਦੋਂ (current.valid()) {
				ਜੇ (ਅਗਲਾ.valid())
					ਵਾਪਸ ਅਗਲਾ;
				ਅਗਲਾ = current.do_ਅਗਲਾ_sibling();
				ਜੇ (ਅਗਲਾ.valid())
					ਵਾਪਸ ਅਗਲਾ;
				current = current.do_ਮਾਪਾ();
			}
			ਵਾਪਸ current;
		}

		Cursor do_ਬੱਚਾ(IdString name) {
			Id_ਰੁੱਖ *ਰੁੱਖ = nullptr;
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਰੁੱਖ = ਨਿਸ਼ਾਨਾ_2;
			} ਨਹੀਂਤਾਂ {
				ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਨਿਸ਼ਾਨਾ_2->subtrees.ਲੱਭੋ(scope_name);
				ਜੇ (ਮਿਲਿਆ != ਨਿਸ਼ਾਨਾ_2->subtrees.ਸਿਰਾ()) {
					ਰੁੱਖ = ਮਿਲਿਆ->ਦੂਜਾ.get();
				} ਨਹੀਂਤਾਂ {
					ਵਾਪਸ Cursor();
				}
			}
			ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਰੁੱਖ->ਨਾਂ.ਲੱਭੋ(name);
			ਜੇ (ਮਿਲਿਆ == ਰੁੱਖ->ਨਾਂ.ਸਿਰਾ()) {
				ਵਾਪਸ Cursor();
			}
			ਵਾਪਸ Cursor(ਰੁੱਖ, *ਮਿਲਿਆ);
		}

	ਜਨਤਕ:
		bool ਸੰਚਾਲਕ==(ਅਚਲ Cursor &other) ਅਚਲ {
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ_2 == other.ਨਿਸ਼ਾਨਾ_2 && scope_name == other.scope_name;
		}
		bool ਸੰਚਾਲਕ!=(ਅਚਲ Cursor &other) ਅਚਲ {
			ਵਾਪਸ !(*this == other);
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ
		{
			h.eat(scope_name);
			h.eat(ਨਿਸ਼ਾਨਾ_2);
			ਵਾਪਸ h;
		}

		bool valid() ਅਚਲ {
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ_2 != nullptr;
		}

		ਪੂਰਨ ਡੂੰਘਾਈ() ਅਚਲ {
			log_assert(valid());
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ_2->ਡੂੰਘਾਈ + !scope_name.ਖਾਲੀ_ਥਾਵਾਂ();
		}

		bool is_ਜੜ੍ਹ() ਅਚਲ {
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ_2 != nullptr && scope_name.ਖਾਲੀ_ਥਾਵਾਂ();
		}

		bool has_ਇੰਦਰਾਜ() ਅਚਲ {
			log_assert(valid());
			ਵਾਪਸ !scope_name.ਖਾਲੀ_ਥਾਵਾਂ() && ਨਿਸ਼ਾਨਾ_2->entries.ਗਿਣਤੀ_2(scope_name);
		}

		T &ਇੰਦਰਾਜ() {
			log_assert(!scope_name.ਖਾਲੀ_ਥਾਵਾਂ());
			ਵਾਪਸ ਨਿਸ਼ਾਨਾ_2->entries.at(scope_name);
		}

		ਖਾਲੀ ਸੌਂਪੋ_ਰਸਤਾ_ਮੰਜ਼ਿਲ(std::ਸਦਿਸ਼<IdString> &ਬਾਹਰਲਾ_ਰਸਤਾ) {
			log_assert(valid());
			ਬਾਹਰਲਾ_ਰਸਤਾ.clear();
			ਜੇ (scope_name.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ;
			ਬਾਹਰਲਾ_ਰਸਤਾ.ਧੱਕੋ_ਪਿੱਠ(scope_name);
			Id_ਰੁੱਖ *current = ਨਿਸ਼ਾਨਾ_2;
			ਜਦੋਂ (current->ਮਾਪਾ) {
				ਬਾਹਰਲਾ_ਰਸਤਾ.ਧੱਕੋ_ਪਿੱਠ(current->scope_name);
				current = current->ਮਾਪਾ;
			}
			std::ਉਲਟ(ਬਾਹਰਲਾ_ਰਸਤਾ.begin(), ਬਾਹਰਲਾ_ਰਸਤਾ.ਸਿਰਾ());
		}

		std::ਸਦਿਸ਼<IdString> ਰਸਤਾ_2() {
			std::ਸਦਿਸ਼<IdString> ਨਤੀਜਾ;
			ਸੌਂਪੋ_ਰਸਤਾ_ਮੰਜ਼ਿਲ(ਨਤੀਜਾ);
			ਵਾਪਸ ਨਤੀਜਾ;
		}

		std::string ਰਸਤਾ_str(ਅਚਲ RTLIL::Design *design) {
			std::string ਨਤੀਜਾ;
			ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : ਰਸਤਾ_2()) {
				ਜੇ (!ਨਤੀਜਾ.ਖਾਲੀ_ਥਾਵਾਂ())
					ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(' ');
				ਨਤੀਜਾ += design->twines().unescaped_str(ਵਸਤੂ_2);
			}
			ਵਾਪਸ ਨਤੀਜਾ;
		}

		Cursor ਪਹਿਲਾ_ਬੱਚਾ() {
			log_assert(valid());
			ਵਾਪਸ do_ਪਹਿਲਾ_ਬੱਚਾ();
		}

		Cursor ਅਗਲਾ_ਪੂਰਵ_ਕ੍ਰਮ() {
			log_assert(valid());
			ਵਾਪਸ do_ਅਗਲਾ_ਪੂਰਵ_ਕ੍ਰਮ();
		}

		Cursor ਮਾਪਾ() {
			log_assert(valid());
			ਵਾਪਸ do_ਮਾਪਾ();
		}

		Cursor ਬੱਚਾ(IdString name) {
			log_assert(valid());
			ਵਾਪਸ do_ਬੱਚਾ(name);
		}

		Cursor common_ancestor(Cursor other) {
			Cursor current = *this;

			ਜਦੋਂ (current != other) {
				ਜੇ (!current.valid() || !other.valid())
					ਵਾਪਸ Cursor();
				ਪੂਰਨ delta = current.ਡੂੰਘਾਈ() - other.ਡੂੰਘਾਈ();
				ਜੇ (delta >= 0)
					current = current.do_ਮਾਪਾ();
				ਜੇ (delta <= 0)
					other = other.do_ਮਾਪਾ();
			}
			ਵਾਪਸ current;
		}
	};

	ਸਾਂਚਾ<typename P>
	Cursor ਪਾਓ(P begin, P ਸਿਰਾ, ਅਚਲ T &ਮੁੱਲ) {
		ਵਾਪਸ do_ਪਾਓ(this, begin, ਸਿਰਾ, ਮੁੱਲ);
	}

	ਸਾਂਚਾ<typename P>
	Cursor ਪਾਓ(P begin, P ਸਿਰਾ, T &&ਮੁੱਲ) {
		ਵਾਪਸ do_ਪਾਓ(this, begin, ਸਿਰਾ, std::move(ਮੁੱਲ));
	}

	ਸਾਂਚਾ<typename P>
	Cursor ਪਾਓ(ਅਚਲ P &ਰਸਤਾ_2, ਅਚਲ T &ਮੁੱਲ) {
		ਵਾਪਸ do_ਪਾਓ(this, ਰਸਤਾ_2.begin(), ਰਸਤਾ_2.ਸਿਰਾ(), ਮੁੱਲ);
	}

	ਸਾਂਚਾ<typename P>
	Cursor ਪਾਓ(ਅਚਲ P &ਰਸਤਾ_2, T &&ਮੁੱਲ) {
		ਵਾਪਸ do_ਪਾਓ(this, ਰਸਤਾ_2.begin(), ਰਸਤਾ_2.ਸਿਰਾ(), std::move(ਮੁੱਲ));
	}

	Cursor cursor() {
		ਵਾਪਸ ਮਾਪਾ ? Cursor(this->ਮਾਪਾ, this->scope_name) : Cursor(this, IdString());
	}

	ਸਾਂਚਾ<typename P>
	Cursor cursor(P begin, P ਸਿਰਾ) {
		Cursor current = cursor();
		ਲਈ (; begin != ਸਿਰਾ; ++begin) {
			current = current.do_ਬੱਚਾ(*begin);
			ਜੇ (!current.valid())
				ਰੋਕੋ;
		}
		ਵਾਪਸ current;
	}

	ਸਾਂਚਾ<typename P>
	Cursor cursor(ਅਚਲ P &ਰਸਤਾ_2) {
		ਵਾਪਸ cursor(ਰਸਤਾ_2.begin(), ਰਸਤਾ_2.ਸਿਰਾ());
	}
};


ਬਣਤਰ Module_ਵਸਤੂ {
	ਗਿਣਤੀ class Type {
		Wire,
		Cell,
	};
	Type type;
	ਖਾਲੀ *ptr;

	Module_ਵਸਤੂ(Wire *wire) : type(Type::Wire), ptr(wire) {}
	Module_ਵਸਤੂ(Cell *cell) : type(Type::Cell), ptr(cell) {}

	bool is_wire() ਅਚਲ { ਵਾਪਸ type == Type::Wire; }
	bool is_cell() ਅਚਲ { ਵਾਪਸ type == Type::Cell; }

	Wire *wire() ਅਚਲ { ਵਾਪਸ type == Type::Wire ? static_cast<Wire *>(ptr) : nullptr; }
	Cell *cell() ਅਚਲ { ਵਾਪਸ type == Type::Cell ? static_cast<Cell *>(ptr) : nullptr; }

	bool ਸੰਚਾਲਕ==(ਅਚਲ Module_ਵਸਤੂ &other) ਅਚਲ { ਵਾਪਸ ptr == other.ptr && type == other.type; }
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(ptr); ਵਾਪਸ h; }
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(typename Id_ਰੁੱਖ<Module_ਵਸਤੂ>::Cursor cursor ) { ਲਘੂਗਣਕ("%p ", cursor.ਨਿਸ਼ਾਨਾ_2); log_dump_val_ਕਾਮਾ(cursor.scope_name); }

ਸਾਂਚਾ<typename T>
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਅਚਲ typename std::unique_ptr<T> &cursor ) { ਲਘੂਗਣਕ("unique %p", cursor.get()); }

ਸਾਂਚਾ<typename O>
std::ਸਦਿਸ਼<IdString> parse_hdlname(ਅਚਲ O* object)
{
	TwinePool &twines = object->design()->twines();
	IdString name = object->name;
	std::ਸਦਿਸ਼<IdString> ਰਸਤਾ_2;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਵਸਤੂ_2 : object->get_hdlname_attribute())
		ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(twines.ਜੋੜੋ("\\" + ਵਸਤੂ_2));
	bool synthetic = !name.is_ਜਨਤਕ() && !object->name.begins_with("$paramod")
			&& !object->name.begins_with("$abstract");
	ਜੇ (ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ() && name.is_ਜਨਤਕ())
		ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(name);
	ਜੇ (!ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ() && synthetic) {
		ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
		ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(name);
	}
	ਵਾਪਸ ਰਸਤਾ_2;
}

ਸਾਂਚਾ<typename O>
std::pair<std::ਸਦਿਸ਼<IdString>, IdString> parse_scopename(ਅਚਲ O* object)
{
	TwinePool &twines = object->design()->twines();
	std::ਸਦਿਸ਼<IdString> ਰਸਤਾ_2;
	IdString trailing = object->name;
	ਜੇ (trailing.is_ਜਨਤਕ() || object->name.begins_with("$paramod") || object->name.begins_with("$abstract")) {
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਵਸਤੂ_2 : object->get_hdlname_attribute())
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(twines.ਜੋੜੋ("\\" + ਵਸਤੂ_2));
		ਜੇ (!ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ()) {
			trailing = ਰਸਤਾ_2.ਪਿੱਠ();
			ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
		}
	} ਨਹੀਂਤਾਂ ਜੇ (object->has_attribute(ID::hdlname)) {
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਵਸਤੂ_2 : object->get_hdlname_attribute())
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(twines.ਜੋੜੋ("\\" + ਵਸਤੂ_2));
		ਜੇ (!ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ()) {
			ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
		}
	} ਨਹੀਂਤਾਂ {
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਵਸਤੂ_2 : ਵੰਡੋ_ਟੋਕਨ(object->get_string_attribute(ID(scopename)), " "))
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(twines.ਜੋੜੋ("\\" + ਵਸਤੂ_2));
	}
	ਵਾਪਸ {ਰਸਤਾ_2, trailing};
}

ਬਣਤਰ Module_Hdlname_ਸੂਚਕਾਂਕ {
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ Id_ਰੁੱਖ<Module_ਵਸਤੂ>::Cursor Cursor;

	RTLIL::Module *module;
	Id_ਰੁੱਖ<Module_ਵਸਤੂ> ਰੁੱਖ;
	dict<Module_ਵਸਤੂ, Cursor> lookup;

	Module_Hdlname_ਸੂਚਕਾਂਕ(RTLIL::Module *module) : module(module) {}

ਨਿਜੀ:
	ਸਾਂਚਾ<typename I, typename Filter>
	ਖਾਲੀ ਸੂਚਕਾਂਕ_ਚੀਜ਼ਾਂ(I begin, I ਸਿਰਾ, Filter filter);

ਜਨਤਕ:
	// Index all wires and cells of the module
	ਖਾਲੀ ਸੂਚਕਾਂਕ();

	// Index all wires of the module
	ਖਾਲੀ ਸੂਚਕਾਂਕ_wires();

	// Index all cells of the module
	ਖਾਲੀ ਸੂਚਕਾਂਕ_cells();

	// Index only the $scopeinfo cells of the module.
	// This is sufficient when using `containing_scope`.
	ਖਾਲੀ ਸੂਚਕਾਂਕ_scopeinfo_cells();


	// Return the cursor for the containing scope of some RTLIL object (Wire/Cell/...)
	ਸਾਂਚਾ<typename O>
	std::pair<Cursor, IdString> containing_scope(O *object) {
		ਸਵੈਚਾਲਿਤ pair = parse_scopename(object);
		ਵਾਪਸ {ਰੁੱਖ.cursor(pair.ਪਹਿਲਾ), pair.ਦੂਜਾ};
	}

	// Return a vector of source locations starting from the indexed module to
	// the scope represented by the cursor. The vector alternates module and
	// module item source locations, using empty strings for missing src
	// attributes.
	std::ਸਦਿਸ਼<std::string> scope_sources(Cursor cursor);

	// Return a vector of source locations starting from the indexed module to
	// the passed RTLIL object (Wire/Cell/...). The vector alternates module
	// and module item source locations, using empty strings for missing src
	// attributes.
	ਸਾਂਚਾ<typename O>
	std::ਸਦਿਸ਼<std::string> sources(O *object) {
		ਸਵੈਚਾਲਿਤ pair = parse_scopename(object);
		std::ਸਦਿਸ਼<std::string> ਨਤੀਜਾ = scope_sources(ਰੁੱਖ.cursor(pair.ਪਹਿਲਾ));
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(object->get_src_attribute());
		ਵਾਪਸ ਨਤੀਜਾ;
	}
};

ਗਿਣਤੀ class ScopeinfoAttrs {
	Module,
	Cell,
};

// Check whether the flattened module or flattened cell corresponding to a $scopeinfo cell had a specific attribute.
bool scopeinfo_has_attribute(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get a specific attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
RTLIL::Const scopeinfo_get_attribute(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs, RTLIL::IdString id);

// Get all attribute from the flattened module or flattened cell corresponding to a $scopeinfo cell.
dict<RTLIL::IdString, RTLIL::Const> scopeinfo_ਗੁਣ(ਅਚਲ RTLIL::Cell *scopeinfo, ScopeinfoAttrs attrs);

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
