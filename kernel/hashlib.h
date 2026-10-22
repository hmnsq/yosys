// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#నిర్వచితం_కాకపోతే HASHLIB_H
#నిర్వచించు HASHLIB_H

#అన్తర్భావ <array>
#అన్తర్భావ <atomic>
#అన్తర్భావ <stdexcept>
#అన్తర్భావ <algorithm>
#అన్తర్భావ <optional>
#అన్తర్భావ <string>
#అన్తర్భావ <variant>
#అన్తర్భావ <vector>
#అన్తర్భావ <type_traits>
#అన్తర్భావ <stdint.h>

#నిర్వచించు YS_HASHING_సంస్కరణ 1

namespace hashlib {

/**
 * HASHING
 *
 * Also refer to docs/source/yosys_internals/hashing.rst
 *
 * The Hasher knows how to hash 32 and 64-bit integers. That's it.
 * In the future, it could be expanded to do vectors with SIMD.
 *
 * The Hasher doesn't know how to hash common standard containers
 * and compositions. However, hashlib provides centralized wrappers.
 *
 * Hashlib doesn't know how to hash silly Yosys-specific types.
 * Hashlib doesn't depend on Yosys and can be used standalone.
 * Please don't use hashlib standalone for new projects.
 * Never directly include kernel/hashlib.h in Yosys code.
 * Instead include kernel/yosys_common.h
 *
 * The hash_ops type is now always left to its default value, derived
 * from templated functions through SFINAE. Providing custom ops is
 * still supported.
 *
 * HASH TABLES
 *
 * We implement associative data structures with separate chaining.
 * Linked lists use integers into the indirection hashtable array
 * instead of pointers.
 */

#ఒకవేళ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_వాడని __attribute__((warn_వాడని))
#లేకపోతే
#  define HASHLIB_ATTRIBUTE_WARN_వాడని
#అయితే_ముగింపు

స్థిరరాశి పూర్ణాంకం hashtable_కొలత_trigger = 2;
స్థిరరాశి పూర్ణాంకం hashtable_కొలత_గుణకం = 3;

namespace legacy {
	అంతఃవరుస uint32_t djb2_కూడు(uint32_t a, uint32_t b) {
		తిరిగి ((a << 5) + a) + b;
	}
};

మూస<typename T>
నిర్మాణం hash_ops;

అంతఃవరుస గుర్తులేని పూర్ణాంకం mkhash_xorshift(గుర్తులేని పూర్ణాంకం a) {
	ఒకవేళ (పరిమాణం(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} లేకపోతే ఒకవేళ (పరిమాణం(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} లేకపోతే
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	తిరిగి a;
}

class HasherDJB32 {
బహిరంగ:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

అంతరంగ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	స్థిర uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		తిరిగి hash;
	}
బహిరంగ:
	శూన్య hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		తిరిగి;
	}
	శూన్య hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		తిరిగి;
	}
	[[nodiscard]]
	hash_t yield() స్థిరరాశి {
		తిరిగి (hash_t)state;
	}

	మూస<typename T>
	శూన్య eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	మూస<typename T>
	శూన్య eat(స్థిరరాశి T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	శూన్య commutative_eat(hash_t t) {
		state ^= t;
	}

	శూన్య force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#నిర్వచించు HASH_శిఖరం_LOOP_FST [[nodiscard]] స్థిర అంతఃవరుస Hasher hash
#నిర్వచించు HASH_శిఖరం_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	తిరిగి h; \
}

మూస<typename T>
నిర్మాణం hash_ops {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి T &a, స్థిరరాశి T &b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి T &a, Hasher h) {
		ఒకవేళ constexpr (std::is_integral_v<T>) {
			static_assert(పరిమాణం(T) <= పరిమాణం(uint64_t));
			ఒకవేళ (పరిమాణం(T) == పరిమాణం(uint64_t))
				h.hash64(a);
			లేకపోతే
				h.hash32(a);
			తిరిగి h;
		} లేకపోతే ఒకవేళ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			తిరిగి hash_ops<u_type>::hash_into((u_type) a, h);
		} లేకపోతే ఒకవేళ constexpr (std::is_సూచిక_v<T>) {
			తిరిగి hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} లేకపోతే ఒకవేళ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			పూర్ణాంకం కొలత = a.కొలత();
			పూర్ణాంకం i = 0;
			అయితే (i + 8 < కొలత) {
				uint64_t v;
				జ్ఞాపకం_నకలు(&v, a.దత్తాంశం() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			జ్ఞాపకం_నకలు(&v, a.దత్తాంశం() + i, కొలత - i);
			h.hash64(v);
			తిరిగి h;
		} లేకపోతే {
			తిరిగి a.hash_into(h);
		}
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి T &a) HASH_శిఖరం_LOOP_SND
};

మూస<typename P, typename Q> నిర్మాణం hash_ops<std::pair<P, Q>> {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::pair<P, Q> &a, స్థిరరాశి std::pair<P, Q> &b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.మొదటి, h);
		h = hash_ops<Q>::hash_into(a.రెండవ, h);
		తిరిగి h;
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి std::pair<P, Q> &a) HASH_శిఖరం_LOOP_SND
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash(స్థిరరాశి P &p, స్థిరరాశి Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		తిరిగి h;
	}
};

మూస<typename... T> నిర్మాణం hash_ops<std::tuple<T...>> {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::tuple<T...> &a, స్థిరరాశి std::tuple<T...> &b) {
		తిరిగి a == b;
	}
	మూస<కొలత_t I = 0>
	స్థిర అంతఃవరుస typename std::enable_if<I == పరిమాణం...(T), Hasher>::type hash_into(స్థిరరాశి std::tuple<T...> &, Hasher h) {
		తిరిగి h;
	}
	మూస<కొలత_t I = 0>
	స్థిర అంతఃవరుస typename std::enable_if<I != పరిమాణం...(T), Hasher>::type hash_into(స్థిరరాశి std::tuple<T...> &a, Hasher h) {
		రకనిర్వచనం hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		తిరిగి h;
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి std::tuple<T...> &a) HASH_శిఖరం_LOOP_SND
};

మూస<typename T> నిర్మాణం hash_ops<std::సదిశ<T>> {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::సదిశ<T> &a, స్థిరరాశి std::సదిశ<T> &b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి std::సదిశ<T> &a, Hasher h) {
		h.eat((uint32_t)a.కొలత());
		కొరకు (స్వయంచాలక k : a)
			h.eat(k);
		తిరిగి h;
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి std::సదిశ<T> &a) HASH_శిఖరం_LOOP_SND
};

మూస<typename T, కొలత_t N> నిర్మాణం hash_ops<std::array<T, N>> {
    స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::array<T, N> &a, స్థిరరాశి std::array<T, N> &b) {
        తిరిగి a == b;
    }
    [[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి std::array<T, N> &a, Hasher h) {
        కొరకు (స్థిరరాశి స్వయంచాలక& k : a)
            h = hash_ops<T>::hash_into(k, h);
        తిరిగి h;
    }
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి std::array<T, N> &a) HASH_శిఖరం_LOOP_SND
};

నిర్మాణం hash_cstr_ops {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
		తిరిగి strcmp(a, b) == 0;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి అక్షరం *a, Hasher h) {
		అయితే (*a)
			h.hash32(*(a++));
		తిరిగి h;
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి అక్షరం *a) HASH_శిఖరం_LOOP_SND
};

మూస <> నిర్మాణం hash_ops<అక్షరం*> : hash_cstr_ops {};

నిర్మాణం hash_ptr_ops {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి శూన్య *a, Hasher h) {
		తిరిగి hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి శూన్య *a) HASH_శిఖరం_LOOP_SND
};

నిర్మాణం hash_obj_ops {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b) {
		తిరిగి a == b;
	}
	మూస<typename T>
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి T *a, Hasher h) {
		ఒకవేళ (a)
			h = a->hash_into(h);
		లేకపోతే
			h.eat(0);
		తిరిగి h;
	}
	మూస<typename T>
	HASH_శిఖరం_LOOP_FST (స్థిరరాశి T *a) HASH_శిఖరం_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
మూస<typename T>
[[nodiscard]]
Hasher::hash_t నిరంతరం_hash(స్థిరరాశి T& obj) {
	తిరిగి hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
మూస<typename T>
[[nodiscard]]
[[deprecated]]
అంతఃవరుస గుర్తులేని పూర్ణాంకం mkhash(స్థిరరాశి T &v) {
	తిరిగి (గుర్తులేని పూర్ణాంకం) నిరంతరం_hash<T>(v);
}

మూస<> నిర్మాణం hash_ops<std::monostate> {
	స్థిర అంతఃవరుస bool cmp(std::monostate a, std::monostate b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(std::monostate, Hasher h) {
		తిరిగి h;
	}
};

మూస<typename... T> నిర్మాణం hash_ops<std::variant<T...>> {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::variant<T...> &a, స్థిరరాశి std::variant<T...> &b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి std::variant<T...> &a, Hasher h) {
		std::సందర్శించు([& h](స్థిరరాశి స్వయంచాలక &v) { h.eat(v); }, a);
		h.eat(a.సూచీ());
		తిరిగి h;
	}
};

మూస<typename T> నిర్మాణం hash_ops<std::optional<T>> {
	స్థిర అంతఃవరుస bool cmp(స్థిరరాశి std::optional<T> &a, స్థిరరాశి std::optional<T> &b) {
		తిరిగి a == b;
	}
	[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి std::optional<T> &a, Hasher h) {
		ఒకవేళ(a.has_విలువ())
			h.eat(*a);
		లేకపోతే
			h.eat(0);
		తిరిగి h;
	}
};

అంతఃవరుస గుర్తులేని పూర్ణాంకం hashtable_కొలత(గుర్తులేని పూర్ణాంకం min_కొలత)
{
	// Primes as generated by https://oeis.org/A175953
	స్థిర std::సదిశ<గుర్తులేని పూర్ణాంకం> zero_and_some_ప్రధానసంఖ్యలు = {
		0, 23, 29, 37, 47, 59, 79, 101, 127, 163, 211, 269, 337, 431, 541, 677,
		853, 1069, 1361, 1709, 2137, 2677, 3347, 4201, 5261, 6577, 8231, 10289,
		12889, 16127, 20161, 25219, 31531, 39419, 49277, 61603, 77017, 96281,
		120371, 150473, 188107, 235159, 293957, 367453, 459317, 574157, 717697,
		897133, 1121423, 1401791, 1752239, 2190299, 2737937, 3422429, 4278037,
		5347553, 6684443, 8355563, 10444457, 13055587, 16319519, 20399411,
		25499291, 31874149, 39842687, 49803361, 62254207, 77817767, 97272239,
		121590311, 151987889, 189984863, 237481091, 296851369, 371064217,
		463830313, 579787991, 724735009, 905918777, 1132398479, 1415498113,
		1769372713, 2211715897, 2764644887, 3455806139
	};

	కొరకు (స్వయంచాలక p : zero_and_some_ప్రధానసంఖ్యలు)
		ఒకవేళ (p >= min_కొలత) తిరిగి p;

	ఒకవేళ (పరిమాణం(గుర్తులేని పూర్ణాంకం) == 4)
		throw std::నిడివి_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	కొరకు (స్వయంచాలక p : zero_and_some_ప్రధానసంఖ్యలు)
		ఒకవేళ (100129 * p > min_కొలత) తిరిగి 100129 * p;

	throw std::నిడివి_error("hash table exceeded maximum size.");
}

మూస<typename K, typename T, typename OPS = hash_ops<K>> class dict;
మూస<typename K, పూర్ణాంకం స్థానభ్రంశం = 0, typename OPS = hash_ops<K>> class idict;
మూస<typename K, typename OPS = hash_ops<K>> class pool;
మూస<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
బహిరంగ:
	commutative_hash() {
		buckets.fill(0);
	}
	మూస <typename T>
	శూన్య eat(స్థిరరాశి T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	శూన్య eat(స్థిరరాశి Hasher &h) {
		Hasher::hash_t v = h.yield();
		కొలత_t సూచీ = v & (buckets.కొలత() - 1);
		buckets[సూచీ] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		కొరకు (స్వయంచాలక b : buckets)
			h.eat(b);
		తిరిగి h;
	}
అంతరంగ:
	std::array<Hasher::hash_t, 4> buckets;
};

మూస<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_వాడని dict {
	నిర్మాణం నమోదు_t
	{
		std::pair<K, T> udata;
		పూర్ణాంకం తదుపరి;

		నమోదు_t() { }
		నమోదు_t(స్థిరరాశి std::pair<K, T> &udata, పూర్ణాంకం తదుపరి) : udata(udata), తదుపరి(తదుపరి) { }
		నమోదు_t(std::pair<K, T> &&udata, పూర్ణాంకం తదుపరి) : udata(std::move(udata)), తదుపరి(తదుపరి) { }
		bool క్రియాకారకం<(స్థిరరాశి నమోదు_t &other) స్థిరరాశి { తిరిగి udata.మొదటి < other.udata.మొదటి; }
	};

	std::సదిశ<పూర్ణాంకం> hashtable;
	std::సదిశ<నమోదు_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#నిర్వచితమైతే NDEBUG
	స్థిర అంతఃవరుస శూన్య do_assert(bool) { }
#లేకపోతే
	స్థిర అంతఃవరుస శూన్య do_assert(bool cond) {
		ఒకవేళ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#అయితే_ముగింపు

	Hasher::hash_t do_hash(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = 0;
		ఒకవేళ (!hashtable.ఖాళీలు())
			hash = ops.hash(కీలకం).yield() % (గుర్తులేని పూర్ణాంకం)(hashtable.కొలత());
		తిరిగి hash;
	}

	శూన్య do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_కొలత(entries.సామర్థ్యం() * hashtable_కొలత_గుణకం), -1);

		కొరకు (పూర్ణాంకం i = 0; i < పూర్ణాంకం(entries.కొలత()); i++) {
			do_assert(-1 <= entries[i].తదుపరి && entries[i].తదుపరి < పూర్ణాంకం(entries.కొలత()));
			Hasher::hash_t hash = do_hash(entries[i].udata.మొదటి);
			entries[i].తదుపరి = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	పూర్ణాంకం do_erase(పూర్ణాంకం సూచీ, Hasher::hash_t hash)
	{
		do_assert(సూచీ < పూర్ణాంకం(entries.కొలత()));
		ఒకవేళ (hashtable.ఖాళీలు() || సూచీ < 0)
			తిరిగి 0;

		పూర్ణాంకం k = hashtable[hash];
		do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));

		ఒకవేళ (k == సూచీ) {
			hashtable[hash] = entries[సూచీ].తదుపరి;
		} లేకపోతే {
			అయితే (entries[k].తదుపరి != సూచీ) {
				k = entries[k].తదుపరి;
				do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));
			}
			entries[k].తదుపరి = entries[సూచీ].తదుపరి;
		}

		పూర్ణాంకం వెనుక_idx = entries.కొలత()-1;

		ఒకవేళ (సూచీ != వెనుక_idx)
		{
			Hasher::hash_t వెనుక_hash = do_hash(entries[వెనుక_idx].udata.మొదటి);

			k = hashtable[వెనుక_hash];
			do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));

			ఒకవేళ (k == వెనుక_idx) {
				hashtable[వెనుక_hash] = సూచీ;
			} లేకపోతే {
				అయితే (entries[k].తదుపరి != వెనుక_idx) {
					k = entries[k].తదుపరి;
					do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));
				}
				entries[k].తదుపరి = సూచీ;
			}

			entries[సూచీ] = std::move(entries[వెనుక_idx]);
		}

		entries.తీయి_వెనుక();

		ఒకవేళ (entries.ఖాళీలు())
			hashtable.clear();

		తిరిగి 1;
	}

	పూర్ణాంకం do_lookup(స్థిరరాశి K &కీలకం, Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు())
			తిరిగి -1;

		ఒకవేళ (entries.కొలత() * hashtable_కొలత_trigger > hashtable.కొలత()) {
			do_rehash();
			hash = do_hash(కీలకం);
		}

		తిరిగి do_lookup_అంతర్గత(కీలకం, hash);
	}

	పూర్ణాంకం do_lookup_అంతర్గత(స్థిరరాశి K &కీలకం, Hasher::hash_t hash) స్థిరరాశి
	{
		పూర్ణాంకం సూచీ = hashtable[hash];

		అయితే (సూచీ >= 0 && !ops.cmp(entries[సూచీ].udata.మొదటి, కీలకం)) {
			సూచీ = entries[సూచీ].తదుపరి;
			do_assert(-1 <= సూచీ && సూచీ < పూర్ణాంకం(entries.కొలత()));
		}

		తిరిగి సూచీ;
	}

	పూర్ణాంకం do_lookup_no_rehash(స్థిరరాశి K &కీలకం, Hasher::hash_t hash) స్థిరరాశి
	{
		ఒకవేళ (hashtable.ఖాళీలు())
			తిరిగి -1;

		తిరిగి do_lookup_అంతర్గత(కీలకం, hash);
	}

	పూర్ణాంకం do_చొప్పించు(స్థిరరాశి K &కీలకం, స్థిరరాశి Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు()) {
			entries.emplace_వెనుక(std::pair<K, T>(కీలకం, T()), -1);
			do_rehash();
		} లేకపోతే {
			entries.emplace_వెనుక(std::pair<K, T>(కీలకం, T()), hashtable[hash]);
			hashtable[hash] = entries.కొలత() - 1;
		}
		తిరిగి entries.కొలత() - 1;
	}

	పూర్ణాంకం do_చొప్పించు(స్థిరరాశి std::pair<K, T> &విలువ, స్థిరరాశి Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు()) {
			entries.emplace_వెనుక(విలువ, -1);
			do_rehash();
		} లేకపోతే {
			entries.emplace_వెనుక(విలువ, hashtable[hash]);
			hashtable[hash] = entries.కొలత() - 1;
		}
		తిరిగి entries.కొలత() - 1;
	}

	పూర్ణాంకం do_చొప్పించు(std::pair<K, T> &&rvalue, స్థిరరాశి Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు()) {
			entries.emplace_వెనుక(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} లేకపోతే {
			entries.emplace_వెనుక(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.కొలత() - 1;
		}
		తిరిగి entries.కొలత() - 1;
	}

బహిరంగ:
	class const_iterator
	{
		మిత్ర class dict;
	రక్షిత:
		స్థిరరాశి dict *ptr;
		పూర్ణాంకం సూచీ;
		const_iterator(స్థిరరాశి dict *ptr, పూర్ణాంకం సూచీ) : ptr(ptr), సూచీ(సూచీ) { }
	బహిరంగ:
		రకనిర్వచనం std::bidirectional_iterator_ట్యాగ్ iterator_category;
		రకనిర్వచనం std::pair<K, T> విలువ_type;
		రకనిర్వచనం ptrdiff_t difference_type;
		రకనిర్వచనం స్థిరరాశి std::pair<K, T>* సూచిక;
		రకనిర్వచనం స్థిరరాశి std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator క్రియాకారకం++() { సూచీ--; తిరిగి *this; }
		const_iterator క్రియాకారకం++(పూర్ణాంకం) { const_iterator tmp = *this; సూచీ--; తిరిగి tmp; }
		const_iterator క్రియాకారకం--() { సూచీ++; తిరిగి *this; }
		const_iterator క్రియాకారకం--(పూర్ణాంకం) { const_iterator tmp = *this; సూచీ++; తిరిగి tmp; }
		const_iterator క్రియాకారకం+=(పూర్ణాంకం amt) { సూచీ -= amt; తిరిగి *this; }
		bool క్రియాకారకం<(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ > other.సూచీ; }
		bool క్రియాకారకం==(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
		bool క్రియాకారకం!=(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
		స్థిరరాశి std::pair<K, T> &క్రియాకారకం*() స్థిరరాశి { తిరిగి ptr->entries[సూచీ].udata; }
		స్థిరరాశి std::pair<K, T> *క్రియాకారకం->() స్థిరరాశి { తిరిగి &ptr->entries[సూచీ].udata; }
	};

	class iterator
	{
		మిత్ర class dict;
	రక్షిత:
		dict *ptr;
		పూర్ణాంకం సూచీ;
		iterator(dict *ptr, పూర్ణాంకం సూచీ) : ptr(ptr), సూచీ(సూచీ) { }
	బహిరంగ:
		రకనిర్వచనం std::forward_iterator_ట్యాగ్ iterator_category;
		రకనిర్వచనం std::pair<K, T> విలువ_type;
		రకనిర్వచనం ptrdiff_t difference_type;
		రకనిర్వచనం std::pair<K, T>* సూచిక;
		రకనిర్వచనం std::pair<K, T>& reference;
		iterator() { }
		iterator క్రియాకారకం++() { సూచీ--; తిరిగి *this; }
		iterator క్రియాకారకం+=(పూర్ణాంకం amt) { సూచీ -= amt; తిరిగి *this; }
		bool క్రియాకారకం<(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి సూచీ > other.సూచీ; }
		bool క్రియాకారకం==(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
		bool క్రియాకారకం!=(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
		std::pair<K, T> &క్రియాకారకం*() { తిరిగి ptr->entries[సూచీ].udata; }
		std::pair<K, T> *క్రియాకారకం->() { తిరిగి &ptr->entries[సూచీ].udata; }
		స్థిరరాశి std::pair<K, T> &క్రియాకారకం*() స్థిరరాశి { తిరిగి ptr->entries[సూచీ].udata; }
		స్థిరరాశి std::pair<K, T> *క్రియాకారకం->() స్థిరరాశి { తిరిగి &ptr->entries[సూచీ].udata; }
		క్రియాకారకం const_iterator() స్థిరరాశి { తిరిగి const_iterator(ptr, సూచీ); }
	};
	using తిరగేయి_iterator = std::తిరగేయి_iterator<const_iterator>;
	తిరగేయి_iterator rbegin() స్థిరరాశి {
		తిరిగి std::make_తిరగేయి_iterator(చివర());
	}
	తిరగేయి_iterator rend() స్థిరరాశి {
		తిరిగి std::make_తిరగేయి_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(స్థిరరాశి dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		మార్పిడి(other);
	}

	dict &క్రియాకారకం=(స్థిరరాశి dict &other) {
		entries = other.entries;
		do_rehash();
		తిరిగి *this;
	}

	dict &క్రియాకారకం=(dict &&other) noexcept {
		clear();
		మార్పిడి(other);
		తిరిగి *this;
	}

	dict(స్థిరరాశి std::initializer_list<std::pair<K, T>> &list)
	{
		కొరకు (స్వయంచాలక &it : list)
			చొప్పించు(it);
	}

	మూస<class ఆదానం_Iterator>
	dict(ఆదానం_Iterator మొదటి, ఆదానం_Iterator చివరి)
	{
		చొప్పించు(మొదటి, చివరి);
	}

	మూస<class ఆదానం_Iterator>
	శూన్య చొప్పించు(ఆదానం_Iterator మొదటి, ఆదానం_Iterator చివరి)
	{
		కొరకు (; మొదటి != చివరి; ++మొదటి)
			చొప్పించు(*మొదటి);
	}

	std::pair<iterator, bool> చొప్పించు(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(కీలకం, hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> చొప్పించు(స్థిరరాశి std::pair<K, T> &విలువ)
	{
		Hasher::hash_t hash = do_hash(విలువ.మొదటి);
		పూర్ణాంకం i = do_lookup(విలువ.మొదటి, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(విలువ, hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> చొప్పించు(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.మొదటి);
		పూర్ణాంకం i = do_lookup(rvalue.మొదటి, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::forward<std::pair<K, T>>(rvalue), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K స్థిరరాశి &కీలకం, T స్థిరరాశి &విలువ)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::make_pair(కీలకం, విలువ), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K స్థిరరాశి &కీలకం, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::make_pair(కీలకం, std::forward<T>(rvalue)), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T స్థిరరాశి &విలువ)
	{
		Hasher::hash_t hash = do_hash(rkey);
		పూర్ణాంకం i = do_lookup(rkey, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::make_pair(std::forward<K>(rkey), విలువ), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		పూర్ణాంకం i = do_lookup(rkey, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	పూర్ణాంకం erase(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం సూచీ = do_lookup(కీలకం, hash);
		తిరిగి do_erase(సూచీ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->మొదటి);
		do_erase(it.సూచీ, hash);
		తిరిగి ++it;
	}

	పూర్ణాంకం లెక్క(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		తిరిగి i < 0 ? 0 : 1;
	}

	పూర్ణాంకం లెక్క(స్థిరరాశి K &కీలకం, const_iterator it) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		తిరిగి i < 0 || i > it.సూచీ ? 0 : 1;
	}

	iterator కనుగొను(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి చివర();
		తిరిగి iterator(this, i);
	}

	const_iterator కనుగొను(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి చివర();
		తిరిగి const_iterator(this, i);
	}

	T& at(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i < 0)
			throw std::బహిర్గతం_యొక్క_range("dict::at()");
		తిరిగి entries[i].udata.రెండవ;
	}

	స్థిరరాశి T& at(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			throw std::బహిర్గతం_యొక్క_range("dict::at()");
		తిరిగి entries[i].udata.రెండవ;
	}

	స్థిరరాశి T& at(స్థిరరాశి K &కీలకం, స్థిరరాశి T &defval) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి defval;
		తిరిగి entries[i].udata.రెండవ;
	}

	T& క్రియాకారకం[](స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i < 0)
			i = do_చొప్పించు(std::pair<K, T>(కీలకం, T()), hash);
		తిరిగి entries[i].udata.రెండవ;
	}

	మూస<typename Compare = std::less<K>>
	శూన్య క్రమీకరించు(Compare comp = Compare())
	{
		std::క్రమీకరించు(entries.begin(), entries.చివర(), [comp](స్థిరరాశి నమోదు_t &a, స్థిరరాశి నమోదు_t &b){ తిరిగి comp(b.udata.మొదటి, a.udata.మొదటి); });
		do_rehash();
	}

	శూన్య మార్పిడి(dict &other)
	{
		hashtable.మార్పిడి(other.hashtable);
		entries.మార్పిడి(other.entries);
	}

	bool క్రియాకారకం==(స్థిరరాశి dict &other) స్థిరరాశి {
		ఒకవేళ (కొలత() != other.కొలత())
			తిరిగి false;
		కొరకు (స్వయంచాలక &it : entries) {
			స్వయంచాలక oit = other.కనుగొను(it.udata.మొదటి);
			ఒకవేళ (oit == other.చివర() || !(oit->రెండవ == it.udata.రెండవ))
				తిరిగి false;
		}
		తిరిగి true;
	}

	bool క్రియాకారకం!=(స్థిరరాశి dict &other) స్థిరరాశి {
		తిరిగి !క్రియాకారకం==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		commutative_hash comm;
		కొరకు (స్వయంచాలక &it : entries) {
			Hasher నమోదు_hash;
			నమోదు_hash.eat(it.udata.మొదటి);
			నమోదు_hash.eat(it.udata.రెండవ);
			comm.eat(నమోదు_hash);
		}
		తిరిగి comm.hash_into(h);
	}

	శూన్య reserve(కొలత_t n) { entries.reserve(n); }
	కొలత_t కొలత() స్థిరరాశి { తిరిగి entries.కొలత(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి entries.ఖాళీలు(); }
	శూన్య clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { తిరిగి iterator(this, పూర్ణాంకం(entries.కొలత())-1); }
	iterator element(పూర్ణాంకం n) { తిరిగి iterator(this, పూర్ణాంకం(entries.కొలత())-1-n); }
	iterator చివర() { తిరిగి iterator(nullptr, -1); }

	const_iterator begin() స్థిరరాశి { తిరిగి const_iterator(this, పూర్ణాంకం(entries.కొలత())-1); }
	const_iterator element(పూర్ణాంకం n) స్థిరరాశి { తిరిగి const_iterator(this, పూర్ణాంకం(entries.కొలత())-1-n); }
	const_iterator చివర() స్థిరరాశి { తిరిగి const_iterator(this, -1); }
};

మూస<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_వాడని pool
{
	మూస<typename, పూర్ణాంకం, typename> మిత్ర class idict;

రక్షిత:
	నిర్మాణం నమోదు_t
	{
		K udata;
		పూర్ణాంకం తదుపరి;

		నమోదు_t() { }
		నమోదు_t(స్థిరరాశి K &udata, పూర్ణాంకం తదుపరి) : udata(udata), తదుపరి(తదుపరి) { }
		నమోదు_t(K &&udata, పూర్ణాంకం తదుపరి) : udata(std::move(udata)), తదుపరి(తదుపరి) { }
	};

	std::సదిశ<పూర్ణాంకం> hashtable;
	std::సదిశ<నమోదు_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#నిర్వచితమైతే NDEBUG
	స్థిర అంతఃవరుస శూన్య do_assert(bool) { }
#లేకపోతే
	స్థిర అంతఃవరుస శూన్య do_assert(bool cond) {
		ఒకవేళ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#అయితే_ముగింపు

	Hasher::hash_t do_hash(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = 0;
		ఒకవేళ (!hashtable.ఖాళీలు())
			hash = ops.hash(కీలకం).yield() % (గుర్తులేని పూర్ణాంకం)(hashtable.కొలత());
		తిరిగి hash;
	}

	శూన్య do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_కొలత(entries.సామర్థ్యం() * hashtable_కొలత_గుణకం), -1);

		కొరకు (పూర్ణాంకం i = 0; i < పూర్ణాంకం(entries.కొలత()); i++) {
			do_assert(-1 <= entries[i].తదుపరి && entries[i].తదుపరి < పూర్ణాంకం(entries.కొలత()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].తదుపరి = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	పూర్ణాంకం do_erase(పూర్ణాంకం సూచీ, Hasher::hash_t hash)
	{
		do_assert(సూచీ < పూర్ణాంకం(entries.కొలత()));
		ఒకవేళ (hashtable.ఖాళీలు() || సూచీ < 0)
			తిరిగి 0;

		పూర్ణాంకం k = hashtable[hash];
		ఒకవేళ (k == సూచీ) {
			hashtable[hash] = entries[సూచీ].తదుపరి;
		} లేకపోతే {
			అయితే (entries[k].తదుపరి != సూచీ) {
				k = entries[k].తదుపరి;
				do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));
			}
			entries[k].తదుపరి = entries[సూచీ].తదుపరి;
		}

		పూర్ణాంకం వెనుక_idx = entries.కొలత()-1;

		ఒకవేళ (సూచీ != వెనుక_idx)
		{
			Hasher::hash_t వెనుక_hash = do_hash(entries[వెనుక_idx].udata);

			k = hashtable[వెనుక_hash];
			ఒకవేళ (k == వెనుక_idx) {
				hashtable[వెనుక_hash] = సూచీ;
			} లేకపోతే {
				అయితే (entries[k].తదుపరి != వెనుక_idx) {
					k = entries[k].తదుపరి;
					do_assert(0 <= k && k < పూర్ణాంకం(entries.కొలత()));
				}
				entries[k].తదుపరి = సూచీ;
			}

			entries[సూచీ] = std::move(entries[వెనుక_idx]);
		}

		entries.తీయి_వెనుక();

		ఒకవేళ (entries.ఖాళీలు())
			hashtable.clear();

		తిరిగి 1;
	}

	పూర్ణాంకం do_lookup(స్థిరరాశి K &కీలకం, Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు())
			తిరిగి -1;

		ఒకవేళ (entries.కొలత() * hashtable_కొలత_trigger > hashtable.కొలత()) {
			do_rehash();
			hash = do_hash(కీలకం);
		}

		తిరిగి do_lookup_అంతర్గత(కీలకం, hash);
	}

	పూర్ణాంకం do_lookup_అంతర్గత(స్థిరరాశి K &కీలకం, Hasher::hash_t hash) స్థిరరాశి
	{
		పూర్ణాంకం సూచీ = hashtable[hash];

		అయితే (సూచీ >= 0 && !ops.cmp(entries[సూచీ].udata, కీలకం)) {
			సూచీ = entries[సూచీ].తదుపరి;
			do_assert(-1 <= సూచీ && సూచీ < పూర్ణాంకం(entries.కొలత()));
		}

		తిరిగి సూచీ;
	}

	పూర్ణాంకం do_lookup_no_rehash(స్థిరరాశి K &కీలకం, Hasher::hash_t hash) స్థిరరాశి
	{
		ఒకవేళ (hashtable.ఖాళీలు())
			తిరిగి -1;

		తిరిగి do_lookup_అంతర్గత(కీలకం, hash);
	}

	పూర్ణాంకం do_చొప్పించు(స్థిరరాశి K &విలువ, Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు()) {
			entries.emplace_వెనుక(విలువ, -1);
			do_rehash();
			hash = do_hash(విలువ);
		} లేకపోతే {
			entries.emplace_వెనుక(విలువ, hashtable[hash]);
			hashtable[hash] = entries.కొలత() - 1;
		}
		తిరిగి entries.కొలత() - 1;
	}

	పూర్ణాంకం do_చొప్పించు(K &&rvalue, Hasher::hash_t &hash)
	{
		ఒకవేళ (hashtable.ఖాళీలు()) {
			entries.emplace_వెనుక(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.వెనుక().udata);
		} లేకపోతే {
			entries.emplace_వెనుక(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.కొలత() - 1;
		}
		తిరిగి entries.కొలత() - 1;
	}

బహిరంగ:
	class const_iterator
	{
		మిత్ర class pool;
	రక్షిత:
		స్థిరరాశి pool *ptr;
		పూర్ణాంకం సూచీ;
		const_iterator(స్థిరరాశి pool *ptr, పూర్ణాంకం సూచీ) : ptr(ptr), సూచీ(సూచీ) { }
	బహిరంగ:
		రకనిర్వచనం std::forward_iterator_ట్యాగ్ iterator_category;
		రకనిర్వచనం K విలువ_type;
		రకనిర్వచనం ptrdiff_t difference_type;
		రకనిర్వచనం K* సూచిక;
		రకనిర్వచనం K& reference;
		const_iterator() { }
		const_iterator క్రియాకారకం++() { సూచీ--; తిరిగి *this; }
		bool క్రియాకారకం==(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
		bool క్రియాకారకం!=(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
		స్థిరరాశి K &క్రియాకారకం*() స్థిరరాశి { తిరిగి ptr->entries[సూచీ].udata; }
		స్థిరరాశి K *క్రియాకారకం->() స్థిరరాశి { తిరిగి &ptr->entries[సూచీ].udata; }
	};

	class iterator
	{
		మిత్ర class pool;
	రక్షిత:
		pool *ptr;
		పూర్ణాంకం సూచీ;
		iterator(pool *ptr, పూర్ణాంకం సూచీ) : ptr(ptr), సూచీ(సూచీ) { }
	బహిరంగ:
		రకనిర్వచనం std::forward_iterator_ట్యాగ్ iterator_category;
		రకనిర్వచనం K విలువ_type;
		రకనిర్వచనం ptrdiff_t difference_type;
		రకనిర్వచనం K* సూచిక;
		రకనిర్వచనం K& reference;
		iterator() { }
		iterator క్రియాకారకం++() { సూచీ--; తిరిగి *this; }
		bool క్రియాకారకం==(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
		bool క్రియాకారకం!=(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
		K &క్రియాకారకం*() { తిరిగి ptr->entries[సూచీ].udata; }
		K *క్రియాకారకం->() { తిరిగి &ptr->entries[సూచీ].udata; }
		స్థిరరాశి K &క్రియాకారకం*() స్థిరరాశి { తిరిగి ptr->entries[సూచీ].udata; }
		స్థిరరాశి K *క్రియాకారకం->() స్థిరరాశి { తిరిగి &ptr->entries[సూచీ].udata; }
		క్రియాకారకం const_iterator() స్థిరరాశి { తిరిగి const_iterator(ptr, సూచీ); }
	};

	constexpr pool()
	{
	}

	pool(స్థిరరాశి pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		మార్పిడి(other);
	}

	pool &క్రియాకారకం=(స్థిరరాశి pool &other) {
		entries = other.entries;
		do_rehash();
		తిరిగి *this;
	}

	pool &క్రియాకారకం=(pool &&other) noexcept {
		clear();
		మార్పిడి(other);
		తిరిగి *this;
	}

	pool(స్థిరరాశి std::initializer_list<K> &list)
	{
		కొరకు (స్వయంచాలక &it : list)
			చొప్పించు(it);
	}

	మూస<class ఆదానం_Iterator>
	pool(ఆదానం_Iterator మొదటి, ఆదానం_Iterator చివరి)
	{
		చొప్పించు(మొదటి, చివరి);
	}

	మూస<class ఆదానం_Iterator>
	శూన్య చొప్పించు(ఆదానం_Iterator మొదటి, ఆదానం_Iterator చివరి)
	{
		కొరకు (; మొదటి != చివరి; ++మొదటి)
			చొప్పించు(*మొదటి);
	}

	std::pair<iterator, bool> చొప్పించు(స్థిరరాశి K &విలువ)
	{
		Hasher::hash_t hash = do_hash(విలువ);
		పూర్ణాంకం i = do_lookup(విలువ, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(విలువ, hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> చొప్పించు(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		పూర్ణాంకం i = do_lookup(rvalue, hash);
		ఒకవేళ (i >= 0)
			తిరిగి std::pair<iterator, bool>(iterator(this, i), false);
		i = do_చొప్పించు(std::forward<K>(rvalue), hash);
		తిరిగి std::pair<iterator, bool>(iterator(this, i), true);
	}

	మూస<typename... వాదనలు_3>
	std::pair<iterator, bool> emplace(వాదనలు_3&&... వాదనలు_2)
	{
		తిరిగి చొప్పించు(K(std::forward<వాదనలు_3>(వాదనలు_2)...));
	}

	పూర్ణాంకం erase(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం సూచీ = do_lookup(కీలకం, hash);
		తిరిగి do_erase(సూచీ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.సూచీ, hash);
		తిరిగి ++it;
	}

	పూర్ణాంకం లెక్క(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		తిరిగి i < 0 ? 0 : 1;
	}

	పూర్ణాంకం లెక్క(స్థిరరాశి K &కీలకం, const_iterator it) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		తిరిగి i < 0 || i > it.సూచీ ? 0 : 1;
	}

	iterator కనుగొను(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి చివర();
		తిరిగి iterator(this, i);
	}

	const_iterator కనుగొను(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి చివర();
		తిరిగి const_iterator(this, i);
	}

	bool క్రియాకారకం[](స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = do_hash(కీలకం);
		పూర్ణాంకం i = do_lookup(కీలకం, hash);
		తిరిగి i >= 0;
	}

	మూస<typename Compare = std::less<K>>
	శూన్య క్రమీకరించు(Compare comp = Compare())
	{
		std::క్రమీకరించు(entries.begin(), entries.చివర(), [comp](స్థిరరాశి నమోదు_t &a, స్థిరరాశి నమోదు_t &b){ తిరిగి comp(b.udata, a.udata); });
		do_rehash();
	}

	K తీయి()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		తిరిగి ret;
	}

	శూన్య మార్పిడి(pool &other)
	{
		hashtable.మార్పిడి(other.hashtable);
		entries.మార్పిడి(other.entries);
	}

	bool క్రియాకారకం==(స్థిరరాశి pool &other) స్థిరరాశి {
		ఒకవేళ (కొలత() != other.కొలత())
			తిరిగి false;
		కొరకు (స్వయంచాలక &it : entries)
			ఒకవేళ (!other.లెక్క(it.udata))
				తిరిగి false;
		తిరిగి true;
	}

	bool క్రియాకారకం!=(స్థిరరాశి pool &other) స్థిరరాశి {
		తిరిగి !క్రియాకారకం==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		commutative_hash comm;
		కొరకు (స్వయంచాలక &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		తిరిగి comm.hash_into(h);
	}

	శూన్య reserve(కొలత_t n) { entries.reserve(n); }
	కొలత_t కొలత() స్థిరరాశి { తిరిగి entries.కొలత(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి entries.ఖాళీలు(); }
	శూన్య clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { తిరిగి iterator(this, పూర్ణాంకం(entries.కొలత())-1); }
	iterator element(పూర్ణాంకం n) { తిరిగి iterator(this, పూర్ణాంకం(entries.కొలత())-1-n); }
	iterator చివర() { తిరిగి iterator(nullptr, -1); }

	const_iterator begin() స్థిరరాశి { తిరిగి const_iterator(this, పూర్ణాంకం(entries.కొలత())-1); }
	const_iterator element(పూర్ణాంకం n) స్థిరరాశి { తిరిగి const_iterator(this, పూర్ణాంకం(entries.కొలత())-1-n); }
	const_iterator చివర() స్థిరరాశి { తిరిగి const_iterator(nullptr, -1); }
};

మూస<typename K, పూర్ణాంకం స్థానభ్రంశం, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_వాడని idict
{
	pool<K, OPS> database;

బహిరంగ:
	class const_iterator
	{
		మిత్ర class idict;
	రక్షిత:
		స్థిరరాశి idict &container;
		పూర్ణాంకం సూచీ;
		const_iterator(స్థిరరాశి idict &container, పూర్ణాంకం సూచీ) : container(container), సూచీ(సూచీ) { }
	బహిరంగ:
		రకనిర్వచనం std::forward_iterator_ట్యాగ్ iterator_category;
		రకనిర్వచనం K విలువ_type;
		రకనిర్వచనం ptrdiff_t difference_type;
		రకనిర్వచనం K* సూచిక;
		రకనిర్వచనం K& reference;
		const_iterator() { }
		const_iterator క్రియాకారకం++() { సూచీ++; తిరిగి *this; }
		bool క్రియాకారకం==(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
		bool క్రియాకారకం!=(స్థిరరాశి const_iterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
		స్థిరరాశి K &క్రియాకారకం*() స్థిరరాశి { తిరిగి container[సూచీ]; }
		స్థిరరాశి K *క్రియాకారకం->() స్థిరరాశి { తిరిగి &container[సూచీ]; }
	};

	constexpr idict()
	{
	}

	పూర్ణాంకం క్రియాకారకం()(స్థిరరాశి K &కీలకం)
	{
		Hasher::hash_t hash = database.do_hash(కీలకం);
		పూర్ణాంకం i = database.do_lookup(కీలకం, hash);
		ఒకవేళ (i < 0)
			i = database.do_చొప్పించు(కీలకం, hash);
		తిరిగి i + స్థానభ్రంశం;
	}

	పూర్ణాంకం at(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = database.do_hash(కీలకం);
		పూర్ణాంకం i = database.do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			throw std::బహిర్గతం_యొక్క_range("idict::at()");
		తిరిగి i + స్థానభ్రంశం;
	}

	పూర్ణాంకం at(స్థిరరాశి K &కీలకం, పూర్ణాంకం defval) స్థిరరాశి
	{
		Hasher::hash_t hash = database.do_hash(కీలకం);
		పూర్ణాంకం i = database.do_lookup_no_rehash(కీలకం, hash);
		ఒకవేళ (i < 0)
			తిరిగి defval;
		తిరిగి i + స్థానభ్రంశం;
	}

	పూర్ణాంకం లెక్క(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = database.do_hash(కీలకం);
		పూర్ణాంకం i = database.do_lookup_no_rehash(కీలకం, hash);
		తిరిగి i < 0 ? 0 : 1;
	}

	పూర్ణాంకం lookup(స్థిరరాశి K &కీలకం) స్థిరరాశి
	{
		Hasher::hash_t hash = database.do_hash(కీలకం);
		తిరిగి database.do_lookup_no_rehash(కీలకం, hash);
	}

	శూన్య expect(స్థిరరాశి K &కీలకం, పూర్ణాంకం i)
	{
		పూర్ణాంకం j = (*this)(కీలకం);
		ఒకవేళ (i != j)
			throw std::బహిర్గతం_యొక్క_range("idict::expect()");
	}

	స్థిరరాశి K &క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి
	{
		తిరిగి database.entries.at(సూచీ - స్థానభ్రంశం).udata;
	}

	శూన్య మార్పిడి(idict &other)
	{
		database.మార్పిడి(other.database);
	}

	శూన్య reserve(కొలత_t n) { database.reserve(n); }
	కొలత_t కొలత() స్థిరరాశి { తిరిగి database.కొలత(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి database.ఖాళీలు(); }
	శూన్య clear() { database.clear(); }

	const_iterator begin() స్థిరరాశి { తిరిగి const_iterator(*this, స్థానభ్రంశం); }
	const_iterator element(పూర్ణాంకం n) స్థిరరాశి { తిరిగి const_iterator(*this, n); }
	const_iterator చివర() స్థిరరాశి { తిరిగి const_iterator(*this, స్థానభ్రంశం + కొలత()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
మూస<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_వాడని mfp
{
	idict<K, 0, OPS> database;
	class Atomic_మాతృ {
	బహిరంగ:
		explicit Atomic_మాతృ(పూర్ణాంకం p) : మాతృ(p) {}
		Atomic_మాతృ(స్థిరరాశి Atomic_మాతృ &other) : మాతృ(other.get()) {}
		Atomic_మాతృ &క్రియాకారకం=(స్థిరరాశి Atomic_మాతృ &other) { సమితి(other.get()); తిరిగి *this; }
		పూర్ణాంకం get() స్థిరరాశి { తిరిగి మాతృ.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed); }
		శూన్య సమితి(పూర్ణాంకం p) { మాతృ.store(p, std::జ్ఞాపకం_క్రమం_relaxed); }
	అంతరంగ:
		std::atomic<పూర్ణాంకం> మాతృ;
	};
	std::సదిశ<Atomic_మాతృ> parents;

బహిరంగ:
	రకనిర్వచనం typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	పూర్ణాంకం క్రియాకారకం()(స్థిరరాశి K &కీలకం)
	{
		పూర్ణాంకం i = database(కీలకం);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		ఒకవేళ (parents.కొలత() < database.కొలత()) {
			parents.emplace_వెనుక(-1);
		}
		తిరిగి i;
	}

	// Finds an element at given index
	స్థిరరాశి K &క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి
	{
		తిరిగి database[సూచీ];
	}

	// Why this method is correct for concurent ifind() calls:
	// Consider the mfp state after the last non-const method call before
	// a particular call to ifind(i). In this state, i's parent chain leads
	// to some root R. Let S be the set of integers s such that ifind(s) = R
	// in this state. Let 'orig_parents' be the value of 'parents' in this state.
	//
	// Now consider the concurrent calls to ifind(s), s ∈ S, before the next non-const method
	// call. Consider the atomic writes performed by various ifind() calls, in any causally
	// consistent order. The first atomic write can only set parents[k] to R, because the
	// atomic read of parents[p] in the first while loop can only observe the value
	// 'orig_parents[p]'. Subsequent writes can also only set parents[k] to R, because the
	// parents[p] reads either observe 'orig_parents[p]' or R (and observing R ends the first
	// while loop immediately). Thus all parents[p] reads observe either 'orig_parents[p]'
	// or R, so ifind() always returns R.
	పూర్ణాంకం ifind(పూర్ణాంకం i) స్థిరరాశి
	{
		పూర్ణాంకం p = i, k = i;

		అయితే (true) {
			పూర్ణాంకం pp = parents[p].get();
			ఒకవేళ (pp < 0)
				ఆపు;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		అయితే (k != p) {
			పూర్ణాంకం తదుపరి_k = parents[k].get();
			const_cast<Atomic_మాతృ*>(&parents[k])->సమితి(p);
			k = తదుపరి_k;
		}

		తిరిగి p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	శూన్య imerge(పూర్ణాంకం i, పూర్ణాంకం j)
	{
		i = ifind(i);
		j = ifind(j);

		ఒకవేళ (i != j)
			parents[i].సమితి(j);
	}

	శూన్య ipromote(పూర్ణాంకం i)
	{
		పూర్ణాంకం k = i;

		అయితే (k != -1) {
			పూర్ణాంకం తదుపరి_k = parents[k].get();
			parents[k].సమితి(i);
			k = తదుపరి_k;
		}

		parents[i].సమితి(-1);
	}

	పూర్ణాంకం lookup(స్థిరరాశి K &a)
	{
		తిరిగి ifind((*this)(a));
	}

	స్థిరరాశి K &కనుగొను(స్థిరరాశి K &a) స్థిరరాశి
	{
		పూర్ణాంకం i = database.at(a, -1);
		ఒకవేళ (i < 0)
			తిరిగి a;
		తిరిగి (*this)[ifind(i)];
	}

	శూన్య విలీనం(స్థిరరాశి K &a, స్థిరరాశి K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	శూన్య promote(స్థిరరాశి K &a)
	{
		పూర్ణాంకం i = database.at(a, -1);
		ఒకవేళ (i >= 0)
			ipromote(i);
	}

	శూన్య మార్పిడి(mfp &other)
	{
		database.మార్పిడి(other.database);
		parents.మార్పిడి(other.parents);
	}

	శూన్య reserve(కొలత_t n) { database.reserve(n); }
	కొలత_t కొలత() స్థిరరాశి { తిరిగి database.కొలత(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి database.ఖాళీలు(); }
	శూన్య clear() { database.clear(); parents.clear(); }

	const_iterator begin() స్థిరరాశి { తిరిగి database.begin(); }
	const_iterator element(పూర్ణాంకం n) స్థిరరాశి { తిరిగి database.element(n); }
	const_iterator చివర() స్థిరరాశి { తిరిగి database.చివర(); }
};

} /* namespace hashlib */

#అయితే_ముగింపు
