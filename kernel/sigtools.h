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

#include "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa SigPool
{
	sa_nracanaa bitDef_t : public std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ> {
		bitDef_t() : std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ>(NULL, 0) { }
		bitDef_t(const RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first->name);
			h.eat(second);
			ສົ່ງຄືນ h;
		}
	};

	pool<bitDef_t> bits;

	void clear()
	{
		bits.clear();
	}

	void add(const RTLIL::SigSpec &sig)
	{
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits.insert(bit);
	}

	void add(const SigPool &other)
	{
		ສຳລັບ (auto &bit : other.bits)
			bits.insert(bit);
	}

	void del(const RTLIL::SigSpec &sig)
	{
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits.erase(bit);
	}

	void del(const SigPool &other)
	{
		ສຳລັບ (auto &bit : other.bits)
			bits.erase(bit);
	}

	void expand(const RTLIL::SigSpec &from, const RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			ຖ້າ (bit_from.first != NULL && bit_to.first != NULL && bits.count(bit_from) > 0)
				bits.insert(bit_to);
		}
	}

	RTLIL::SigSpec extract(const RTLIL::SigSpec &sig) const
	{
		RTLIL::SigSpec result;
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL && bits.count(bit))
				result.append(bit);
		ສົ່ງຄືນ result;
	}

	RTLIL::SigSpec remove(const RTLIL::SigSpec &sig) const
	{
		RTLIL::SigSpec result;
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL && bits.count(bit) == 0)
				result.append(bit);
		ສົ່ງຄືນ result;
	}

	bool check(const RTLIL::SigBit &bit) const
	{
		ສົ່ງຄືນ bit.wire != NULL && bits.count(bit);
	}

	bool check_any(const RTLIL::SigSpec &sig) const
	{
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL && bits.count(bit))
				ສົ່ງຄືນ true;
		ສົ່ງຄືນ false;
	}

	bool check_all(const RTLIL::SigSpec &sig) const
	{
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL && bits.count(bit) == 0)
				ສົ່ງຄືນ false;
		ສົ່ງຄືນ true;
	}

	RTLIL::SigSpec export_one() const
	{
		ສຳລັບ (auto &bit : bits)
			ສົ່ງຄືນ RTLIL::SigSpec(bit.first, bit.second);
		ສົ່ງຄືນ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() const
	{
		pool<RTLIL::SigBit> sig;
		ສຳລັບ (auto &bit : bits)
			sig.insert(RTLIL::SigBit(bit.first, bit.second));
		ສົ່ງຄືນ sig;
	}

	size_t size() const
	{
		ສົ່ງຄືນ bits.size();
	}
};

template <typename T, class Compare = void>
sa_nracanaa SigSet
{
	static_assert(!std::is_same<Compare,void>::value, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	sa_nracanaa bitDef_t : public std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ> {
		bitDef_t() : std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ>(NULL, 0) { }
		bitDef_t(const RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ຈຳນວນເຕັມ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first->name);
			h.eat(second);
			ສົ່ງຄືນ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	void clear()
	{
		bits.clear();
	}

	void insert(const RTLIL::SigSpec &sig, T data)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits[bit].insert(data);
	}

	void insert(const RTLIL::SigSpec& sig, const std::set<T> &data)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits[bit].insert(data.begin(), data.end());
	}

	void erase(const RTLIL::SigSpec& sig)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits[bit].clear();
	}

	void erase(const RTLIL::SigSpec &sig, T data)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits[bit].erase(data);
	}

	void erase(const RTLIL::SigSpec &sig, const std::set<T> &data)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL)
				bits[bit].erase(data.begin(), data.end());
	}

	void find(const RTLIL::SigSpec &sig, std::set<T> &result)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL) {
				auto &data = bits[bit];
				result.insert(data.begin(), data.end());
			}
	}

	void find(const RTLIL::SigSpec &sig, pool<T> &result)
	{
		ສຳລັບ (const auto &bit : sig)
			ຖ້າ (bit.wire != NULL) {
				auto &data = bits[bit];
				result.insert(data.begin(), data.end());
			}
	}

	std::set<T> find(const RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		ສົ່ງຄືນ result;
	}

	bool has(const RTLIL::SigSpec &sig)
	{
		ສຳລັບ (auto &bit : sig)
			ຖ້າ (bit.wire != NULL && bits.count(bit))
				ສົ່ງຄືນ true;
		ສົ່ງຄືນ false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::value>::type> : public SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::value>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : public SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

sa_nracanaa SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	void apply(RTLIL::SigBit &bit) const
	{
		bit = database.find(bit);
	}

	void apply(RTLIL::SigSpec &sig) const
	{
		ສຳລັບ (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) const
	{
		apply(bit);
		ສົ່ງຄືນ bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) const
	{
		apply(sig);
		ສົ່ງຄືນ sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) const
	{
		SigSpec sig(wire);
		apply(sig);
		ສົ່ງຄືນ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
sa_nracanaa SigMap final : public SigMapView
{
	SigMap(RTLIL::Module *module = NULL)
	{
		ຖ້າ (module != NULL)
			set(module);
	}

	void swap(SigMap &other)
	{
		database.swap(other.database);
	}

	void clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	void set(RTLIL::Module *module)
	{
		ຈຳນວນເຕັມ bitcount = 0;
		ສຳລັບ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		ສຳລັບ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	void add(const RTLIL::SigSpec& from, const RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(from); i++)
		{
			ຈຳນວນເຕັມ bfi = database.lookup(from[i]);
			ຈຳນວນເຕັມ bti = database.lookup(to[i]);

			const RTLIL::SigBit &bf = database[bfi];
			const RTLIL::SigBit &bt = database[bti];

			ຖ້າ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ຖ້າ (bf.wire == nullptr)
					database.ipromote(bfi);

				ຖ້າ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	void add(const RTLIL::SigBit &bit)
	{
		const auto &b = database.find(bit);
		ຖ້າ (b.wire != nullptr)
			database.promote(bit);
	}

	void add(const RTLIL::SigSpec &sig)
	{
		ສຳລັບ (const auto &bit : sig)
			add(bit);
	}

	anta_hstha void add(Wire *wire) { ສົ່ງຄືນ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() const
	{
		RTLIL::SigSpec sig;
		ສຳລັບ (const auto &bit : database)
			ຖ້າ (bit.wire != nullptr)
				sig.append(bit);
		ສົ່ງຄືນ sig;
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
sa_nracanaa SigValMap final : public SigMapView
{
	dict<SigBit, Val> values;

	void swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		values.swap(other.values);
	}

	void clear()
	{
		database.clear();
		values.clear();
	}

	// Rebuild SigMap for all connections in module
	void set(RTLIL::Module *module)
	{
		ຈຳນວນເຕັມ bitcount = 0;
		ສຳລັບ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		values.clear();
		database.reserve(bitcount);

		ສຳລັບ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	void add(const RTLIL::SigSpec& from, const RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(from); i++)
		{
			ຈຳນວນເຕັມ bfi = database.lookup(from[i]);
			ຈຳນວນເຕັມ bti = database.lookup(to[i]);
			ຖ້າ (bfi == bti) {
				continue;
			}

			const RTLIL::SigBit &bf = database[bfi];
			const RTLIL::SigBit &bt = database[bti];
			ຖ້າ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_value(bt, bf);
			} ບໍ່ດັ່ງນັ້ນ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_value(bf, bt);
			}
		}
	}

	void addVal(const RTLIL::SigBit &bit, const Val &val)
	{
		values[database.find(bit)] |= val;
	}

	void addVal(const RTLIL::SigSpec &sig, const Val &val)
	{
		ສຳລັບ (const auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_value(RTLIL::SigBit &bit) const
	{
		bit = database.find(bit);
		auto it = values.find(bit);
		ສົ່ງຄືນ it == values.end() ? Val() : it->second;
	}

private:
	void merge_value(const RTLIL::SigBit &from, const RTLIL::SigBit &to)
	{
		auto it = values.find(from);
		ຖ້າ (it == values.end()) {
			ສົ່ງຄືນ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		values.erase(it);
		values[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
