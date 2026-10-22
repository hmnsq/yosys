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
			أرجع h;
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
			إذا (bit.wire != عدم)
				bits.أدرج(bit);
	}

	فراغ add(ثابت SigPool &other)
	{
		لكل (auto &bit : other.bits)
			bits.أدرج(bit);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم)
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
			إذا (bit_from.first != عدم && bit_to.first != عدم && bits.عدد(bit_from) > 0)
				bits.أدرج(bit_to);
		}
	}

	RTLIL::SigSpec extract(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم && bits.عدد(bit))
				result.append(bit);
		أرجع result;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec result;
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم && bits.عدد(bit) == 0)
				result.append(bit);
		أرجع result;
	}

	bool check(ثابت RTLIL::SigBit &bit) ثابت
	{
		أرجع bit.wire != عدم && bits.عدد(bit);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم && bits.عدد(bit))
				أرجع true;
		أرجع false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم && bits.عدد(bit) == 0)
				أرجع false;
		أرجع true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (auto &bit : bits)
			أرجع RTLIL::SigSpec(bit.first, bit.second);
		أرجع RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::SigBit> sig;
		لكل (auto &bit : bits)
			sig.أدرج(RTLIL::SigBit(bit.first, bit.second));
		أرجع sig;
	}

	size_t size() ثابت
	{
		أرجع bits.size();
	}
};

template <typename T, class Compare = فراغ>
بنية SigSet
{
	static_assert(!std::is_same<Compare,فراغ>::قيمة, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية bitDef_t : عام std::pair<RTLIL::Wire*, صحيح> {
		bitDef_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		bitDef_t(ثابت RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, صحيح>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first->name);
			h.eat(second);
			أرجع h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	فراغ clear()
	{
		bits.clear();
	}

	فراغ أدرج(ثابت RTLIL::SigSpec &sig, T data)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم)
				bits[bit].أدرج(data);
	}

	فراغ أدرج(ثابت RTLIL::SigSpec& sig, ثابت std::set<T> &data)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم)
				bits[bit].أدرج(data.begin(), data.end());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم)
				bits[bit].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T data)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم)
				bits[bit].erase(data);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::set<T> &data)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم)
				bits[bit].erase(data.begin(), data.end());
	}

	فراغ find(ثابت RTLIL::SigSpec &sig, std::set<T> &result)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم) {
				auto &data = bits[bit];
				result.أدرج(data.begin(), data.end());
			}
	}

	فراغ find(ثابت RTLIL::SigSpec &sig, pool<T> &result)
	{
		لكل (ثابت auto &bit : sig)
			إذا (bit.wire != عدم) {
				auto &data = bits[bit];
				result.أدرج(data.begin(), data.end());
			}
	}

	std::set<T> find(ثابت RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		أرجع result;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (auto &bit : sig)
			إذا (bit.wire != عدم && bits.عدد(bit))
				أرجع true;
		أرجع false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::قيمة>::type> : عام SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::قيمة>::type;
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
		أرجع bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		أرجع sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ثابت
	{
		SigSpec sig(wire);
		apply(sig);
		أرجع sig;
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
		إذا (module != عدم)
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

			إذا (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				إذا (bf.wire == nullptr)
					database.ipromote(bfi);

				إذا (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	فراغ add(ثابت RTLIL::SigBit &bit)
	{
		ثابت auto &b = database.find(bit);
		إذا (b.wire != nullptr)
			database.promote(bit);
	}

	فراغ add(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت auto &bit : sig)
			add(bit);
	}

	مضمن فراغ add(Wire *wire) { أرجع add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت auto &bit : database)
			إذا (bit.wire != nullptr)
				sig.append(bit);
		أرجع sig;
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
	dict<SigBit, Val> قيم;

	فراغ swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		قيم.swap(other.قيم);
	}

	فراغ clear()
	{
		database.clear();
		قيم.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ set(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		قيم.clear();
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
			إذا (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::SigBit &bf = database[bfi];
			ثابت RTLIL::SigBit &bt = database[bti];
			إذا (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_قيمة(bt, bf);
			} وإلا {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_قيمة(bf, bt);
			}
		}
	}

	فراغ addVal(ثابت RTLIL::SigBit &bit, ثابت Val &val)
	{
		قيم[database.find(bit)] |= val;
	}

	فراغ addVal(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_قيمة(RTLIL::SigBit &bit) ثابت
	{
		bit = database.find(bit);
		auto it = قيم.find(bit);
		أرجع it == قيم.end() ? Val() : it->second;
	}

خاص:
	فراغ merge_قيمة(ثابت RTLIL::SigBit &from, ثابت RTLIL::SigBit &to)
	{
		auto it = قيم.find(from);
		إذا (it == قيم.end()) {
			أرجع;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		قيم.erase(it);
		قيم[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
