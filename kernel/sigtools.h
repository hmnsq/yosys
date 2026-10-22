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

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

संरचना SigPool
{
	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णाङ्क> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णाङ्क>(रिक्त, 0) { }
		bitDef_t(ध्रुव RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first->name);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	pool<bitDef_t> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य add(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits.जोड़ो(bit);
	}

	शून्य add(ध्रुव SigPool &other)
	{
		प्रति (auto &bit : other.bits)
			bits.जोड़ो(bit);
	}

	शून्य del(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits.erase(bit);
	}

	शून्य del(ध्रुव SigPool &other)
	{
		प्रति (auto &bit : other.bits)
			bits.erase(bit);
	}

	शून्य expand(ध्रुव RTLIL::SigSpec &from, ध्रुव RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		प्रति (पूर्णाङ्क i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			यदि (bit_from.first != रिक्त && bit_to.first != रिक्त && bits.गिनती(bit_from) > 0)
				bits.जोड़ो(bit_to);
		}
	}

	RTLIL::SigSpec extract(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::SigSpec result;
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त && bits.गिनती(bit))
				result.append(bit);
		प्रत्यागम result;
	}

	RTLIL::SigSpec remove(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::SigSpec result;
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त && bits.गिनती(bit) == 0)
				result.append(bit);
		प्रत्यागम result;
	}

	bool check(ध्रुव RTLIL::SigBit &bit) ध्रुव
	{
		प्रत्यागम bit.wire != रिक्त && bits.गिनती(bit);
	}

	bool check_any(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त && bits.गिनती(bit))
				प्रत्यागम true;
		प्रत्यागम false;
	}

	bool check_all(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त && bits.गिनती(bit) == 0)
				प्रत्यागम false;
		प्रत्यागम true;
	}

	RTLIL::SigSpec export_one() ध्रुव
	{
		प्रति (auto &bit : bits)
			प्रत्यागम RTLIL::SigSpec(bit.first, bit.second);
		प्रत्यागम RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ध्रुव
	{
		pool<RTLIL::SigBit> sig;
		प्रति (auto &bit : bits)
			sig.जोड़ो(RTLIL::SigBit(bit.first, bit.second));
		प्रत्यागम sig;
	}

	size_t size() ध्रुव
	{
		प्रत्यागम bits.size();
	}
};

template <typename T, class Compare = शून्य>
संरचना SigSet
{
	static_assert(!std::is_same<Compare,शून्य>::मूल्य, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णाङ्क> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णाङ्क>(रिक्त, 0) { }
		bitDef_t(ध्रुव RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first->name);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य जोड़ो(ध्रुव RTLIL::SigSpec &sig, T data)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].जोड़ो(data);
	}

	शून्य जोड़ो(ध्रुव RTLIL::SigSpec& sig, ध्रुव std::set<T> &data)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].जोड़ो(data.begin(), data.end());
	}

	शून्य erase(ध्रुव RTLIL::SigSpec& sig)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].clear();
	}

	शून्य erase(ध्रुव RTLIL::SigSpec &sig, T data)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].erase(data);
	}

	शून्य erase(ध्रुव RTLIL::SigSpec &sig, ध्रुव std::set<T> &data)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].erase(data.begin(), data.end());
	}

	शून्य find(ध्रुव RTLIL::SigSpec &sig, std::set<T> &result)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त) {
				auto &data = bits[bit];
				result.जोड़ो(data.begin(), data.end());
			}
	}

	शून्य find(ध्रुव RTLIL::SigSpec &sig, pool<T> &result)
	{
		प्रति (ध्रुव auto &bit : sig)
			यदि (bit.wire != रिक्त) {
				auto &data = bits[bit];
				result.जोड़ो(data.begin(), data.end());
			}
	}

	std::set<T> find(ध्रुव RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		प्रत्यागम result;
	}

	bool has(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (auto &bit : sig)
			यदि (bit.wire != रिक्त && bits.गिनती(bit))
				प्रत्यागम true;
		प्रत्यागम false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::मूल्य>::type> : सार्वजनिक SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::मूल्य>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : सार्वजनिक SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

संरचना SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	शून्य apply(RTLIL::SigBit &bit) ध्रुव
	{
		bit = database.find(bit);
	}

	शून्य apply(RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ध्रुव
	{
		apply(bit);
		प्रत्यागम bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ध्रुव
	{
		apply(sig);
		प्रत्यागम sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ध्रुव
	{
		SigSpec sig(wire);
		apply(sig);
		प्रत्यागम sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
संरचना SigMap final : सार्वजनिक SigMapView
{
	SigMap(RTLIL::Module *module = रिक्त)
	{
		यदि (module != रिक्त)
			set(module);
	}

	शून्य swap(SigMap &other)
	{
		database.swap(other.database);
	}

	शून्य clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य set(RTLIL::Module *module)
	{
		पूर्णाङ्क bitcount = 0;
		प्रति (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		प्रति (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	शून्य add(ध्रुव RTLIL::SigSpec& from, ध्रुव RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		प्रति (पूर्णाङ्क i = 0; i < GetSize(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(to[i]);

			ध्रुव RTLIL::SigBit &bf = database[bfi];
			ध्रुव RTLIL::SigBit &bt = database[bti];

			यदि (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				यदि (bf.wire == nullptr)
					database.ipromote(bfi);

				यदि (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	शून्य add(ध्रुव RTLIL::SigBit &bit)
	{
		ध्रुव auto &b = database.find(bit);
		यदि (b.wire != nullptr)
			database.promote(bit);
	}

	शून्य add(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (ध्रुव auto &bit : sig)
			add(bit);
	}

	अन्तःस्थ शून्य add(Wire *wire) { प्रत्यागम add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ध्रुव
	{
		RTLIL::SigSpec sig;
		प्रति (ध्रुव auto &bit : database)
			यदि (bit.wire != nullptr)
				sig.append(bit);
		प्रत्यागम sig;
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
संरचना SigValMap final : सार्वजनिक SigMapView
{
	dict<SigBit, Val> मूल्यावली;

	शून्य swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		मूल्यावली.swap(other.मूल्यावली);
	}

	शून्य clear()
	{
		database.clear();
		मूल्यावली.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य set(RTLIL::Module *module)
	{
		पूर्णाङ्क bitcount = 0;
		प्रति (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		मूल्यावली.clear();
		database.reserve(bitcount);

		प्रति (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	शून्य add(ध्रुव RTLIL::SigSpec& from, ध्रुव RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		प्रति (पूर्णाङ्क i = 0; i < GetSize(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(to[i]);
			यदि (bfi == bti) {
				अनुवर्त;
			}

			ध्रुव RTLIL::SigBit &bf = database[bfi];
			ध्रुव RTLIL::SigBit &bt = database[bti];
			यदि (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_मूल्य(bt, bf);
			} अन्यथा {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_मूल्य(bf, bt);
			}
		}
	}

	शून्य addVal(ध्रुव RTLIL::SigBit &bit, ध्रुव Val &val)
	{
		मूल्यावली[database.find(bit)] |= val;
	}

	शून्य addVal(ध्रुव RTLIL::SigSpec &sig, ध्रुव Val &val)
	{
		प्रति (ध्रुव auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_मूल्य(RTLIL::SigBit &bit) ध्रुव
	{
		bit = database.find(bit);
		auto it = मूल्यावली.find(bit);
		प्रत्यागम it == मूल्यावली.end() ? Val() : it->second;
	}

गुप्त:
	शून्य merge_मूल्य(ध्रुव RTLIL::SigBit &from, ध्रुव RTLIL::SigBit &to)
	{
		auto it = मूल्यावली.find(from);
		यदि (it == मूल्यावली.end()) {
			प्रत्यागम;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		मूल्यावली.erase(it);
		मूल्यावली[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
