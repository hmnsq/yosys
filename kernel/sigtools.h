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

#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা SigPool
{
	সংরচনা bitDef_t : সার্বজনিক std::pair<RTLIL::Wire*, পূর্ণাঙ্ক> {
		bitDef_t() : std::pair<RTLIL::Wire*, পূর্ণাঙ্ক>(রিক্ত, 0) { }
		bitDef_t(ধ্রুব RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, পূর্ণাঙ্ক>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
			h.eat(first->name);
			h.eat(second);
			প্রত্যাগম h;
		}
	};

	pool<bitDef_t> bits;

	শূন্য clear()
	{
		bits.clear();
	}

	শূন্য add(ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits.ঢোকাও(bit);
	}

	শূন্য add(ধ্রুব SigPool &other)
	{
		প্রতি (auto &bit : other.bits)
			bits.ঢোকাও(bit);
	}

	শূন্য del(ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits.erase(bit);
	}

	শূন্য del(ধ্রুব SigPool &other)
	{
		প্রতি (auto &bit : other.bits)
			bits.erase(bit);
	}

	শূন্য expand(ধ্রুব RTLIL::SigSpec &from, ধ্রুব RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			যদি (bit_from.first != রিক্ত && bit_to.first != রিক্ত && bits.গুনতি(bit_from) > 0)
				bits.ঢোকাও(bit_to);
		}
	}

	RTLIL::SigSpec extract(ধ্রুব RTLIL::SigSpec &sig) ধ্রুব
	{
		RTLIL::SigSpec result;
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত && bits.গুনতি(bit))
				result.append(bit);
		প্রত্যাগম result;
	}

	RTLIL::SigSpec remove(ধ্রুব RTLIL::SigSpec &sig) ধ্রুব
	{
		RTLIL::SigSpec result;
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত && bits.গুনতি(bit) == 0)
				result.append(bit);
		প্রত্যাগম result;
	}

	bool check(ধ্রুব RTLIL::SigBit &bit) ধ্রুব
	{
		প্রত্যাগম bit.wire != রিক্ত && bits.গুনতি(bit);
	}

	bool check_any(ধ্রুব RTLIL::SigSpec &sig) ধ্রুব
	{
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত && bits.গুনতি(bit))
				প্রত্যাগম true;
		প্রত্যাগম false;
	}

	bool check_all(ধ্রুব RTLIL::SigSpec &sig) ধ্রুব
	{
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত && bits.গুনতি(bit) == 0)
				প্রত্যাগম false;
		প্রত্যাগম true;
	}

	RTLIL::SigSpec export_one() ধ্রুব
	{
		প্রতি (auto &bit : bits)
			প্রত্যাগম RTLIL::SigSpec(bit.first, bit.second);
		প্রত্যাগম RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ধ্রুব
	{
		pool<RTLIL::SigBit> sig;
		প্রতি (auto &bit : bits)
			sig.ঢোকাও(RTLIL::SigBit(bit.first, bit.second));
		প্রত্যাগম sig;
	}

	size_t size() ধ্রুব
	{
		প্রত্যাগম bits.size();
	}
};

template <typename T, class Compare = শূন্য>
সংরচনা SigSet
{
	static_assert(!std::is_same<Compare,শূন্য>::মান, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	সংরচনা bitDef_t : সার্বজনিক std::pair<RTLIL::Wire*, পূর্ণাঙ্ক> {
		bitDef_t() : std::pair<RTLIL::Wire*, পূর্ণাঙ্ক>(রিক্ত, 0) { }
		bitDef_t(ধ্রুব RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, পূর্ণাঙ্ক>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
			h.eat(first->name);
			h.eat(second);
			প্রত্যাগম h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	শূন্য clear()
	{
		bits.clear();
	}

	শূন্য ঢোকাও(ধ্রুব RTLIL::SigSpec &sig, T data)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits[bit].ঢোকাও(data);
	}

	শূন্য ঢোকাও(ধ্রুব RTLIL::SigSpec& sig, ধ্রুব std::set<T> &data)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits[bit].ঢোকাও(data.begin(), data.end());
	}

	শূন্য erase(ধ্রুব RTLIL::SigSpec& sig)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits[bit].clear();
	}

	শূন্য erase(ধ্রুব RTLIL::SigSpec &sig, T data)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits[bit].erase(data);
	}

	শূন্য erase(ধ্রুব RTLIL::SigSpec &sig, ধ্রুব std::set<T> &data)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত)
				bits[bit].erase(data.begin(), data.end());
	}

	শূন্য find(ধ্রুব RTLIL::SigSpec &sig, std::set<T> &result)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত) {
				auto &data = bits[bit];
				result.ঢোকাও(data.begin(), data.end());
			}
	}

	শূন্য find(ধ্রুব RTLIL::SigSpec &sig, pool<T> &result)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			যদি (bit.wire != রিক্ত) {
				auto &data = bits[bit];
				result.ঢোকাও(data.begin(), data.end());
			}
	}

	std::set<T> find(ধ্রুব RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		প্রত্যাগম result;
	}

	bool has(ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (auto &bit : sig)
			যদি (bit.wire != রিক্ত && bits.গুনতি(bit))
				প্রত্যাগম true;
		প্রত্যাগম false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::মান>::type> : সার্বজনিক SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::মান>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : সার্বজনিক SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

সংরচনা SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	শূন্য apply(RTLIL::SigBit &bit) ধ্রুব
	{
		bit = database.find(bit);
	}

	শূন্য apply(RTLIL::SigSpec &sig) ধ্রুব
	{
		প্রতি (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ধ্রুব
	{
		apply(bit);
		প্রত্যাগম bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ধ্রুব
	{
		apply(sig);
		প্রত্যাগম sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ধ্রুব
	{
		SigSpec sig(wire);
		apply(sig);
		প্রত্যাগম sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
সংরচনা SigMap final : সার্বজনিক SigMapView
{
	SigMap(RTLIL::Module *module = রিক্ত)
	{
		যদি (module != রিক্ত)
			set(module);
	}

	শূন্য swap(SigMap &other)
	{
		database.swap(other.database);
	}

	শূন্য clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	শূন্য set(RTLIL::Module *module)
	{
		পূর্ণাঙ্ক bitcount = 0;
		প্রতি (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		প্রতি (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	শূন্য add(ধ্রুব RTLIL::SigSpec& from, ধ্রুব RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(from); i++)
		{
			পূর্ণাঙ্ক bfi = database.lookup(from[i]);
			পূর্ণাঙ্ক bti = database.lookup(to[i]);

			ধ্রুব RTLIL::SigBit &bf = database[bfi];
			ধ্রুব RTLIL::SigBit &bt = database[bti];

			যদি (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				যদি (bf.wire == nullptr)
					database.ipromote(bfi);

				যদি (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	শূন্য add(ধ্রুব RTLIL::SigBit &bit)
	{
		ধ্রুব auto &b = database.find(bit);
		যদি (b.wire != nullptr)
			database.promote(bit);
	}

	শূন্য add(ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			add(bit);
	}

	অন্তঃস্থ শূন্য add(Wire *wire) { প্রত্যাগম add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ধ্রুব
	{
		RTLIL::SigSpec sig;
		প্রতি (ধ্রুব auto &bit : database)
			যদি (bit.wire != nullptr)
				sig.append(bit);
		প্রত্যাগম sig;
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
সংরচনা SigValMap final : সার্বজনিক SigMapView
{
	dict<SigBit, Val> মানসমূহ;

	শূন্য swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		মানসমূহ.swap(other.মানসমূহ);
	}

	শূন্য clear()
	{
		database.clear();
		মানসমূহ.clear();
	}

	// Rebuild SigMap for all connections in module
	শূন্য set(RTLIL::Module *module)
	{
		পূর্ণাঙ্ক bitcount = 0;
		প্রতি (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		মানসমূহ.clear();
		database.reserve(bitcount);

		প্রতি (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	শূন্য add(ধ্রুব RTLIL::SigSpec& from, ধ্রুব RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(from); i++)
		{
			পূর্ণাঙ্ক bfi = database.lookup(from[i]);
			পূর্ণাঙ্ক bti = database.lookup(to[i]);
			যদি (bfi == bti) {
				অনুবর্ত;
			}

			ধ্রুব RTLIL::SigBit &bf = database[bfi];
			ধ্রুব RTLIL::SigBit &bt = database[bti];
			যদি (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_মান(bt, bf);
			} অন্যথা {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_মান(bf, bt);
			}
		}
	}

	শূন্য addVal(ধ্রুব RTLIL::SigBit &bit, ধ্রুব Val &val)
	{
		মানসমূহ[database.find(bit)] |= val;
	}

	শূন্য addVal(ধ্রুব RTLIL::SigSpec &sig, ধ্রুব Val &val)
	{
		প্রতি (ধ্রুব auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_মান(RTLIL::SigBit &bit) ধ্রুব
	{
		bit = database.find(bit);
		auto it = মানসমূহ.find(bit);
		প্রত্যাগম it == মানসমূহ.end() ? Val() : it->second;
	}

গুপ্ত:
	শূন্য merge_মান(ধ্রুব RTLIL::SigBit &from, ধ্রুব RTLIL::SigBit &to)
	{
		auto it = মানসমূহ.find(from);
		যদি (it == মানসমূহ.end()) {
			প্রত্যাগম;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		মানসমূহ.erase(it);
		মানসমূহ[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
