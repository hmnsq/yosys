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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ SigPool
{
	ಸಂರಚನಾ bitDef_t : ಸಾರ್ವಜನಿಕ std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ> {
		bitDef_t() : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ>(ರಿಕ್ತ, 0) { }
		bitDef_t(ಧ್ರುವ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(first->name);
			h.eat(second);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};

	pool<bitDef_t> bits;

	ಶೂನ್ಯ clear()
	{
		bits.clear();
	}

	ಶೂನ್ಯ add(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits.ಸೇರಿಸು(bit);
	}

	ಶೂನ್ಯ add(ಧ್ರುವ SigPool &other)
	{
		ಪ್ರತಿ (auto &bit : other.bits)
			bits.ಸೇರಿಸು(bit);
	}

	ಶೂನ್ಯ del(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits.erase(bit);
	}

	ಶೂನ್ಯ del(ಧ್ರುವ SigPool &other)
	{
		ಪ್ರತಿ (auto &bit : other.bits)
			bits.erase(bit);
	}

	ಶೂನ್ಯ expand(ಧ್ರುವ RTLIL::SigSpec &from, ಧ್ರುವ RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			ಯದಿ (bit_from.first != ರಿಕ್ತ && bit_to.first != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit_from) > 0)
				bits.ಸೇರಿಸು(bit_to);
		}
	}

	RTLIL::SigSpec extract(ಧ್ರುವ RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		RTLIL::SigSpec result;
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit))
				result.append(bit);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	RTLIL::SigSpec remove(ಧ್ರುವ RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		RTLIL::SigSpec result;
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit) == 0)
				result.append(bit);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	bool check(ಧ್ರುವ RTLIL::SigBit &bit) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit);
	}

	bool check_any(ಧ್ರುವ RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit))
				ಪ್ರತ್ಯಾಗಮ true;
		ಪ್ರತ್ಯಾಗಮ false;
	}

	bool check_all(ಧ್ರುವ RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit) == 0)
				ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ true;
	}

	RTLIL::SigSpec export_one() ಧ್ರುವ
	{
		ಪ್ರತಿ (auto &bit : bits)
			ಪ್ರತ್ಯಾಗಮ RTLIL::SigSpec(bit.first, bit.second);
		ಪ್ರತ್ಯಾಗಮ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ಧ್ರುವ
	{
		pool<RTLIL::SigBit> sig;
		ಪ್ರತಿ (auto &bit : bits)
			sig.ಸೇರಿಸು(RTLIL::SigBit(bit.first, bit.second));
		ಪ್ರತ್ಯಾಗಮ sig;
	}

	size_t size() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ bits.size();
	}
};

template <typename T, class Compare = ಶೂನ್ಯ>
ಸಂರಚನಾ SigSet
{
	static_assert(!std::is_same<Compare,ಶೂನ್ಯ>::ಮೌಲ್ಯ, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ಸಂರಚನಾ bitDef_t : ಸಾರ್ವಜನಿಕ std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ> {
		bitDef_t() : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ>(ರಿಕ್ತ, 0) { }
		bitDef_t(ಧ್ರುವ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಙ್ಕ>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(first->name);
			h.eat(second);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	ಶೂನ್ಯ clear()
	{
		bits.clear();
	}

	ಶೂನ್ಯ ಸೇರಿಸು(ಧ್ರುವ RTLIL::SigSpec &sig, T data)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits[bit].ಸೇರಿಸು(data);
	}

	ಶೂನ್ಯ ಸೇರಿಸು(ಧ್ರುವ RTLIL::SigSpec& sig, ಧ್ರುವ std::set<T> &data)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits[bit].ಸೇರಿಸು(data.begin(), data.end());
	}

	ಶೂನ್ಯ erase(ಧ್ರುವ RTLIL::SigSpec& sig)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits[bit].clear();
	}

	ಶೂನ್ಯ erase(ಧ್ರುವ RTLIL::SigSpec &sig, T data)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits[bit].erase(data);
	}

	ಶೂನ್ಯ erase(ಧ್ರುವ RTLIL::SigSpec &sig, ಧ್ರುವ std::set<T> &data)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				bits[bit].erase(data.begin(), data.end());
	}

	ಶೂನ್ಯ find(ಧ್ರುವ RTLIL::SigSpec &sig, std::set<T> &result)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ) {
				auto &data = bits[bit];
				result.ಸೇರಿಸು(data.begin(), data.end());
			}
	}

	ಶೂನ್ಯ find(ಧ್ರುವ RTLIL::SigSpec &sig, pool<T> &result)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ) {
				auto &data = bits[bit];
				result.ಸೇರಿಸು(data.begin(), data.end());
			}
	}

	std::set<T> find(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	bool has(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire != ರಿಕ್ತ && bits.ಎಣಿಕೆ(bit))
				ಪ್ರತ್ಯಾಗಮ true;
		ಪ್ರತ್ಯಾಗಮ false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::ಮೌಲ್ಯ>::type> : ಸಾರ್ವಜನಿಕ SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::ಮೌಲ್ಯ>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : ಸಾರ್ವಜನಿಕ SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

ಸಂರಚನಾ SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	ಶೂನ್ಯ apply(RTLIL::SigBit &bit) ಧ್ರುವ
	{
		bit = database.find(bit);
	}

	ಶೂನ್ಯ apply(RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		ಪ್ರತಿ (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ಧ್ರುವ
	{
		apply(bit);
		ಪ್ರತ್ಯಾಗಮ bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ಧ್ರುವ
	{
		apply(sig);
		ಪ್ರತ್ಯಾಗಮ sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ಧ್ರುವ
	{
		SigSpec sig(wire);
		apply(sig);
		ಪ್ರತ್ಯಾಗಮ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ಸಂರಚನಾ SigMap final : ಸಾರ್ವಜನಿಕ SigMapView
{
	SigMap(RTLIL::Module *module = ರಿಕ್ತ)
	{
		ಯದಿ (module != ರಿಕ್ತ)
			set(module);
	}

	ಶೂನ್ಯ swap(SigMap &other)
	{
		database.swap(other.database);
	}

	ಶೂನ್ಯ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	ಶೂನ್ಯ set(RTLIL::Module *module)
	{
		ಪೂರ್ಣಾಙ್ಕ bitcount = 0;
		ಪ್ರತಿ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		ಪ್ರತಿ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	ಶೂನ್ಯ add(ಧ್ರುವ RTLIL::SigSpec& from, ಧ್ರುವ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(from); i++)
		{
			ಪೂರ್ಣಾಙ್ಕ bfi = database.lookup(from[i]);
			ಪೂರ್ಣಾಙ್ಕ bti = database.lookup(to[i]);

			ಧ್ರುವ RTLIL::SigBit &bf = database[bfi];
			ಧ್ರುವ RTLIL::SigBit &bt = database[bti];

			ಯದಿ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ಯದಿ (bf.wire == nullptr)
					database.ipromote(bfi);

				ಯದಿ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	ಶೂನ್ಯ add(ಧ್ರುವ RTLIL::SigBit &bit)
	{
		ಧ್ರುವ auto &b = database.find(bit);
		ಯದಿ (b.wire != nullptr)
			database.promote(bit);
	}

	ಶೂನ್ಯ add(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			add(bit);
	}

	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ add(Wire *wire) { ಪ್ರತ್ಯಾಗಮ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ಧ್ರುವ
	{
		RTLIL::SigSpec sig;
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : database)
			ಯದಿ (bit.wire != nullptr)
				sig.append(bit);
		ಪ್ರತ್ಯಾಗಮ sig;
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
ಸಂರಚನಾ SigValMap final : ಸಾರ್ವಜನಿಕ SigMapView
{
	dict<SigBit, Val> ಮೌಲ್ಯಗಳು;

	ಶೂನ್ಯ swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		ಮೌಲ್ಯಗಳು.swap(other.ಮೌಲ್ಯಗಳು);
	}

	ಶೂನ್ಯ clear()
	{
		database.clear();
		ಮೌಲ್ಯಗಳು.clear();
	}

	// Rebuild SigMap for all connections in module
	ಶೂನ್ಯ set(RTLIL::Module *module)
	{
		ಪೂರ್ಣಾಙ್ಕ bitcount = 0;
		ಪ್ರತಿ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		ಮೌಲ್ಯಗಳು.clear();
		database.reserve(bitcount);

		ಪ್ರತಿ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	ಶೂನ್ಯ add(ಧ್ರುವ RTLIL::SigSpec& from, ಧ್ರುವ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(from); i++)
		{
			ಪೂರ್ಣಾಙ್ಕ bfi = database.lookup(from[i]);
			ಪೂರ್ಣಾಙ್ಕ bti = database.lookup(to[i]);
			ಯದಿ (bfi == bti) {
				ಅನುವರ್ತ;
			}

			ಧ್ರುವ RTLIL::SigBit &bf = database[bfi];
			ಧ್ರುವ RTLIL::SigBit &bt = database[bti];
			ಯದಿ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_ಮೌಲ್ಯ(bt, bf);
			} ಅನ್ಯಥಾ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_ಮೌಲ್ಯ(bf, bt);
			}
		}
	}

	ಶೂನ್ಯ addVal(ಧ್ರುವ RTLIL::SigBit &bit, ಧ್ರುವ Val &val)
	{
		ಮೌಲ್ಯಗಳು[database.find(bit)] |= val;
	}

	ಶೂನ್ಯ addVal(ಧ್ರುವ RTLIL::SigSpec &sig, ಧ್ರುವ Val &val)
	{
		ಪ್ರತಿ (ಧ್ರುವ auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_ಮೌಲ್ಯ(RTLIL::SigBit &bit) ಧ್ರುವ
	{
		bit = database.find(bit);
		auto it = ಮೌಲ್ಯಗಳು.find(bit);
		ಪ್ರತ್ಯಾಗಮ it == ಮೌಲ್ಯಗಳು.end() ? Val() : it->second;
	}

ಗುಪ್ತ:
	ಶೂನ್ಯ merge_ಮೌಲ್ಯ(ಧ್ರುವ RTLIL::SigBit &from, ಧ್ರುವ RTLIL::SigBit &to)
	{
		auto it = ಮೌಲ್ಯಗಳು.find(from);
		ಯದಿ (it == ಮೌಲ್ಯಗಳು.end()) {
			ಪ್ರತ್ಯಾಗಮ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		ಮೌಲ್ಯಗಳು.erase(it);
		ಮೌಲ್ಯಗಳು[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
