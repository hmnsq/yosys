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

#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு SigPool
{
	கட்டமைப்பு bitDef_t : பொது std::pair<RTLIL::Wire*, முழு> {
		bitDef_t() : std::pair<RTLIL::Wire*, முழு>(வெற்று, 0) { }
		bitDef_t(மாறிலி RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, முழு>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(first->name);
			h.eat(second);
			திரும்பு h;
		}
	};

	pool<bitDef_t> bits;

	வெறுமை clear()
	{
		bits.clear();
	}

	வெறுமை add(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits.செருகு(bit);
	}

	வெறுமை add(மாறிலி SigPool &other)
	{
		க்கு (auto &bit : other.bits)
			bits.செருகு(bit);
	}

	வெறுமை del(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits.erase(bit);
	}

	வெறுமை del(மாறிலி SigPool &other)
	{
		க்கு (auto &bit : other.bits)
			bits.erase(bit);
	}

	வெறுமை expand(மாறிலி RTLIL::SigSpec &from, மாறிலி RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		க்கு (முழு i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			எனில் (bit_from.first != வெற்று && bit_to.first != வெற்று && bits.மொத்தம்(bit_from) > 0)
				bits.செருகு(bit_to);
		}
	}

	RTLIL::SigSpec extract(மாறிலி RTLIL::SigSpec &sig) மாறிலி
	{
		RTLIL::SigSpec result;
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று && bits.மொத்தம்(bit))
				result.append(bit);
		திரும்பு result;
	}

	RTLIL::SigSpec remove(மாறிலி RTLIL::SigSpec &sig) மாறிலி
	{
		RTLIL::SigSpec result;
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று && bits.மொத்தம்(bit) == 0)
				result.append(bit);
		திரும்பு result;
	}

	bool check(மாறிலி RTLIL::SigBit &bit) மாறிலி
	{
		திரும்பு bit.wire != வெற்று && bits.மொத்தம்(bit);
	}

	bool check_any(மாறிலி RTLIL::SigSpec &sig) மாறிலி
	{
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று && bits.மொத்தம்(bit))
				திரும்பு true;
		திரும்பு false;
	}

	bool check_all(மாறிலி RTLIL::SigSpec &sig) மாறிலி
	{
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று && bits.மொத்தம்(bit) == 0)
				திரும்பு false;
		திரும்பு true;
	}

	RTLIL::SigSpec export_one() மாறிலி
	{
		க்கு (auto &bit : bits)
			திரும்பு RTLIL::SigSpec(bit.first, bit.second);
		திரும்பு RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() மாறிலி
	{
		pool<RTLIL::SigBit> sig;
		க்கு (auto &bit : bits)
			sig.செருகு(RTLIL::SigBit(bit.first, bit.second));
		திரும்பு sig;
	}

	size_t size() மாறிலி
	{
		திரும்பு bits.size();
	}
};

template <typename T, class Compare = வெறுமை>
கட்டமைப்பு SigSet
{
	static_assert(!std::is_same<Compare,வெறுமை>::மதிப்பு, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	கட்டமைப்பு bitDef_t : பொது std::pair<RTLIL::Wire*, முழு> {
		bitDef_t() : std::pair<RTLIL::Wire*, முழு>(வெற்று, 0) { }
		bitDef_t(மாறிலி RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, முழு>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(first->name);
			h.eat(second);
			திரும்பு h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	வெறுமை clear()
	{
		bits.clear();
	}

	வெறுமை செருகு(மாறிலி RTLIL::SigSpec &sig, T data)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits[bit].செருகு(data);
	}

	வெறுமை செருகு(மாறிலி RTLIL::SigSpec& sig, மாறிலி std::set<T> &data)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits[bit].செருகு(data.begin(), data.end());
	}

	வெறுமை erase(மாறிலி RTLIL::SigSpec& sig)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits[bit].clear();
	}

	வெறுமை erase(மாறிலி RTLIL::SigSpec &sig, T data)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits[bit].erase(data);
	}

	வெறுமை erase(மாறிலி RTLIL::SigSpec &sig, மாறிலி std::set<T> &data)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று)
				bits[bit].erase(data.begin(), data.end());
	}

	வெறுமை find(மாறிலி RTLIL::SigSpec &sig, std::set<T> &result)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று) {
				auto &data = bits[bit];
				result.செருகு(data.begin(), data.end());
			}
	}

	வெறுமை find(மாறிலி RTLIL::SigSpec &sig, pool<T> &result)
	{
		க்கு (மாறிலி auto &bit : sig)
			எனில் (bit.wire != வெற்று) {
				auto &data = bits[bit];
				result.செருகு(data.begin(), data.end());
			}
	}

	std::set<T> find(மாறிலி RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		திரும்பு result;
	}

	bool has(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (auto &bit : sig)
			எனில் (bit.wire != வெற்று && bits.மொத்தம்(bit))
				திரும்பு true;
		திரும்பு false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::மதிப்பு>::type> : பொது SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::மதிப்பு>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : பொது SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

கட்டமைப்பு SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	வெறுமை apply(RTLIL::SigBit &bit) மாறிலி
	{
		bit = database.find(bit);
	}

	வெறுமை apply(RTLIL::SigSpec &sig) மாறிலி
	{
		க்கு (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) மாறிலி
	{
		apply(bit);
		திரும்பு bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) மாறிலி
	{
		apply(sig);
		திரும்பு sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) மாறிலி
	{
		SigSpec sig(wire);
		apply(sig);
		திரும்பு sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
கட்டமைப்பு SigMap final : பொது SigMapView
{
	SigMap(RTLIL::Module *module = வெற்று)
	{
		எனில் (module != வெற்று)
			set(module);
	}

	வெறுமை swap(SigMap &other)
	{
		database.swap(other.database);
	}

	வெறுமை clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	வெறுமை set(RTLIL::Module *module)
	{
		முழு bitcount = 0;
		க்கு (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		க்கு (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	வெறுமை add(மாறிலி RTLIL::SigSpec& from, மாறிலி RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		க்கு (முழு i = 0; i < GetSize(from); i++)
		{
			முழு bfi = database.lookup(from[i]);
			முழு bti = database.lookup(to[i]);

			மாறிலி RTLIL::SigBit &bf = database[bfi];
			மாறிலி RTLIL::SigBit &bt = database[bti];

			எனில் (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				எனில் (bf.wire == nullptr)
					database.ipromote(bfi);

				எனில் (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	வெறுமை add(மாறிலி RTLIL::SigBit &bit)
	{
		மாறிலி auto &b = database.find(bit);
		எனில் (b.wire != nullptr)
			database.promote(bit);
	}

	வெறுமை add(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (மாறிலி auto &bit : sig)
			add(bit);
	}

	உள்வரி வெறுமை add(Wire *wire) { திரும்பு add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() மாறிலி
	{
		RTLIL::SigSpec sig;
		க்கு (மாறிலி auto &bit : database)
			எனில் (bit.wire != nullptr)
				sig.append(bit);
		திரும்பு sig;
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
கட்டமைப்பு SigValMap final : பொது SigMapView
{
	dict<SigBit, Val> மதிப்புகள்;

	வெறுமை swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		மதிப்புகள்.swap(other.மதிப்புகள்);
	}

	வெறுமை clear()
	{
		database.clear();
		மதிப்புகள்.clear();
	}

	// Rebuild SigMap for all connections in module
	வெறுமை set(RTLIL::Module *module)
	{
		முழு bitcount = 0;
		க்கு (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		மதிப்புகள்.clear();
		database.reserve(bitcount);

		க்கு (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	வெறுமை add(மாறிலி RTLIL::SigSpec& from, மாறிலி RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		க்கு (முழு i = 0; i < GetSize(from); i++)
		{
			முழு bfi = database.lookup(from[i]);
			முழு bti = database.lookup(to[i]);
			எனில் (bfi == bti) {
				தொடர்;
			}

			மாறிலி RTLIL::SigBit &bf = database[bfi];
			மாறிலி RTLIL::SigBit &bt = database[bti];
			எனில் (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_மதிப்பு(bt, bf);
			} இல்லையெனில் {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_மதிப்பு(bf, bt);
			}
		}
	}

	வெறுமை addVal(மாறிலி RTLIL::SigBit &bit, மாறிலி Val &val)
	{
		மதிப்புகள்[database.find(bit)] |= val;
	}

	வெறுமை addVal(மாறிலி RTLIL::SigSpec &sig, மாறிலி Val &val)
	{
		க்கு (மாறிலி auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_மதிப்பு(RTLIL::SigBit &bit) மாறிலி
	{
		bit = database.find(bit);
		auto it = மதிப்புகள்.find(bit);
		திரும்பு it == மதிப்புகள்.end() ? Val() : it->second;
	}

தனிப்பட்ட:
	வெறுமை merge_மதிப்பு(மாறிலி RTLIL::SigBit &from, மாறிலி RTLIL::SigBit &to)
	{
		auto it = மதிப்புகள்.find(from);
		எனில் (it == மதிப்புகள்.end()) {
			திரும்பு;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		மதிப்புகள்.erase(it);
		மதிப்புகள்[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
