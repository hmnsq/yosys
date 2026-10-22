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

#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

સંરચના SigPool
{
	સંરચના bitDef_t : જાહેર std::pair<RTLIL::Wire*, પૂર્ણાંક> {
		bitDef_t() : std::pair<RTLIL::Wire*, પૂર્ણાંક>(ખાલી, 0) { }
		bitDef_t(અચલ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, પૂર્ણાંક>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(first->name);
			h.eat(second);
			પાછા h;
		}
	};

	pool<bitDef_t> bits;

	શૂન્ય clear()
	{
		bits.clear();
	}

	શૂન્ય add(અચલ RTLIL::SigSpec &sig)
	{
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits.ઉમેરો(bit);
	}

	શૂન્ય add(અચલ SigPool &other)
	{
		માટે (auto &bit : other.bits)
			bits.ઉમેરો(bit);
	}

	શૂન્ય del(અચલ RTLIL::SigSpec &sig)
	{
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits.erase(bit);
	}

	શૂન્ય del(અચલ SigPool &other)
	{
		માટે (auto &bit : other.bits)
			bits.erase(bit);
	}

	શૂન્ય expand(અચલ RTLIL::SigSpec &from, અચલ RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		માટે (પૂર્ણાંક i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			જો (bit_from.first != ખાલી && bit_to.first != ખાલી && bits.ગણતરી(bit_from) > 0)
				bits.ઉમેરો(bit_to);
		}
	}

	RTLIL::SigSpec extract(અચલ RTLIL::SigSpec &sig) અચલ
	{
		RTLIL::SigSpec result;
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી && bits.ગણતરી(bit))
				result.append(bit);
		પાછા result;
	}

	RTLIL::SigSpec remove(અચલ RTLIL::SigSpec &sig) અચલ
	{
		RTLIL::SigSpec result;
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી && bits.ગણતરી(bit) == 0)
				result.append(bit);
		પાછા result;
	}

	bool check(અચલ RTLIL::SigBit &bit) અચલ
	{
		પાછા bit.wire != ખાલી && bits.ગણતરી(bit);
	}

	bool check_any(અચલ RTLIL::SigSpec &sig) અચલ
	{
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી && bits.ગણતરી(bit))
				પાછા true;
		પાછા false;
	}

	bool check_all(અચલ RTLIL::SigSpec &sig) અચલ
	{
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી && bits.ગણતરી(bit) == 0)
				પાછા false;
		પાછા true;
	}

	RTLIL::SigSpec export_one() અચલ
	{
		માટે (auto &bit : bits)
			પાછા RTLIL::SigSpec(bit.first, bit.second);
		પાછા RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() અચલ
	{
		pool<RTLIL::SigBit> sig;
		માટે (auto &bit : bits)
			sig.ઉમેરો(RTLIL::SigBit(bit.first, bit.second));
		પાછા sig;
	}

	size_t size() અચલ
	{
		પાછા bits.size();
	}
};

template <typename T, class Compare = શૂન્ય>
સંરચના SigSet
{
	static_assert(!std::is_same<Compare,શૂન્ય>::કિંમત, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	સંરચના bitDef_t : જાહેર std::pair<RTLIL::Wire*, પૂર્ણાંક> {
		bitDef_t() : std::pair<RTLIL::Wire*, પૂર્ણાંક>(ખાલી, 0) { }
		bitDef_t(અચલ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, પૂર્ણાંક>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(first->name);
			h.eat(second);
			પાછા h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	શૂન્ય clear()
	{
		bits.clear();
	}

	શૂન્ય ઉમેરો(અચલ RTLIL::SigSpec &sig, T data)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits[bit].ઉમેરો(data);
	}

	શૂન્ય ઉમેરો(અચલ RTLIL::SigSpec& sig, અચલ std::set<T> &data)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits[bit].ઉમેરો(data.begin(), data.end());
	}

	શૂન્ય erase(અચલ RTLIL::SigSpec& sig)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits[bit].clear();
	}

	શૂન્ય erase(અચલ RTLIL::SigSpec &sig, T data)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits[bit].erase(data);
	}

	શૂન્ય erase(અચલ RTLIL::SigSpec &sig, અચલ std::set<T> &data)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી)
				bits[bit].erase(data.begin(), data.end());
	}

	શૂન્ય find(અચલ RTLIL::SigSpec &sig, std::set<T> &result)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી) {
				auto &data = bits[bit];
				result.ઉમેરો(data.begin(), data.end());
			}
	}

	શૂન્ય find(અચલ RTLIL::SigSpec &sig, pool<T> &result)
	{
		માટે (અચલ auto &bit : sig)
			જો (bit.wire != ખાલી) {
				auto &data = bits[bit];
				result.ઉમેરો(data.begin(), data.end());
			}
	}

	std::set<T> find(અચલ RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		પાછા result;
	}

	bool has(અચલ RTLIL::SigSpec &sig)
	{
		માટે (auto &bit : sig)
			જો (bit.wire != ખાલી && bits.ગણતરી(bit))
				પાછા true;
		પાછા false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::કિંમત>::type> : જાહેર SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::કિંમત>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : જાહેર SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

સંરચના SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	શૂન્ય apply(RTLIL::SigBit &bit) અચલ
	{
		bit = database.find(bit);
	}

	શૂન્ય apply(RTLIL::SigSpec &sig) અચલ
	{
		માટે (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) અચલ
	{
		apply(bit);
		પાછા bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) અચલ
	{
		apply(sig);
		પાછા sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) અચલ
	{
		SigSpec sig(wire);
		apply(sig);
		પાછા sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
સંરચના SigMap final : જાહેર SigMapView
{
	SigMap(RTLIL::Module *module = ખાલી)
	{
		જો (module != ખાલી)
			set(module);
	}

	શૂન્ય swap(SigMap &other)
	{
		database.swap(other.database);
	}

	શૂન્ય clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	શૂન્ય set(RTLIL::Module *module)
	{
		પૂર્ણાંક bitcount = 0;
		માટે (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		માટે (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	શૂન્ય add(અચલ RTLIL::SigSpec& from, અચલ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		માટે (પૂર્ણાંક i = 0; i < GetSize(from); i++)
		{
			પૂર્ણાંક bfi = database.lookup(from[i]);
			પૂર્ણાંક bti = database.lookup(to[i]);

			અચલ RTLIL::SigBit &bf = database[bfi];
			અચલ RTLIL::SigBit &bt = database[bti];

			જો (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				જો (bf.wire == nullptr)
					database.ipromote(bfi);

				જો (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	શૂન્ય add(અચલ RTLIL::SigBit &bit)
	{
		અચલ auto &b = database.find(bit);
		જો (b.wire != nullptr)
			database.promote(bit);
	}

	શૂન્ય add(અચલ RTLIL::SigSpec &sig)
	{
		માટે (અચલ auto &bit : sig)
			add(bit);
	}

	અંતઃરેખ શૂન્ય add(Wire *wire) { પાછા add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() અચલ
	{
		RTLIL::SigSpec sig;
		માટે (અચલ auto &bit : database)
			જો (bit.wire != nullptr)
				sig.append(bit);
		પાછા sig;
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
સંરચના SigValMap final : જાહેર SigMapView
{
	dict<SigBit, Val> કિંમતો;

	શૂન્ય swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		કિંમતો.swap(other.કિંમતો);
	}

	શૂન્ય clear()
	{
		database.clear();
		કિંમતો.clear();
	}

	// Rebuild SigMap for all connections in module
	શૂન્ય set(RTLIL::Module *module)
	{
		પૂર્ણાંક bitcount = 0;
		માટે (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		કિંમતો.clear();
		database.reserve(bitcount);

		માટે (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	શૂન્ય add(અચલ RTLIL::SigSpec& from, અચલ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		માટે (પૂર્ણાંક i = 0; i < GetSize(from); i++)
		{
			પૂર્ણાંક bfi = database.lookup(from[i]);
			પૂર્ણાંક bti = database.lookup(to[i]);
			જો (bfi == bti) {
				ચાલુ;
			}

			અચલ RTLIL::SigBit &bf = database[bfi];
			અચલ RTLIL::SigBit &bt = database[bti];
			જો (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_કિંમત(bt, bf);
			} નહીંતર {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_કિંમત(bf, bt);
			}
		}
	}

	શૂન્ય addVal(અચલ RTLIL::SigBit &bit, અચલ Val &val)
	{
		કિંમતો[database.find(bit)] |= val;
	}

	શૂન્ય addVal(અચલ RTLIL::SigSpec &sig, અચલ Val &val)
	{
		માટે (અચલ auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_કિંમત(RTLIL::SigBit &bit) અચલ
	{
		bit = database.find(bit);
		auto it = કિંમતો.find(bit);
		પાછા it == કિંમતો.end() ? Val() : it->second;
	}

ખાનગી:
	શૂન્ય merge_કિંમત(અચલ RTLIL::SigBit &from, અચલ RTLIL::SigBit &to)
	{
		auto it = કિંમતો.find(from);
		જો (it == કિંમતો.end()) {
			પાછા;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		કિંમતો.erase(it);
		કિંમતો[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
