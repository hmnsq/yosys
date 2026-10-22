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

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساختار SigPool
{
	ساختار bitDef_t : عمومی std::pair<RTLIL::Wire*, صحیح> {
		bitDef_t() : std::pair<RTLIL::Wire*, صحیح>(پوچ, 0) { }
		bitDef_t(ثابت RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, صحیح>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first->name);
			h.eat(second);
			بازگشت h;
		}
	};

	pool<bitDef_t> bits;

	تهی clear()
	{
		bits.clear();
	}

	تهی add(ثابت RTLIL::SigSpec &sig)
	{
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits.درج(bit);
	}

	تهی add(ثابت SigPool &other)
	{
		برای (auto &bit : other.bits)
			bits.درج(bit);
	}

	تهی del(ثابت RTLIL::SigSpec &sig)
	{
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits.erase(bit);
	}

	تهی del(ثابت SigPool &other)
	{
		برای (auto &bit : other.bits)
			bits.erase(bit);
	}

	تهی expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		برای (صحیح i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			اگر (bit_from.first != پوچ && bit_to.first != پوچ && bits.شمار(bit_from) > 0)
				bits.درج(bit_to);
		}
	}

	RTLIL::SigSpec extract(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ && bits.شمار(bit))
				result.append(bit);
		بازگشت result;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ && bits.شمار(bit) == 0)
				result.append(bit);
		بازگشت result;
	}

	bool check(ثابت RTLIL::SigBit &bit) ثابت
	{
		بازگشت bit.wire != پوچ && bits.شمار(bit);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ && bits.شمار(bit))
				بازگشت true;
		بازگشت false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ && bits.شمار(bit) == 0)
				بازگشت false;
		بازگشت true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		برای (auto &bit : bits)
			بازگشت RTLIL::SigSpec(bit.first, bit.second);
		بازگشت RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::SigBit> sig;
		برای (auto &bit : bits)
			sig.درج(RTLIL::SigBit(bit.first, bit.second));
		بازگشت sig;
	}

	size_t size() ثابت
	{
		بازگشت bits.size();
	}
};

template <typename T, class Compare = تهی>
ساختار SigSet
{
	static_assert(!std::is_same<Compare,تهی>::مقدار, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ساختار bitDef_t : عمومی std::pair<RTLIL::Wire*, صحیح> {
		bitDef_t() : std::pair<RTLIL::Wire*, صحیح>(پوچ, 0) { }
		bitDef_t(ثابت RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, صحیح>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first->name);
			h.eat(second);
			بازگشت h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	تهی clear()
	{
		bits.clear();
	}

	تهی درج(ثابت RTLIL::SigSpec &sig, T data)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits[bit].درج(data);
	}

	تهی درج(ثابت RTLIL::SigSpec& sig, ثابت std::set<T> &data)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits[bit].درج(data.begin(), data.end());
	}

	تهی erase(ثابت RTLIL::SigSpec& sig)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits[bit].clear();
	}

	تهی erase(ثابت RTLIL::SigSpec &sig, T data)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits[bit].erase(data);
	}

	تهی erase(ثابت RTLIL::SigSpec &sig, ثابت std::set<T> &data)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ)
				bits[bit].erase(data.begin(), data.end());
	}

	تهی find(ثابت RTLIL::SigSpec &sig, std::set<T> &result)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ) {
				auto &data = bits[bit];
				result.درج(data.begin(), data.end());
			}
	}

	تهی find(ثابت RTLIL::SigSpec &sig, pool<T> &result)
	{
		برای (ثابت auto &bit : sig)
			اگر (bit.wire != پوچ) {
				auto &data = bits[bit];
				result.درج(data.begin(), data.end());
			}
	}

	std::set<T> find(ثابت RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		بازگشت result;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		برای (auto &bit : sig)
			اگر (bit.wire != پوچ && bits.شمار(bit))
				بازگشت true;
		بازگشت false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::مقدار>::type> : عمومی SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::مقدار>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : عمومی SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

ساختار SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	تهی apply(RTLIL::SigBit &bit) ثابت
	{
		bit = database.find(bit);
	}

	تهی apply(RTLIL::SigSpec &sig) ثابت
	{
		برای (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ثابت
	{
		apply(bit);
		بازگشت bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		بازگشت sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ثابت
	{
		SigSpec sig(wire);
		apply(sig);
		بازگشت sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ساختار SigMap final : عمومی SigMapView
{
	SigMap(RTLIL::Module *module = پوچ)
	{
		اگر (module != پوچ)
			set(module);
	}

	تهی swap(SigMap &other)
	{
		database.swap(other.database);
	}

	تهی clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	تهی set(RTLIL::Module *module)
	{
		صحیح bitcount = 0;
		برای (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		برای (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	تهی add(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		برای (صحیح i = 0; i < GetSize(from); i++)
		{
			صحیح bfi = database.lookup(from[i]);
			صحیح bti = database.lookup(to[i]);

			ثابت RTLIL::SigBit &bf = database[bfi];
			ثابت RTLIL::SigBit &bt = database[bti];

			اگر (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				اگر (bf.wire == nullptr)
					database.ipromote(bfi);

				اگر (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	تهی add(ثابت RTLIL::SigBit &bit)
	{
		ثابت auto &b = database.find(bit);
		اگر (b.wire != nullptr)
			database.promote(bit);
	}

	تهی add(ثابت RTLIL::SigSpec &sig)
	{
		برای (ثابت auto &bit : sig)
			add(bit);
	}

	درونخطی تهی add(Wire *wire) { بازگشت add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		برای (ثابت auto &bit : database)
			اگر (bit.wire != nullptr)
				sig.append(bit);
		بازگشت sig;
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
ساختار SigValMap final : عمومی SigMapView
{
	dict<SigBit, Val> مقادیر;

	تهی swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		مقادیر.swap(other.مقادیر);
	}

	تهی clear()
	{
		database.clear();
		مقادیر.clear();
	}

	// Rebuild SigMap for all connections in module
	تهی set(RTLIL::Module *module)
	{
		صحیح bitcount = 0;
		برای (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		مقادیر.clear();
		database.reserve(bitcount);

		برای (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	تهی add(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		برای (صحیح i = 0; i < GetSize(from); i++)
		{
			صحیح bfi = database.lookup(from[i]);
			صحیح bti = database.lookup(to[i]);
			اگر (bfi == bti) {
				ادامه;
			}

			ثابت RTLIL::SigBit &bf = database[bfi];
			ثابت RTLIL::SigBit &bt = database[bti];
			اگر (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_مقدار(bt, bf);
			} وگرنه {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_مقدار(bf, bt);
			}
		}
	}

	تهی addVal(ثابت RTLIL::SigBit &bit, ثابت Val &val)
	{
		مقادیر[database.find(bit)] |= val;
	}

	تهی addVal(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		برای (ثابت auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_مقدار(RTLIL::SigBit &bit) ثابت
	{
		bit = database.find(bit);
		auto it = مقادیر.find(bit);
		بازگشت it == مقادیر.end() ? Val() : it->second;
	}

خصوصی:
	تهی merge_مقدار(ثابت RTLIL::SigBit &from, ثابت RTLIL::SigBit &to)
	{
		auto it = مقادیر.find(from);
		اگر (it == مقادیر.end()) {
			بازگشت;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		مقادیر.erase(it);
		مقادیر[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
