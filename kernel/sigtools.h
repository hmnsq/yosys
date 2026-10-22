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
	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णांक> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णांक>(रिक्त, 0) { }
		bitDef_t(स्थिरांक RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णांक>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(first->name);
			h.eat(second);
			परत h;
		}
	};

	pool<bitDef_t> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य add(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits.घाला(bit);
	}

	शून्य add(स्थिरांक SigPool &other)
	{
		साठी (स्वतः &bit : other.bits)
			bits.घाला(bit);
	}

	शून्य del(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits.erase(bit);
	}

	शून्य del(स्थिरांक SigPool &other)
	{
		साठी (स्वतः &bit : other.bits)
			bits.erase(bit);
	}

	शून्य expand(स्थिरांक RTLIL::SigSpec &from, स्थिरांक RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		साठी (पूर्णांक i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			जर (bit_from.first != रिक्त && bit_to.first != रिक्त && bits.मोजणी(bit_from) > 0)
				bits.घाला(bit_to);
		}
	}

	RTLIL::SigSpec extract(स्थिरांक RTLIL::SigSpec &sig) स्थिरांक
	{
		RTLIL::SigSpec result;
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त && bits.मोजणी(bit))
				result.append(bit);
		परत result;
	}

	RTLIL::SigSpec हटाओ(स्थिरांक RTLIL::SigSpec &sig) स्थिरांक
	{
		RTLIL::SigSpec result;
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त && bits.मोजणी(bit) == 0)
				result.append(bit);
		परत result;
	}

	bool check(स्थिरांक RTLIL::SigBit &bit) स्थिरांक
	{
		परत bit.wire != रिक्त && bits.मोजणी(bit);
	}

	bool check_any(स्थिरांक RTLIL::SigSpec &sig) स्थिरांक
	{
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त && bits.मोजणी(bit))
				परत true;
		परत false;
	}

	bool check_all(स्थिरांक RTLIL::SigSpec &sig) स्थिरांक
	{
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त && bits.मोजणी(bit) == 0)
				परत false;
		परत true;
	}

	RTLIL::SigSpec export_one() स्थिरांक
	{
		साठी (स्वतः &bit : bits)
			परत RTLIL::SigSpec(bit.first, bit.second);
		परत RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() स्थिरांक
	{
		pool<RTLIL::SigBit> sig;
		साठी (स्वतः &bit : bits)
			sig.घाला(RTLIL::SigBit(bit.first, bit.second));
		परत sig;
	}

	माप_प्रकार size() स्थिरांक
	{
		परत bits.size();
	}
};

ढाँचा <typename T, class Compare = शून्य>
संरचना SigSet
{
	static_assert(!std::is_same<Compare,शून्य>::मूल्य, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णांक> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णांक>(रिक्त, 0) { }
		bitDef_t(स्थिरांक RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णांक>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(first->name);
			h.eat(second);
			परत h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	शून्य clear()
	{
		bits.clear();
	}

	शून्य घाला(स्थिरांक RTLIL::SigSpec &sig, T data)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits[bit].घाला(data);
	}

	शून्य घाला(स्थिरांक RTLIL::SigSpec& sig, स्थिरांक std::set<T> &data)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits[bit].घाला(data.begin(), data.end());
	}

	शून्य erase(स्थिरांक RTLIL::SigSpec& sig)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits[bit].clear();
	}

	शून्य erase(स्थिरांक RTLIL::SigSpec &sig, T data)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits[bit].erase(data);
	}

	शून्य erase(स्थिरांक RTLIL::SigSpec &sig, स्थिरांक std::set<T> &data)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त)
				bits[bit].erase(data.begin(), data.end());
	}

	शून्य find(स्थिरांक RTLIL::SigSpec &sig, std::set<T> &result)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त) {
				स्वतः &data = bits[bit];
				result.घाला(data.begin(), data.end());
			}
	}

	शून्य find(स्थिरांक RTLIL::SigSpec &sig, pool<T> &result)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			जर (bit.wire != रिक्त) {
				स्वतः &data = bits[bit];
				result.घाला(data.begin(), data.end());
			}
	}

	std::set<T> find(स्थिरांक RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		परत result;
	}

	bool has(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्वतः &bit : sig)
			जर (bit.wire != रिक्त && bits.मोजणी(bit))
				परत true;
		परत false;
	}
};

ढाँचा<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::मूल्य>::type> : सार्वजनिक SigSet<T, std::less<T>> {};
ढाँचा<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::मूल्य>::type;
ढाँचा<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : सार्वजनिक SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

संरचना SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	शून्य apply(RTLIL::SigBit &bit) स्थिरांक
	{
		bit = database.find(bit);
	}

	शून्य apply(RTLIL::SigSpec &sig) स्थिरांक
	{
		साठी (स्वतः &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit चालक()(RTLIL::SigBit bit) स्थिरांक
	{
		apply(bit);
		परत bit;
	}

	RTLIL::SigSpec चालक()(RTLIL::SigSpec sig) स्थिरांक
	{
		apply(sig);
		परत sig;
	}

	RTLIL::SigSpec चालक()(RTLIL::Wire *wire) स्थिरांक
	{
		SigSpec sig(wire);
		apply(sig);
		परत sig;
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
		जर (module != रिक्त)
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
		पूर्णांक bitcount = 0;
		साठी (स्वतः &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		साठी (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	शून्य add(स्थिरांक RTLIL::SigSpec& from, स्थिरांक RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		साठी (पूर्णांक i = 0; i < GetSize(from); i++)
		{
			पूर्णांक bfi = database.lookup(from[i]);
			पूर्णांक bti = database.lookup(to[i]);

			स्थिरांक RTLIL::SigBit &bf = database[bfi];
			स्थिरांक RTLIL::SigBit &bt = database[bti];

			जर (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				जर (bf.wire == nullptr)
					database.ipromote(bfi);

				जर (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	शून्य add(स्थिरांक RTLIL::SigBit &bit)
	{
		स्थिरांक स्वतः &b = database.find(bit);
		जर (b.wire != nullptr)
			database.promote(bit);
	}

	शून्य add(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			add(bit);
	}

	ओळीत शून्य add(Wire *wire) { परत add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() स्थिरांक
	{
		RTLIL::SigSpec sig;
		साठी (स्थिरांक स्वतः &bit : database)
			जर (bit.wire != nullptr)
				sig.append(bit);
		परत sig;
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
	dict<SigBit, Val> मूल्ये;

	शून्य swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		मूल्ये.swap(other.मूल्ये);
	}

	शून्य clear()
	{
		database.clear();
		मूल्ये.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य set(RTLIL::Module *module)
	{
		पूर्णांक bitcount = 0;
		साठी (स्वतः &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		मूल्ये.clear();
		database.reserve(bitcount);

		साठी (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	शून्य add(स्थिरांक RTLIL::SigSpec& from, स्थिरांक RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		साठी (पूर्णांक i = 0; i < GetSize(from); i++)
		{
			पूर्णांक bfi = database.lookup(from[i]);
			पूर्णांक bti = database.lookup(to[i]);
			जर (bfi == bti) {
				सुरूठेवा;
			}

			स्थिरांक RTLIL::SigBit &bf = database[bfi];
			स्थिरांक RTLIL::SigBit &bt = database[bti];
			जर (bf.wire == nullptr) {
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

	शून्य addVal(स्थिरांक RTLIL::SigBit &bit, स्थिरांक Val &val)
	{
		मूल्ये[database.find(bit)] |= val;
	}

	शून्य addVal(स्थिरांक RTLIL::SigSpec &sig, स्थिरांक Val &val)
	{
		साठी (स्थिरांक स्वतः &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_मूल्य(RTLIL::SigBit &bit) स्थिरांक
	{
		bit = database.find(bit);
		स्वतः it = मूल्ये.find(bit);
		परत it == मूल्ये.end() ? Val() : it->second;
	}

खाजगी:
	शून्य merge_मूल्य(स्थिरांक RTLIL::SigBit &from, स्थिरांक RTLIL::SigBit &to)
	{
		स्वतः it = मूल्ये.find(from);
		जर (it == मूल्ये.end()) {
			परत;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		मूल्ये.erase(it);
		मूल्ये[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
