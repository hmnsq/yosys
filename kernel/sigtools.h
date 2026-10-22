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
		bitDef_t(अचर RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) अचर {
			h.eat(first->name);
			h.eat(second);
			फर्काऊ h;
		}
	};

	pool<bitDef_t> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य add(अचर RTLIL::SigSpec &sig)
	{
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits.घुसाउ(bit);
	}

	शून्य add(अचर SigPool &other)
	{
		लागि (स्वतः &bit : other.bits)
			bits.घुसाउ(bit);
	}

	शून्य del(अचर RTLIL::SigSpec &sig)
	{
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits.erase(bit);
	}

	शून्य del(अचर SigPool &other)
	{
		लागि (स्वतः &bit : other.bits)
			bits.erase(bit);
	}

	शून्य expand(अचर RTLIL::SigSpec &from, अचर RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		लागि (पूर्णाङ्क i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			यदि (bit_from.first != रिक्त && bit_to.first != रिक्त && bits.गन्ती(bit_from) > 0)
				bits.घुसाउ(bit_to);
		}
	}

	RTLIL::SigSpec extract(अचर RTLIL::SigSpec &sig) अचर
	{
		RTLIL::SigSpec result;
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त && bits.गन्ती(bit))
				result.append(bit);
		फर्काऊ result;
	}

	RTLIL::SigSpec हटाओ(अचर RTLIL::SigSpec &sig) अचर
	{
		RTLIL::SigSpec result;
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त && bits.गन्ती(bit) == 0)
				result.append(bit);
		फर्काऊ result;
	}

	bool check(अचर RTLIL::SigBit &bit) अचर
	{
		फर्काऊ bit.wire != रिक्त && bits.गन्ती(bit);
	}

	bool check_any(अचर RTLIL::SigSpec &sig) अचर
	{
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त && bits.गन्ती(bit))
				फर्काऊ true;
		फर्काऊ false;
	}

	bool check_all(अचर RTLIL::SigSpec &sig) अचर
	{
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त && bits.गन्ती(bit) == 0)
				फर्काऊ false;
		फर्काऊ true;
	}

	RTLIL::SigSpec export_one() अचर
	{
		लागि (स्वतः &bit : bits)
			फर्काऊ RTLIL::SigSpec(bit.first, bit.second);
		फर्काऊ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() अचर
	{
		pool<RTLIL::SigBit> sig;
		लागि (स्वतः &bit : bits)
			sig.घुसाउ(RTLIL::SigBit(bit.first, bit.second));
		फर्काऊ sig;
	}

	माप_प्रकार size() अचर
	{
		फर्काऊ bits.size();
	}
};

ढाँचा <typename T, class Compare = शून्य>
संरचना SigSet
{
	static_assert(!std::is_same<Compare,शून्य>::मान, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णाङ्क> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णाङ्क>(रिक्त, 0) { }
		bitDef_t(अचर RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) अचर {
			h.eat(first->name);
			h.eat(second);
			फर्काऊ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य घुसाउ(अचर RTLIL::SigSpec &sig, T data)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].घुसाउ(data);
	}

	शून्य घुसाउ(अचर RTLIL::SigSpec& sig, अचर std::set<T> &data)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].घुसाउ(data.begin(), data.end());
	}

	शून्य erase(अचर RTLIL::SigSpec& sig)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].clear();
	}

	शून्य erase(अचर RTLIL::SigSpec &sig, T data)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].erase(data);
	}

	शून्य erase(अचर RTLIL::SigSpec &sig, अचर std::set<T> &data)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त)
				bits[bit].erase(data.begin(), data.end());
	}

	शून्य find(अचर RTLIL::SigSpec &sig, std::set<T> &result)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त) {
				स्वतः &data = bits[bit];
				result.घुसाउ(data.begin(), data.end());
			}
	}

	शून्य find(अचर RTLIL::SigSpec &sig, pool<T> &result)
	{
		लागि (अचर स्वतः &bit : sig)
			यदि (bit.wire != रिक्त) {
				स्वतः &data = bits[bit];
				result.घुसाउ(data.begin(), data.end());
			}
	}

	std::set<T> find(अचर RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		फर्काऊ result;
	}

	bool has(अचर RTLIL::SigSpec &sig)
	{
		लागि (स्वतः &bit : sig)
			यदि (bit.wire != रिक्त && bits.गन्ती(bit))
				फर्काऊ true;
		फर्काऊ false;
	}
};

ढाँचा<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::मान>::type> : सार्वजनिक SigSet<T, std::less<T>> {};
ढाँचा<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::मान>::type;
ढाँचा<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : सार्वजनिक SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

संरचना SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	शून्य apply(RTLIL::SigBit &bit) अचर
	{
		bit = database.find(bit);
	}

	शून्य apply(RTLIL::SigSpec &sig) अचर
	{
		लागि (स्वतः &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit चालक()(RTLIL::SigBit bit) अचर
	{
		apply(bit);
		फर्काऊ bit;
	}

	RTLIL::SigSpec चालक()(RTLIL::SigSpec sig) अचर
	{
		apply(sig);
		फर्काऊ sig;
	}

	RTLIL::SigSpec चालक()(RTLIL::Wire *wire) अचर
	{
		SigSpec sig(wire);
		apply(sig);
		फर्काऊ sig;
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
		लागि (स्वतः &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		लागि (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	शून्य add(अचर RTLIL::SigSpec& from, अचर RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		लागि (पूर्णाङ्क i = 0; i < GetSize(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(to[i]);

			अचर RTLIL::SigBit &bf = database[bfi];
			अचर RTLIL::SigBit &bt = database[bti];

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
	शून्य add(अचर RTLIL::SigBit &bit)
	{
		अचर स्वतः &b = database.find(bit);
		यदि (b.wire != nullptr)
			database.promote(bit);
	}

	शून्य add(अचर RTLIL::SigSpec &sig)
	{
		लागि (अचर स्वतः &bit : sig)
			add(bit);
	}

	अन्तःस्थ शून्य add(Wire *wire) { फर्काऊ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() अचर
	{
		RTLIL::SigSpec sig;
		लागि (अचर स्वतः &bit : database)
			यदि (bit.wire != nullptr)
				sig.append(bit);
		फर्काऊ sig;
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
ढाँचा <class Val>
संरचना SigValMap final : सार्वजनिक SigMapView
{
	dict<SigBit, Val> मानहरू;

	शून्य swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		मानहरू.swap(other.मानहरू);
	}

	शून्य clear()
	{
		database.clear();
		मानहरू.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य set(RTLIL::Module *module)
	{
		पूर्णाङ्क bitcount = 0;
		लागि (स्वतः &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		मानहरू.clear();
		database.reserve(bitcount);

		लागि (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	शून्य add(अचर RTLIL::SigSpec& from, अचर RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		लागि (पूर्णाङ्क i = 0; i < GetSize(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(to[i]);
			यदि (bfi == bti) {
				जारी;
			}

			अचर RTLIL::SigBit &bf = database[bfi];
			अचर RTLIL::SigBit &bt = database[bti];
			यदि (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_मान(bt, bf);
			} अन्यथा {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_मान(bf, bt);
			}
		}
	}

	शून्य addVal(अचर RTLIL::SigBit &bit, अचर Val &val)
	{
		मानहरू[database.find(bit)] |= val;
	}

	शून्य addVal(अचर RTLIL::SigSpec &sig, अचर Val &val)
	{
		लागि (अचर स्वतः &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_मान(RTLIL::SigBit &bit) अचर
	{
		bit = database.find(bit);
		स्वतः it = मानहरू.find(bit);
		फर्काऊ it == मानहरू.end() ? Val() : it->second;
	}

गुप्त:
	शून्य merge_मान(अचर RTLIL::SigBit &from, अचर RTLIL::SigBit &to)
	{
		स्वतः it = मानहरू.find(from);
		यदि (it == मानहरू.end()) {
			फर्काऊ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		मानहरू.erase(it);
		मानहरू[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
