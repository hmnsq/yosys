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

কাঠামো SigPool
{
	কাঠামো bitDef_t : সর্বজনীন std::pair<RTLIL::Wire*, পূর্ণসংখ্যা> {
		bitDef_t() : std::pair<RTLIL::Wire*, পূর্ণসংখ্যা>(ফাঁকা, 0) { }
		bitDef_t(ধ্রুবক RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, পূর্ণসংখ্যা>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(first->name);
			h.eat(second);
			ফেরত h;
		}
	};

	pool<bitDef_t> bits;

	শূন্য clear()
	{
		bits.clear();
	}

	শূন্য add(ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits.ঢোকাও(bit);
	}

	শূন্য add(ধ্রুবক SigPool &other)
	{
		জন্য (auto &bit : other.bits)
			bits.ঢোকাও(bit);
	}

	শূন্য del(ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits.erase(bit);
	}

	শূন্য del(ধ্রুবক SigPool &other)
	{
		জন্য (auto &bit : other.bits)
			bits.erase(bit);
	}

	শূন্য expand(ধ্রুবক RTLIL::SigSpec &from, ধ্রুবক RTLIL::SigSpec &to)
	{
		log_assert(Get_আকার(from) == Get_আকার(to));
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			যদি (bit_from.first != ফাঁকা && bit_to.first != ফাঁকা && bits.গুনতি(bit_from) > 0)
				bits.ঢোকাও(bit_to);
		}
	}

	RTLIL::SigSpec extract(ধ্রুবক RTLIL::SigSpec &sig) ধ্রুবক
	{
		RTLIL::SigSpec ফলাফল;
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা && bits.গুনতি(bit))
				ফলাফল.append(bit);
		ফেরত ফলাফল;
	}

	RTLIL::SigSpec remove(ধ্রুবক RTLIL::SigSpec &sig) ধ্রুবক
	{
		RTLIL::SigSpec ফলাফল;
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা && bits.গুনতি(bit) == 0)
				ফলাফল.append(bit);
		ফেরত ফলাফল;
	}

	bool check(ধ্রুবক RTLIL::SigBit &bit) ধ্রুবক
	{
		ফেরত bit.wire != ফাঁকা && bits.গুনতি(bit);
	}

	bool check_any(ধ্রুবক RTLIL::SigSpec &sig) ধ্রুবক
	{
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা && bits.গুনতি(bit))
				ফেরত true;
		ফেরত false;
	}

	bool check_all(ধ্রুবক RTLIL::SigSpec &sig) ধ্রুবক
	{
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা && bits.গুনতি(bit) == 0)
				ফেরত false;
		ফেরত true;
	}

	RTLIL::SigSpec export_one() ধ্রুবক
	{
		জন্য (auto &bit : bits)
			ফেরত RTLIL::SigSpec(bit.first, bit.second);
		ফেরত RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ধ্রুবক
	{
		pool<RTLIL::SigBit> sig;
		জন্য (auto &bit : bits)
			sig.ঢোকাও(RTLIL::SigBit(bit.first, bit.second));
		ফেরত sig;
	}

	আকার_t আকার_2() ধ্রুবক
	{
		ফেরত bits.আকার_2();
	}
};

template <typename T, class Compare = শূন্য>
কাঠামো SigSet
{
	static_assert(!std::is_same<Compare,শূন্য>::মান, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	কাঠামো bitDef_t : সর্বজনীন std::pair<RTLIL::Wire*, পূর্ণসংখ্যা> {
		bitDef_t() : std::pair<RTLIL::Wire*, পূর্ণসংখ্যা>(ফাঁকা, 0) { }
		bitDef_t(ধ্রুবক RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, পূর্ণসংখ্যা>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(first->name);
			h.eat(second);
			ফেরত h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	শূন্য clear()
	{
		bits.clear();
	}

	শূন্য ঢোকাও(ধ্রুবক RTLIL::SigSpec &sig, T তথ্য)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits[bit].ঢোকাও(তথ্য);
	}

	শূন্য ঢোকাও(ধ্রুবক RTLIL::SigSpec& sig, ধ্রুবক std::set<T> &তথ্য)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits[bit].ঢোকাও(তথ্য.begin(), তথ্য.end());
	}

	শূন্য erase(ধ্রুবক RTLIL::SigSpec& sig)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits[bit].clear();
	}

	শূন্য erase(ধ্রুবক RTLIL::SigSpec &sig, T তথ্য)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits[bit].erase(তথ্য);
	}

	শূন্য erase(ধ্রুবক RTLIL::SigSpec &sig, ধ্রুবক std::set<T> &তথ্য)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা)
				bits[bit].erase(তথ্য.begin(), তথ্য.end());
	}

	শূন্য find(ধ্রুবক RTLIL::SigSpec &sig, std::set<T> &ফলাফল)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা) {
				auto &তথ্য = bits[bit];
				ফলাফল.ঢোকাও(তথ্য.begin(), তথ্য.end());
			}
	}

	শূন্য find(ধ্রুবক RTLIL::SigSpec &sig, pool<T> &ফলাফল)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			যদি (bit.wire != ফাঁকা) {
				auto &তথ্য = bits[bit];
				ফলাফল.ঢোকাও(তথ্য.begin(), তথ্য.end());
			}
	}

	std::set<T> find(ধ্রুবক RTLIL::SigSpec &sig)
	{
		std::set<T> ফলাফল;
		find(sig, ফলাফল);
		ফেরত ফলাফল;
	}

	bool has(ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (auto &bit : sig)
			যদি (bit.wire != ফাঁকা && bits.গুনতি(bit))
				ফেরত true;
		ফেরত false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::মান>::type> : সর্বজনীন SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::মান>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : সর্বজনীন SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

কাঠামো SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	শূন্য apply(RTLIL::SigBit &bit) ধ্রুবক
	{
		bit = database.find(bit);
	}

	শূন্য apply(RTLIL::SigSpec &sig) ধ্রুবক
	{
		জন্য (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ধ্রুবক
	{
		apply(bit);
		ফেরত bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ধ্রুবক
	{
		apply(sig);
		ফেরত sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ধ্রুবক
	{
		SigSpec sig(wire);
		apply(sig);
		ফেরত sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
কাঠামো SigMap final : সর্বজনীন SigMapView
{
	SigMap(RTLIL::Module *module = ফাঁকা)
	{
		যদি (module != ফাঁকা)
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
		পূর্ণসংখ্যা bitcount = 0;
		জন্য (auto &it : module->connections())
			bitcount += it.first.আকার_2();

		database.clear();
		database.reserve(bitcount);

		জন্য (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	শূন্য add(ধ্রুবক RTLIL::SigSpec& from, ধ্রুবক RTLIL::SigSpec& to)
	{
		log_assert(Get_আকার(from) == Get_আকার(to));

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(from); i++)
		{
			পূর্ণসংখ্যা bfi = database.lookup(from[i]);
			পূর্ণসংখ্যা bti = database.lookup(to[i]);

			ধ্রুবক RTLIL::SigBit &bf = database[bfi];
			ধ্রুবক RTLIL::SigBit &bt = database[bti];

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
	শূন্য add(ধ্রুবক RTLIL::SigBit &bit)
	{
		ধ্রুবক auto &b = database.find(bit);
		যদি (b.wire != nullptr)
			database.promote(bit);
	}

	শূন্য add(ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			add(bit);
	}

	অন্তঃরেখ শূন্য add(Wire *wire) { ফেরত add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ধ্রুবক
	{
		RTLIL::SigSpec sig;
		জন্য (ধ্রুবক auto &bit : database)
			যদি (bit.wire != nullptr)
				sig.append(bit);
		ফেরত sig;
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
কাঠামো SigValMap final : সর্বজনীন SigMapView
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
		পূর্ণসংখ্যা bitcount = 0;
		জন্য (auto &it : module->connections())
			bitcount += it.first.আকার_2();

		database.clear();
		মানসমূহ.clear();
		database.reserve(bitcount);

		জন্য (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	শূন্য add(ধ্রুবক RTLIL::SigSpec& from, ধ্রুবক RTLIL::SigSpec& to)
	{
		log_assert(Get_আকার(from) == Get_আকার(to));

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(from); i++)
		{
			পূর্ণসংখ্যা bfi = database.lookup(from[i]);
			পূর্ণসংখ্যা bti = database.lookup(to[i]);
			যদি (bfi == bti) {
				চালু;
			}

			ধ্রুবক RTLIL::SigBit &bf = database[bfi];
			ধ্রুবক RTLIL::SigBit &bt = database[bti];
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

	শূন্য addVal(ধ্রুবক RTLIL::SigBit &bit, ধ্রুবক Val &val)
	{
		মানসমূহ[database.find(bit)] |= val;
	}

	শূন্য addVal(ধ্রুবক RTLIL::SigSpec &sig, ধ্রুবক Val &val)
	{
		জন্য (ধ্রুবক auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_মান(RTLIL::SigBit &bit) ধ্রুবক
	{
		bit = database.find(bit);
		auto it = মানসমূহ.find(bit);
		ফেরত it == মানসমূহ.end() ? Val() : it->second;
	}

ব্যক্তিগত:
	শূন্য merge_মান(ধ্রুবক RTLIL::SigBit &from, ধ্রুবক RTLIL::SigBit &to)
	{
		auto it = মানসমূহ.find(from);
		যদি (it == মানসমূহ.end()) {
			ফেরত;
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
