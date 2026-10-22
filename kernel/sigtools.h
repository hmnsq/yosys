/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ifndef SIGTOOLS_H
#define SIGTOOLS_H

#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ഘടന SigPool
{
	ഘടന bitDef_t : പൊതു std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ> {
		bitDef_t() : std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ>(ശൂന്യ, 0) { }
		bitDef_t(സ്ഥിരം RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(first->name);
			h.eat(second);
			തിരികെ h;
		}
	};

	pool<bitDef_t> bits;

	ശൂന്യം clear()
	{
		bits.clear();
	}

	ശൂന്യം add(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits.തിരുകുക(bit);
	}

	ശൂന്യം add(സ്ഥിരം SigPool &other)
	{
		വേണ്ടി (auto &bit : other.bits)
			bits.തിരുകുക(bit);
	}

	ശൂന്യം del(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits.erase(bit);
	}

	ശൂന്യം del(സ്ഥിരം SigPool &other)
	{
		വേണ്ടി (auto &bit : other.bits)
			bits.erase(bit);
	}

	ശൂന്യം expand(സ്ഥിരം RTLIL::SigSpec &from, സ്ഥിരം RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			എങ്കിൽ (bit_from.first != ശൂന്യ && bit_to.first != ശൂന്യ && bits.എണ്ണം(bit_from) > 0)
				bits.തിരുകുക(bit_to);
		}
	}

	RTLIL::SigSpec extract(സ്ഥിരം RTLIL::SigSpec &sig) സ്ഥിരം
	{
		RTLIL::SigSpec result;
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ && bits.എണ്ണം(bit))
				result.append(bit);
		തിരികെ result;
	}

	RTLIL::SigSpec remove(സ്ഥിരം RTLIL::SigSpec &sig) സ്ഥിരം
	{
		RTLIL::SigSpec result;
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ && bits.എണ്ണം(bit) == 0)
				result.append(bit);
		തിരികെ result;
	}

	bool check(സ്ഥിരം RTLIL::SigBit &bit) സ്ഥിരം
	{
		തിരികെ bit.wire != ശൂന്യ && bits.എണ്ണം(bit);
	}

	bool check_any(സ്ഥിരം RTLIL::SigSpec &sig) സ്ഥിരം
	{
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ && bits.എണ്ണം(bit))
				തിരികെ true;
		തിരികെ false;
	}

	bool check_all(സ്ഥിരം RTLIL::SigSpec &sig) സ്ഥിരം
	{
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ && bits.എണ്ണം(bit) == 0)
				തിരികെ false;
		തിരികെ true;
	}

	RTLIL::SigSpec export_one() സ്ഥിരം
	{
		വേണ്ടി (auto &bit : bits)
			തിരികെ RTLIL::SigSpec(bit.first, bit.second);
		തിരികെ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() സ്ഥിരം
	{
		pool<RTLIL::SigBit> sig;
		വേണ്ടി (auto &bit : bits)
			sig.തിരുകുക(RTLIL::SigBit(bit.first, bit.second));
		തിരികെ sig;
	}

	size_t size() സ്ഥിരം
	{
		തിരികെ bits.size();
	}
};

template <typename T, class Compare = ശൂന്യം>
ഘടന SigSet
{
	static_assert(!std::is_same<Compare,ശൂന്യം>::മൂല്യം, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ഘടന bitDef_t : പൊതു std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ> {
		bitDef_t() : std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ>(ശൂന്യ, 0) { }
		bitDef_t(സ്ഥിരം RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, പൂർണ്ണസംഖ്യ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(first->name);
			h.eat(second);
			തിരികെ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	ശൂന്യം clear()
	{
		bits.clear();
	}

	ശൂന്യം തിരുകുക(സ്ഥിരം RTLIL::SigSpec &sig, T data)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits[bit].തിരുകുക(data);
	}

	ശൂന്യം തിരുകുക(സ്ഥിരം RTLIL::SigSpec& sig, സ്ഥിരം std::set<T> &data)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits[bit].തിരുകുക(data.begin(), data.end());
	}

	ശൂന്യം erase(സ്ഥിരം RTLIL::SigSpec& sig)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits[bit].clear();
	}

	ശൂന്യം erase(സ്ഥിരം RTLIL::SigSpec &sig, T data)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits[bit].erase(data);
	}

	ശൂന്യം erase(സ്ഥിരം RTLIL::SigSpec &sig, സ്ഥിരം std::set<T> &data)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				bits[bit].erase(data.begin(), data.end());
	}

	ശൂന്യം find(സ്ഥിരം RTLIL::SigSpec &sig, std::set<T> &result)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ) {
				auto &data = bits[bit];
				result.തിരുകുക(data.begin(), data.end());
			}
	}

	ശൂന്യം find(സ്ഥിരം RTLIL::SigSpec &sig, pool<T> &result)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ) {
				auto &data = bits[bit];
				result.തിരുകുക(data.begin(), data.end());
			}
	}

	std::set<T> find(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		തിരികെ result;
	}

	bool has(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire != ശൂന്യ && bits.എണ്ണം(bit))
				തിരികെ true;
		തിരികെ false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::മൂല്യം>::type> : പൊതു SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::മൂല്യം>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : പൊതു SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

ഘടന SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	ശൂന്യം apply(RTLIL::SigBit &bit) സ്ഥിരം
	{
		bit = database.find(bit);
	}

	ശൂന്യം apply(RTLIL::SigSpec &sig) സ്ഥിരം
	{
		വേണ്ടി (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) സ്ഥിരം
	{
		apply(bit);
		തിരികെ bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) സ്ഥിരം
	{
		apply(sig);
		തിരികെ sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) സ്ഥിരം
	{
		SigSpec sig(wire);
		apply(sig);
		തിരികെ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ഘടന SigMap final : പൊതു SigMapView
{
	SigMap(RTLIL::Module *module = ശൂന്യ)
	{
		എങ്കിൽ (module != ശൂന്യ)
			set(module);
	}

	ശൂന്യം swap(SigMap &other)
	{
		database.swap(other.database);
	}

	ശൂന്യം clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	ശൂന്യം set(RTLIL::Module *module)
	{
		പൂർണ്ണസംഖ്യ bitcount = 0;
		വേണ്ടി (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		വേണ്ടി (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	ശൂന്യം add(സ്ഥിരം RTLIL::SigSpec& from, സ്ഥിരം RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(from); i++)
		{
			പൂർണ്ണസംഖ്യ bfi = database.lookup(from[i]);
			പൂർണ്ണസംഖ്യ bti = database.lookup(to[i]);

			സ്ഥിരം RTLIL::SigBit &bf = database[bfi];
			സ്ഥിരം RTLIL::SigBit &bt = database[bti];

			എങ്കിൽ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				എങ്കിൽ (bf.wire == nullptr)
					database.ipromote(bfi);

				എങ്കിൽ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	ശൂന്യം add(സ്ഥിരം RTLIL::SigBit &bit)
	{
		സ്ഥിരം auto &b = database.find(bit);
		എങ്കിൽ (b.wire != nullptr)
			database.promote(bit);
	}

	ശൂന്യം add(സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			add(bit);
	}

	വരിയിൽ ശൂന്യം add(Wire *wire) { തിരികെ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() സ്ഥിരം
	{
		RTLIL::SigSpec sig;
		വേണ്ടി (സ്ഥിരം auto &bit : database)
			എങ്കിൽ (bit.wire != nullptr)
				sig.append(bit);
		തിരികെ sig;
	}
};

/**
 * SiValgMap wraps a union-find "database" to map SigBits of a module to
 * canonical representative SigBits plus some optional Val value associated with the bits.
 * Val has a commutative, associative, idempotent operator|=, a default constructor
 * which constructs an identity element, and a copy constructor.
 * SigBits that are connected share a set in the underlying database;
 * the associated value is the "sum" of all the values associated with the contributing bits.
 * If any of the SigBits in a set are a constant, the canonical SigBit is a constant.
 */
template <class Val>
ഘടന SigValMap final : പൊതു SigMapView
{
	dict<SigBit, Val> മൂല്യങ്ങൾ;

	ശൂന്യം swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		മൂല്യങ്ങൾ.swap(other.മൂല്യങ്ങൾ);
	}

	ശൂന്യം clear()
	{
		database.clear();
		മൂല്യങ്ങൾ.clear();
	}

	// Rebuild SigMap for all connections in module
	ശൂന്യം set(RTLIL::Module *module)
	{
		പൂർണ്ണസംഖ്യ bitcount = 0;
		വേണ്ടി (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		മൂല്യങ്ങൾ.clear();
		database.reserve(bitcount);

		വേണ്ടി (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	ശൂന്യം add(സ്ഥിരം RTLIL::SigSpec& from, സ്ഥിരം RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(from); i++)
		{
			പൂർണ്ണസംഖ്യ bfi = database.lookup(from[i]);
			പൂർണ്ണസംഖ്യ bti = database.lookup(to[i]);
			എങ്കിൽ (bfi == bti) {
				തുടരുക;
			}

			സ്ഥിരം RTLIL::SigBit &bf = database[bfi];
			സ്ഥിരം RTLIL::SigBit &bt = database[bti];
			എങ്കിൽ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_മൂല്യം(bt, bf);
			} അല്ലെങ്കിൽ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_മൂല്യം(bf, bt);
			}
		}
	}

	ശൂന്യം addVal(സ്ഥിരം RTLIL::SigBit &bit, സ്ഥിരം Val &val)
	{
		മൂല്യങ്ങൾ[database.find(bit)] |= val;
	}

	ശൂന്യം addVal(സ്ഥിരം RTLIL::SigSpec &sig, സ്ഥിരം Val &val)
	{
		വേണ്ടി (സ്ഥിരം auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_മൂല്യം(RTLIL::SigBit &bit) സ്ഥിരം
	{
		bit = database.find(bit);
		auto it = മൂല്യങ്ങൾ.find(bit);
		തിരികെ it == മൂല്യങ്ങൾ.end() ? Val() : it->second;
	}

സ്വകാര്യ:
	ശൂന്യം merge_മൂല്യം(സ്ഥിരം RTLIL::SigBit &from, സ്ഥിരം RTLIL::SigBit &to)
	{
		auto it = മൂല്യങ്ങൾ.find(from);
		എങ്കിൽ (it == മൂല്യങ്ങൾ.end()) {
			തിരികെ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		മൂല്യങ്ങൾ.erase(it);
		മൂല്യങ്ങൾ[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
