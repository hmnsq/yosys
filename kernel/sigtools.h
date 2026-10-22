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

#अगर_अघोषित SIGTOOLS_H
#घोषणा SIGTOOLS_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा SigPool
{
	काष्ठा bitDef_t : खुला std::pair<RTLIL::Wire*, पूर्णांक> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णांक>(शून्य, 0) { }
		bitDef_t(स्थिर RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णांक>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(first->name);
			h.eat(second);
			वापस h;
		}
	};

	pool<bitDef_t> bits;

	व्योम clear()
	{
		bits.clear();
	}

	व्योम add(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits.जोड़ो(bit);
	}

	व्योम add(स्थिर SigPool &other)
	{
		क्रम (स्वतः &bit : other.bits)
			bits.जोड़ो(bit);
	}

	व्योम del(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits.erase(bit);
	}

	व्योम del(स्थिर SigPool &other)
	{
		क्रम (स्वतः &bit : other.bits)
			bits.erase(bit);
	}

	व्योम expand(स्थिर RTLIL::SigSpec &from, स्थिर RTLIL::SigSpec &to)
	{
		log_assert(Get_आकार(from) == Get_आकार(to));
		क्रम (पूर्णांक i = 0; i < Get_आकार(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			अगर (bit_from.first != शून्य && bit_to.first != शून्य && bits.गिनती(bit_from) > 0)
				bits.जोड़ो(bit_to);
		}
	}

	RTLIL::SigSpec extract(स्थिर RTLIL::SigSpec &sig) स्थिर
	{
		RTLIL::SigSpec परिणाम;
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य && bits.गिनती(bit))
				परिणाम.append(bit);
		वापस परिणाम;
	}

	RTLIL::SigSpec हटाओ(स्थिर RTLIL::SigSpec &sig) स्थिर
	{
		RTLIL::SigSpec परिणाम;
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य && bits.गिनती(bit) == 0)
				परिणाम.append(bit);
		वापस परिणाम;
	}

	bool check(स्थिर RTLIL::SigBit &bit) स्थिर
	{
		वापस bit.wire != शून्य && bits.गिनती(bit);
	}

	bool check_any(स्थिर RTLIL::SigSpec &sig) स्थिर
	{
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य && bits.गिनती(bit))
				वापस true;
		वापस false;
	}

	bool check_all(स्थिर RTLIL::SigSpec &sig) स्थिर
	{
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य && bits.गिनती(bit) == 0)
				वापस false;
		वापस true;
	}

	RTLIL::SigSpec export_one() स्थिर
	{
		क्रम (स्वतः &bit : bits)
			वापस RTLIL::SigSpec(bit.first, bit.second);
		वापस RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() स्थिर
	{
		pool<RTLIL::SigBit> sig;
		क्रम (स्वतः &bit : bits)
			sig.जोड़ो(RTLIL::SigBit(bit.first, bit.second));
		वापस sig;
	}

	माप_प्रकार आकार() स्थिर
	{
		वापस bits.आकार();
	}
};

ढाँचा <typename T, class Compare = व्योम>
काष्ठा SigSet
{
	static_assert(!std::is_same<Compare,व्योम>::मूल्य, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	काष्ठा bitDef_t : खुला std::pair<RTLIL::Wire*, पूर्णांक> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णांक>(शून्य, 0) { }
		bitDef_t(स्थिर RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णांक>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(first->name);
			h.eat(second);
			वापस h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	व्योम clear()
	{
		bits.clear();
	}

	व्योम जोड़ो(स्थिर RTLIL::SigSpec &sig, T तथ्य)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits[bit].जोड़ो(तथ्य);
	}

	व्योम जोड़ो(स्थिर RTLIL::SigSpec& sig, स्थिर std::set<T> &तथ्य)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits[bit].जोड़ो(तथ्य.begin(), तथ्य.end());
	}

	व्योम erase(स्थिर RTLIL::SigSpec& sig)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits[bit].clear();
	}

	व्योम erase(स्थिर RTLIL::SigSpec &sig, T तथ्य)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits[bit].erase(तथ्य);
	}

	व्योम erase(स्थिर RTLIL::SigSpec &sig, स्थिर std::set<T> &तथ्य)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य)
				bits[bit].erase(तथ्य.begin(), तथ्य.end());
	}

	व्योम find(स्थिर RTLIL::SigSpec &sig, std::set<T> &परिणाम)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य) {
				स्वतः &तथ्य = bits[bit];
				परिणाम.जोड़ो(तथ्य.begin(), तथ्य.end());
			}
	}

	व्योम find(स्थिर RTLIL::SigSpec &sig, pool<T> &परिणाम)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			अगर (bit.wire != शून्य) {
				स्वतः &तथ्य = bits[bit];
				परिणाम.जोड़ो(तथ्य.begin(), तथ्य.end());
			}
	}

	std::set<T> find(स्थिर RTLIL::SigSpec &sig)
	{
		std::set<T> परिणाम;
		find(sig, परिणाम);
		वापस परिणाम;
	}

	bool has(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्वतः &bit : sig)
			अगर (bit.wire != शून्य && bits.गिनती(bit))
				वापस true;
		वापस false;
	}
};

ढाँचा<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::मूल्य>::type> : खुला SigSet<T, std::less<T>> {};
ढाँचा<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::मूल्य>::type;
ढाँचा<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : खुला SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

काष्ठा SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	व्योम apply(RTLIL::SigBit &bit) स्थिर
	{
		bit = database.find(bit);
	}

	व्योम apply(RTLIL::SigSpec &sig) स्थिर
	{
		क्रम (स्वतः &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit चालक()(RTLIL::SigBit bit) स्थिर
	{
		apply(bit);
		वापस bit;
	}

	RTLIL::SigSpec चालक()(RTLIL::SigSpec sig) स्थिर
	{
		apply(sig);
		वापस sig;
	}

	RTLIL::SigSpec चालक()(RTLIL::Wire *wire) स्थिर
	{
		SigSpec sig(wire);
		apply(sig);
		वापस sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
काष्ठा SigMap final : खुला SigMapView
{
	SigMap(RTLIL::Module *module = शून्य)
	{
		अगर (module != शून्य)
			set(module);
	}

	व्योम swap(SigMap &other)
	{
		database.swap(other.database);
	}

	व्योम clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	व्योम set(RTLIL::Module *module)
	{
		पूर्णांक bitcount = 0;
		क्रम (स्वतः &it : module->connections())
			bitcount += it.first.आकार();

		database.clear();
		database.reserve(bitcount);

		क्रम (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	व्योम add(स्थिर RTLIL::SigSpec& from, स्थिर RTLIL::SigSpec& to)
	{
		log_assert(Get_आकार(from) == Get_आकार(to));

		क्रम (पूर्णांक i = 0; i < Get_आकार(from); i++)
		{
			पूर्णांक bfi = database.lookup(from[i]);
			पूर्णांक bti = database.lookup(to[i]);

			स्थिर RTLIL::SigBit &bf = database[bfi];
			स्थिर RTLIL::SigBit &bt = database[bti];

			अगर (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				अगर (bf.wire == nullptr)
					database.ipromote(bfi);

				अगर (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	व्योम add(स्थिर RTLIL::SigBit &bit)
	{
		स्थिर स्वतः &b = database.find(bit);
		अगर (b.wire != nullptr)
			database.promote(bit);
	}

	व्योम add(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			add(bit);
	}

	अंतरभूत व्योम add(Wire *wire) { वापस add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() स्थिर
	{
		RTLIL::SigSpec sig;
		क्रम (स्थिर स्वतः &bit : database)
			अगर (bit.wire != nullptr)
				sig.append(bit);
		वापस sig;
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
काष्ठा SigValMap final : खुला SigMapView
{
	dict<SigBit, Val> मूल्यावली;

	व्योम swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		मूल्यावली.swap(other.मूल्यावली);
	}

	व्योम clear()
	{
		database.clear();
		मूल्यावली.clear();
	}

	// Rebuild SigMap for all connections in module
	व्योम set(RTLIL::Module *module)
	{
		पूर्णांक bitcount = 0;
		क्रम (स्वतः &it : module->connections())
			bitcount += it.first.आकार();

		database.clear();
		मूल्यावली.clear();
		database.reserve(bitcount);

		क्रम (स्वतः &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	व्योम add(स्थिर RTLIL::SigSpec& from, स्थिर RTLIL::SigSpec& to)
	{
		log_assert(Get_आकार(from) == Get_आकार(to));

		क्रम (पूर्णांक i = 0; i < Get_आकार(from); i++)
		{
			पूर्णांक bfi = database.lookup(from[i]);
			पूर्णांक bti = database.lookup(to[i]);
			अगर (bfi == bti) {
				जारी;
			}

			स्थिर RTLIL::SigBit &bf = database[bfi];
			स्थिर RTLIL::SigBit &bt = database[bti];
			अगर (bf.wire == nullptr) {
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

	व्योम addVal(स्थिर RTLIL::SigBit &bit, स्थिर Val &val)
	{
		मूल्यावली[database.find(bit)] |= val;
	}

	व्योम addVal(स्थिर RTLIL::SigSpec &sig, स्थिर Val &val)
	{
		क्रम (स्थिर स्वतः &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_मूल्य(RTLIL::SigBit &bit) स्थिर
	{
		bit = database.find(bit);
		स्वतः it = मूल्यावली.find(bit);
		वापस it == मूल्यावली.end() ? Val() : it->second;
	}

निजी:
	व्योम merge_मूल्य(स्थिर RTLIL::SigBit &from, स्थिर RTLIL::SigBit &to)
	{
		स्वतः it = मूल्यावली.find(from);
		अगर (it == मूल्यावली.end()) {
			वापस;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		मूल्यावली.erase(it);
		मूल्यावली[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर /* SIGTOOLS_H */
