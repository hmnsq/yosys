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

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية SigPool
{
	بنية bitDef_t : عام std::pair<RTLIL::Wire*, صحيح> {
		bitDef_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		bitDef_t(ثابت RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, صحيح>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first->name);
			h.eat(second);
			ܦܢܐ h;
		}
	};

	pool<bitDef_t> bits;

	فراغ clear()
	{
		bits.clear();
	}

	فراغ add(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits.insert(bit);
	}

	فراغ add(ثابت SigPool &other)
	{
		لكل (auto &bit : other.bits)
			bits.insert(bit);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits.erase(bit);
	}

	فراغ del(ثابت SigPool &other)
	{
		لكل (auto &bit : other.bits)
			bits.erase(bit);
	}

	فراغ expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		لكل (صحيح i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			ܐܢ (bit_from.first != عدم && bit_to.first != عدم && bits.count(bit_from) > 0)
				bits.insert(bit_to);
		}
	}

	RTLIL::SigSpec extract(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم && bits.count(bit))
				result.append(bit);
		ܦܢܐ result;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم && bits.count(bit) == 0)
				result.append(bit);
		ܦܢܐ result;
	}

	bool check(ثابت RTLIL::SigBit &bit) ثابت
	{
		ܦܢܐ bit.wire != عدم && bits.count(bit);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم && bits.count(bit))
				ܦܢܐ true;
		ܦܢܐ false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم && bits.count(bit) == 0)
				ܦܢܐ false;
		ܦܢܐ true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (auto &bit : bits)
			ܦܢܐ RTLIL::SigSpec(bit.first, bit.second);
		ܦܢܐ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::SigBit> sig;
		لكل (auto &bit : bits)
			sig.insert(RTLIL::SigBit(bit.first, bit.second));
		ܦܢܐ sig;
	}

	size_t size() ثابت
	{
		ܦܢܐ bits.size();
	}
};

template <typename T, class Compare = فراغ>
بنية SigSet
{
	static_assert(!std::is_same<Compare,فراغ>::value, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية bitDef_t : عام std::pair<RTLIL::Wire*, صحيح> {
		bitDef_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		bitDef_t(ثابت RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, صحيح>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first->name);
			h.eat(second);
			ܦܢܐ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	فراغ clear()
	{
		bits.clear();
	}

	فراغ insert(ثابت RTLIL::SigSpec &sig, T data)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits[bit].insert(data);
	}

	فراغ insert(ثابت RTLIL::SigSpec& sig, ثابت std::set<T> &data)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits[bit].insert(data.begin(), data.end());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits[bit].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T data)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits[bit].erase(data);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::set<T> &data)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم)
				bits[bit].erase(data.begin(), data.end());
	}

	فراغ find(ثابت RTLIL::SigSpec &sig, std::set<T> &result)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم) {
				auto &data = bits[bit];
				result.insert(data.begin(), data.end());
			}
	}

	فراغ find(ثابت RTLIL::SigSpec &sig, pool<T> &result)
	{
		لكل (ثابت auto &bit : sig)
			ܐܢ (bit.wire != عدم) {
				auto &data = bits[bit];
				result.insert(data.begin(), data.end());
			}
	}

	std::set<T> find(ثابت RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		ܦܢܐ result;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto &bit : sig)
			ܐܢ (bit.wire != عدم && bits.count(bit))
				ܦܢܐ true;
		ܦܢܐ false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::value>::type> : عام SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::value>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : عام SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

بنية SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	فراغ apply(RTLIL::SigBit &bit) ثابت
	{
		bit = database.find(bit);
	}

	فراغ apply(RTLIL::SigSpec &sig) ثابت
	{
		لكل (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ثابت
	{
		apply(bit);
		ܦܢܐ bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		ܦܢܐ sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ثابت
	{
		SigSpec sig(wire);
		apply(sig);
		ܦܢܐ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
بنية SigMap final : عام SigMapView
{
	SigMap(RTLIL::Module *module = عدم)
	{
		ܐܢ (module != عدم)
			set(module);
	}

	فراغ swap(SigMap &other)
	{
		database.swap(other.database);
	}

	فراغ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ set(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		لكل (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	فراغ add(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		لكل (صحيح i = 0; i < GetSize(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(to[i]);

			ثابت RTLIL::SigBit &bf = database[bfi];
			ثابت RTLIL::SigBit &bt = database[bti];

			ܐܢ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ܐܢ (bf.wire == nullptr)
					database.ipromote(bfi);

				ܐܢ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	فراغ add(ثابت RTLIL::SigBit &bit)
	{
		ثابت auto &b = database.find(bit);
		ܐܢ (b.wire != nullptr)
			database.promote(bit);
	}

	فراغ add(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت auto &bit : sig)
			add(bit);
	}

	مضمن فراغ add(Wire *wire) { ܦܢܐ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت auto &bit : database)
			ܐܢ (bit.wire != nullptr)
				sig.append(bit);
		ܦܢܐ sig;
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
بنية SigValMap final : عام SigMapView
{
	dict<SigBit, Val> values;

	فراغ swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		values.swap(other.values);
	}

	فراغ clear()
	{
		database.clear();
		values.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ set(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		values.clear();
		database.reserve(bitcount);

		لكل (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	فراغ add(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		لكل (صحيح i = 0; i < GetSize(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(to[i]);
			ܐܢ (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::SigBit &bf = database[bfi];
			ثابت RTLIL::SigBit &bt = database[bti];
			ܐܢ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_value(bt, bf);
			} ܐܚܪܢܐ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_value(bf, bt);
			}
		}
	}

	فراغ addVal(ثابت RTLIL::SigBit &bit, ثابت Val &val)
	{
		values[database.find(bit)] |= val;
	}

	فراغ addVal(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_value(RTLIL::SigBit &bit) ثابت
	{
		bit = database.find(bit);
		auto it = values.find(bit);
		ܦܢܐ it == values.end() ? Val() : it->second;
	}

خاص:
	فراغ merge_value(ثابت RTLIL::SigBit &from, ثابت RTLIL::SigBit &to)
	{
		auto it = values.find(from);
		ܐܢ (it == values.end()) {
			ܦܢܐ;
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
