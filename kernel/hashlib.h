// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#ifndef HASHLIB_H
#define HASHLIB_H

#অন্তর্ভাব <array>
#অন্তর্ভাব <atomic>
#অন্তর্ভাব <stdexcept>
#অন্তর্ভাব <algorithm>
#অন্তর্ভাব <optional>
#অন্তর্ভাব <string>
#অন্তর্ভাব <variant>
#অন্তর্ভাব <vector>
#অন্তর্ভাব <type_traits>
#অন্তর্ভাব <stdint.h>

#define YS_HASHING_VERSION 1

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

#যদি defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#অন্যথা
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ধ্রুবক পূর্ণসংখ্যা hashtable_আকার_trigger = 2;
ধ্রুবক পূর্ণসংখ্যা hashtable_আকার_factor = 3;

namespace legacy {
	অন্তঃরেখ uint32_t djb2_add(uint32_t a, uint32_t b) {
		ফেরত ((a << 5) + a) + b;
	}
};

template<typename T>
কাঠামো hash_ops;

অন্তঃরেখ unsigned পূর্ণসংখ্যা mkhash_xorshift(unsigned পূর্ণসংখ্যা a) {
	যদি (আকার(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} অন্যথা যদি (আকার(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} অন্যথা
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ফেরত a;
}

class HasherDJB32 {
সর্বজনীন:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ব্যক্তিগত:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	স্থির uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ফেরত hash;
	}
সর্বজনীন:
	শূন্য hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ফেরত;
	}
	শূন্য hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ফেরত;
	}
	[[nodiscard]]
	hash_t yield() ধ্রুবক {
		ফেরত (hash_t)state;
	}

	template<typename T>
	শূন্য eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	শূন্য eat(ধ্রুবক T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	শূন্য commutative_eat(hash_t t) {
		state ^= t;
	}

	শূন্য force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_শীর্ষ_LOOP_FST [[nodiscard]] স্থির অন্তঃরেখ Hasher hash
#define HASH_শীর্ষ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ফেরত h; \
}

template<typename T>
কাঠামো hash_ops {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক T &a, ধ্রুবক T &b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক T &a, Hasher h) {
		যদি constexpr (std::is_integral_v<T>) {
			static_assert(আকার(T) <= আকার(uint64_t));
			যদি (আকার(T) == আকার(uint64_t))
				h.hash64(a);
			অন্যথা
				h.hash32(a);
			ফেরত h;
		} অন্যথা যদি constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ফেরত hash_ops<u_type>::hash_into((u_type) a, h);
		} অন্যথা যদি constexpr (std::is_pointer_v<T>) {
			ফেরত hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} অন্যথা যদি constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			পূর্ণসংখ্যা আকার_2 = a.আকার_2();
			পূর্ণসংখ্যা i = 0;
			যখন (i + 8 < আকার_2) {
				uint64_t v;
				memcpy(&v, a.তথ্য() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.তথ্য() + i, আকার_2 - i);
			h.hash64(v);
			ফেরত h;
		} অন্যথা {
			ফেরত a.hash_into(h);
		}
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক T &a) HASH_শীর্ষ_LOOP_SND
};

template<typename P, typename Q> কাঠামো hash_ops<std::pair<P, Q>> {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::pair<P, Q> &a, ধ্রুবক std::pair<P, Q> &b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		ফেরত h;
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক std::pair<P, Q> &a) HASH_শীর্ষ_LOOP_SND
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash(ধ্রুবক P &p, ধ্রুবক Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ফেরত h;
	}
};

template<typename... T> কাঠামো hash_ops<std::tuple<T...>> {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::tuple<T...> &a, ধ্রুবক std::tuple<T...> &b) {
		ফেরত a == b;
	}
	template<আকার_t I = 0>
	স্থির অন্তঃরেখ typename std::enable_if<I == আকার...(T), Hasher>::type hash_into(ধ্রুবক std::tuple<T...> &, Hasher h) {
		ফেরত h;
	}
	template<আকার_t I = 0>
	স্থির অন্তঃরেখ typename std::enable_if<I != আকার...(T), Hasher>::type hash_into(ধ্রুবক std::tuple<T...> &a, Hasher h) {
		প্রকারলেখ hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ফেরত h;
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক std::tuple<T...> &a) HASH_শীর্ষ_LOOP_SND
};

template<typename T> কাঠামো hash_ops<std::vector<T>> {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::vector<T> &a, ধ্রুবক std::vector<T> &b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.আকার_2());
		জন্য (auto k : a)
			h.eat(k);
		ফেরত h;
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক std::vector<T> &a) HASH_শীর্ষ_LOOP_SND
};

template<typename T, আকার_t N> কাঠামো hash_ops<std::array<T, N>> {
    স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::array<T, N> &a, ধ্রুবক std::array<T, N> &b) {
        ফেরত a == b;
    }
    [[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক std::array<T, N> &a, Hasher h) {
        জন্য (ধ্রুবক auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ফেরত h;
    }
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক std::array<T, N> &a) HASH_শীর্ষ_LOOP_SND
};

কাঠামো hash_cstr_ops {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
		ফেরত strcmp(a, b) == 0;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক অক্ষর *a, Hasher h) {
		যখন (*a)
			h.hash32(*(a++));
		ফেরত h;
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক অক্ষর *a) HASH_শীর্ষ_LOOP_SND
};

template <> কাঠামো hash_ops<অক্ষর*> : hash_cstr_ops {};

কাঠামো hash_ptr_ops {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক শূন্য *a, Hasher h) {
		ফেরত hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক শূন্য *a) HASH_শীর্ষ_LOOP_SND
};

কাঠামো hash_obj_ops {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b) {
		ফেরত a == b;
	}
	template<typename T>
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক T *a, Hasher h) {
		যদি (a)
			h = a->hash_into(h);
		অন্যথা
			h.eat(0);
		ফেরত h;
	}
	template<typename T>
	HASH_শীর্ষ_LOOP_FST (ধ্রুবক T *a) HASH_শীর্ষ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ধ্রুবক T& obj) {
	ফেরত hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
অন্তঃরেখ unsigned পূর্ণসংখ্যা mkhash(ধ্রুবক T &v) {
	ফেরত (unsigned পূর্ণসংখ্যা) run_hash<T>(v);
}

template<> কাঠামো hash_ops<std::monostate> {
	স্থির অন্তঃরেখ bool cmp(std::monostate a, std::monostate b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(std::monostate, Hasher h) {
		ফেরত h;
	}
};

template<typename... T> কাঠামো hash_ops<std::variant<T...>> {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::variant<T...> &a, ধ্রুবক std::variant<T...> &b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক std::variant<T...> &a, Hasher h) {
		std::visit([& h](ধ্রুবক auto &v) { h.eat(v); }, a);
		h.eat(a.সূচক());
		ফেরত h;
	}
};

template<typename T> কাঠামো hash_ops<std::optional<T>> {
	স্থির অন্তঃরেখ bool cmp(ধ্রুবক std::optional<T> &a, ধ্রুবক std::optional<T> &b) {
		ফেরত a == b;
	}
	[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক std::optional<T> &a, Hasher h) {
		যদি(a.has_মান())
			h.eat(*a);
		অন্যথা
			h.eat(0);
		ফেরত h;
	}
};

অন্তঃরেখ unsigned পূর্ণসংখ্যা hashtable_আকার(unsigned পূর্ণসংখ্যা min_আকার)
{
	// Primes as generated by https://oeis.org/A175953
	স্থির std::vector<unsigned পূর্ণসংখ্যা> zero_and_some_primes = {
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

	জন্য (auto p : zero_and_some_primes)
		যদি (p >= min_আকার) ফেরত p;

	যদি (আকার(unsigned পূর্ণসংখ্যা) == 4)
		throw std::দৈর্ঘ্য_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	জন্য (auto p : zero_and_some_primes)
		যদি (100129 * p > min_আকার) ফেরত 100129 * p;

	throw std::দৈর্ঘ্য_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, পূর্ণসংখ্যা offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
সর্বজনীন:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	শূন্য eat(ধ্রুবক T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	শূন্য eat(ধ্রুবক Hasher &h) {
		Hasher::hash_t v = h.yield();
		আকার_t সূচক = v & (buckets.আকার_2() - 1);
		buckets[সূচক] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
		জন্য (auto b : buckets)
			h.eat(b);
		ফেরত h;
	}
ব্যক্তিগত:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	কাঠামো entry_t
	{
		std::pair<K, T> udata;
		পূর্ণসংখ্যা পরবর্তী;

		entry_t() { }
		entry_t(ধ্রুবক std::pair<K, T> &udata, পূর্ণসংখ্যা পরবর্তী) : udata(udata), পরবর্তী(পরবর্তী) { }
		entry_t(std::pair<K, T> &&udata, পূর্ণসংখ্যা পরবর্তী) : udata(std::move(udata)), পরবর্তী(পরবর্তী) { }
		bool operator<(ধ্রুবক entry_t &other) ধ্রুবক { ফেরত udata.first < other.udata.first; }
	};

	std::vector<পূর্ণসংখ্যা> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	স্থির অন্তঃরেখ শূন্য do_assert(bool) { }
#অন্যথা
	স্থির অন্তঃরেখ শূন্য do_assert(bool cond) {
		যদি (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = 0;
		যদি (!hashtable.empty())
			hash = ops.hash(চাবি).yield() % (unsigned পূর্ণসংখ্যা)(hashtable.আকার_2());
		ফেরত hash;
	}

	শূন্য do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_আকার(entries.capacity() * hashtable_আকার_factor), -1);

		জন্য (পূর্ণসংখ্যা i = 0; i < পূর্ণসংখ্যা(entries.আকার_2()); i++) {
			do_assert(-1 <= entries[i].পরবর্তী && entries[i].পরবর্তী < পূর্ণসংখ্যা(entries.আকার_2()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].পরবর্তী = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	পূর্ণসংখ্যা do_erase(পূর্ণসংখ্যা সূচক, Hasher::hash_t hash)
	{
		do_assert(সূচক < পূর্ণসংখ্যা(entries.আকার_2()));
		যদি (hashtable.empty() || সূচক < 0)
			ফেরত 0;

		পূর্ণসংখ্যা k = hashtable[hash];
		do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));

		যদি (k == সূচক) {
			hashtable[hash] = entries[সূচক].পরবর্তী;
		} অন্যথা {
			যখন (entries[k].পরবর্তী != সূচক) {
				k = entries[k].পরবর্তী;
				do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));
			}
			entries[k].পরবর্তী = entries[সূচক].পরবর্তী;
		}

		পূর্ণসংখ্যা back_idx = entries.আকার_2()-1;

		যদি (সূচক != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));

			যদি (k == back_idx) {
				hashtable[back_hash] = সূচক;
			} অন্যথা {
				যখন (entries[k].পরবর্তী != back_idx) {
					k = entries[k].পরবর্তী;
					do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));
				}
				entries[k].পরবর্তী = সূচক;
			}

			entries[সূচক] = std::move(entries[back_idx]);
		}

		entries.বের_করো_back();

		যদি (entries.empty())
			hashtable.clear();

		ফেরত 1;
	}

	পূর্ণসংখ্যা do_lookup(ধ্রুবক K &চাবি, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty())
			ফেরত -1;

		যদি (entries.আকার_2() * hashtable_আকার_trigger > hashtable.আকার_2()) {
			do_rehash();
			hash = do_hash(চাবি);
		}

		ফেরত do_lookup_internal(চাবি, hash);
	}

	পূর্ণসংখ্যা do_lookup_internal(ধ্রুবক K &চাবি, Hasher::hash_t hash) ধ্রুবক
	{
		পূর্ণসংখ্যা সূচক = hashtable[hash];

		যখন (সূচক >= 0 && !ops.cmp(entries[সূচক].udata.first, চাবি)) {
			সূচক = entries[সূচক].পরবর্তী;
			do_assert(-1 <= সূচক && সূচক < পূর্ণসংখ্যা(entries.আকার_2()));
		}

		ফেরত সূচক;
	}

	পূর্ণসংখ্যা do_lookup_no_rehash(ধ্রুবক K &চাবি, Hasher::hash_t hash) ধ্রুবক
	{
		যদি (hashtable.empty())
			ফেরত -1;

		ফেরত do_lookup_internal(চাবি, hash);
	}

	পূর্ণসংখ্যা do_ঢোকাও(ধ্রুবক K &চাবি, ধ্রুবক Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(চাবি, T()), -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(std::pair<K, T>(চাবি, T()), hashtable[hash]);
			hashtable[hash] = entries.আকার_2() - 1;
		}
		ফেরত entries.আকার_2() - 1;
	}

	পূর্ণসংখ্যা do_ঢোকাও(ধ্রুবক std::pair<K, T> &মান, ধ্রুবক Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(মান, -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(মান, hashtable[hash]);
			hashtable[hash] = entries.আকার_2() - 1;
		}
		ফেরত entries.আকার_2() - 1;
	}

	পূর্ণসংখ্যা do_ঢোকাও(std::pair<K, T> &&rvalue, ধ্রুবক Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.আকার_2() - 1;
		}
		ফেরত entries.আকার_2() - 1;
	}

সর্বজনীন:
	class const_iterator
	{
		friend class dict;
	সুরক্ষিত:
		ধ্রুবক dict *ptr;
		পূর্ণসংখ্যা সূচক;
		const_iterator(ধ্রুবক dict *ptr, পূর্ণসংখ্যা সূচক) : ptr(ptr), সূচক(সূচক) { }
	সর্বজনীন:
		প্রকারলেখ std::bidirectional_iterator_tag iterator_category;
		প্রকারলেখ std::pair<K, T> মান_type;
		প্রকারলেখ ptrdiff_t difference_type;
		প্রকারলেখ ধ্রুবক std::pair<K, T>* pointer;
		প্রকারলেখ ধ্রুবক std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { সূচক--; ফেরত *this; }
		const_iterator operator++(পূর্ণসংখ্যা) { const_iterator tmp = *this; সূচক--; ফেরত tmp; }
		const_iterator operator--() { সূচক++; ফেরত *this; }
		const_iterator operator--(পূর্ণসংখ্যা) { const_iterator tmp = *this; সূচক++; ফেরত tmp; }
		const_iterator operator+=(পূর্ণসংখ্যা amt) { সূচক -= amt; ফেরত *this; }
		bool operator<(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক > other.সূচক; }
		bool operator==(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
		bool operator!=(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
		ধ্রুবক std::pair<K, T> &operator*() ধ্রুবক { ফেরত ptr->entries[সূচক].udata; }
		ধ্রুবক std::pair<K, T> *operator->() ধ্রুবক { ফেরত &ptr->entries[সূচক].udata; }
	};

	class iterator
	{
		friend class dict;
	সুরক্ষিত:
		dict *ptr;
		পূর্ণসংখ্যা সূচক;
		iterator(dict *ptr, পূর্ণসংখ্যা সূচক) : ptr(ptr), সূচক(সূচক) { }
	সর্বজনীন:
		প্রকারলেখ std::forward_iterator_tag iterator_category;
		প্রকারলেখ std::pair<K, T> মান_type;
		প্রকারলেখ ptrdiff_t difference_type;
		প্রকারলেখ std::pair<K, T>* pointer;
		প্রকারলেখ std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { সূচক--; ফেরত *this; }
		iterator operator+=(পূর্ণসংখ্যা amt) { সূচক -= amt; ফেরত *this; }
		bool operator<(ধ্রুবক iterator &other) ধ্রুবক { ফেরত সূচক > other.সূচক; }
		bool operator==(ধ্রুবক iterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
		bool operator!=(ধ্রুবক iterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
		std::pair<K, T> &operator*() { ফেরত ptr->entries[সূচক].udata; }
		std::pair<K, T> *operator->() { ফেরত &ptr->entries[সূচক].udata; }
		ধ্রুবক std::pair<K, T> &operator*() ধ্রুবক { ফেরত ptr->entries[সূচক].udata; }
		ধ্রুবক std::pair<K, T> *operator->() ধ্রুবক { ফেরত &ptr->entries[সূচক].udata; }
		operator const_iterator() ধ্রুবক { ফেরত const_iterator(ptr, সূচক); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ধ্রুবক {
		ফেরত std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ধ্রুবক {
		ফেরত std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ধ্রুবক dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ধ্রুবক dict &other) {
		entries = other.entries;
		do_rehash();
		ফেরত *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		ফেরত *this;
	}

	dict(ধ্রুবক std::initializer_list<std::pair<K, T>> &list)
	{
		জন্য (auto &it : list)
			ঢোকাও(it);
	}

	template<class প্রবেশ_Iterator>
	dict(প্রবেশ_Iterator first, প্রবেশ_Iterator last)
	{
		ঢোকাও(first, last);
	}

	template<class প্রবেশ_Iterator>
	শূন্য ঢোকাও(প্রবেশ_Iterator first, প্রবেশ_Iterator last)
	{
		জন্য (; first != last; ++first)
			ঢোকাও(*first);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(চাবি, hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুবক std::pair<K, T> &মান)
	{
		Hasher::hash_t hash = do_hash(মান.first);
		পূর্ণসংখ্যা i = do_lookup(মান.first, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(মান, hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		পূর্ণসংখ্যা i = do_lookup(rvalue.first, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::forward<std::pair<K, T>>(rvalue), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ধ্রুবক &চাবি, T ধ্রুবক &মান)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(চাবি, মান), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ধ্রুবক &চাবি, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(চাবি, std::forward<T>(rvalue)), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ধ্রুবক &মান)
	{
		Hasher::hash_t hash = do_hash(rkey);
		পূর্ণসংখ্যা i = do_lookup(rkey, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(std::forward<K>(rkey), মান), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		পূর্ণসংখ্যা i = do_lookup(rkey, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	পূর্ণসংখ্যা erase(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা সূচক = do_lookup(চাবি, hash);
		ফেরত do_erase(সূচক, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.সূচক, hash);
		ফেরত ++it;
	}

	পূর্ণসংখ্যা গুনতি(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		ফেরত i < 0 ? 0 : 1;
	}

	পূর্ণসংখ্যা গুনতি(ধ্রুবক K &চাবি, const_iterator it) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		ফেরত i < 0 || i > it.সূচক ? 0 : 1;
	}

	iterator find(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i < 0)
			ফেরত end();
		ফেরত iterator(this, i);
	}

	const_iterator find(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ফেরত end();
		ফেরত const_iterator(this, i);
	}

	T& at(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("dict::at()");
		ফেরত entries[i].udata.second;
	}

	ধ্রুবক T& at(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("dict::at()");
		ফেরত entries[i].udata.second;
	}

	ধ্রুবক T& at(ধ্রুবক K &চাবি, ধ্রুবক T &defval) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ফেরত defval;
		ফেরত entries[i].udata.second;
	}

	T& operator[](ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i < 0)
			i = do_ঢোকাও(std::pair<K, T>(চাবি, T()), hash);
		ফেরত entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	শূন্য sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ধ্রুবক entry_t &a, ধ্রুবক entry_t &b){ ফেরত comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	শূন্য swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ধ্রুবক dict &other) ধ্রুবক {
		যদি (আকার_2() != other.আকার_2())
			ফেরত false;
		জন্য (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			যদি (oit == other.end() || !(oit->second == it.udata.second))
				ফেরত false;
		}
		ফেরত true;
	}

	bool operator!=(ধ্রুবক dict &other) ধ্রুবক {
		ফেরত !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
		commutative_hash comm;
		জন্য (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		ফেরত comm.hash_into(h);
	}

	শূন্য reserve(আকার_t n) { entries.reserve(n); }
	আকার_t আকার_2() ধ্রুবক { ফেরত entries.আকার_2(); }
	bool empty() ধ্রুবক { ফেরত entries.empty(); }
	শূন্য clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ফেরত iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1); }
	iterator element(পূর্ণসংখ্যা n) { ফেরত iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1-n); }
	iterator end() { ফেরত iterator(nullptr, -1); }

	const_iterator begin() ধ্রুবক { ফেরত const_iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1); }
	const_iterator element(পূর্ণসংখ্যা n) ধ্রুবক { ফেরত const_iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1-n); }
	const_iterator end() ধ্রুবক { ফেরত const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, পূর্ণসংখ্যা, typename> friend class idict;

সুরক্ষিত:
	কাঠামো entry_t
	{
		K udata;
		পূর্ণসংখ্যা পরবর্তী;

		entry_t() { }
		entry_t(ধ্রুবক K &udata, পূর্ণসংখ্যা পরবর্তী) : udata(udata), পরবর্তী(পরবর্তী) { }
		entry_t(K &&udata, পূর্ণসংখ্যা পরবর্তী) : udata(std::move(udata)), পরবর্তী(পরবর্তী) { }
	};

	std::vector<পূর্ণসংখ্যা> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	স্থির অন্তঃরেখ শূন্য do_assert(bool) { }
#অন্যথা
	স্থির অন্তঃরেখ শূন্য do_assert(bool cond) {
		যদি (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = 0;
		যদি (!hashtable.empty())
			hash = ops.hash(চাবি).yield() % (unsigned পূর্ণসংখ্যা)(hashtable.আকার_2());
		ফেরত hash;
	}

	শূন্য do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_আকার(entries.capacity() * hashtable_আকার_factor), -1);

		জন্য (পূর্ণসংখ্যা i = 0; i < পূর্ণসংখ্যা(entries.আকার_2()); i++) {
			do_assert(-1 <= entries[i].পরবর্তী && entries[i].পরবর্তী < পূর্ণসংখ্যা(entries.আকার_2()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].পরবর্তী = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	পূর্ণসংখ্যা do_erase(পূর্ণসংখ্যা সূচক, Hasher::hash_t hash)
	{
		do_assert(সূচক < পূর্ণসংখ্যা(entries.আকার_2()));
		যদি (hashtable.empty() || সূচক < 0)
			ফেরত 0;

		পূর্ণসংখ্যা k = hashtable[hash];
		যদি (k == সূচক) {
			hashtable[hash] = entries[সূচক].পরবর্তী;
		} অন্যথা {
			যখন (entries[k].পরবর্তী != সূচক) {
				k = entries[k].পরবর্তী;
				do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));
			}
			entries[k].পরবর্তী = entries[সূচক].পরবর্তী;
		}

		পূর্ণসংখ্যা back_idx = entries.আকার_2()-1;

		যদি (সূচক != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			যদি (k == back_idx) {
				hashtable[back_hash] = সূচক;
			} অন্যথা {
				যখন (entries[k].পরবর্তী != back_idx) {
					k = entries[k].পরবর্তী;
					do_assert(0 <= k && k < পূর্ণসংখ্যা(entries.আকার_2()));
				}
				entries[k].পরবর্তী = সূচক;
			}

			entries[সূচক] = std::move(entries[back_idx]);
		}

		entries.বের_করো_back();

		যদি (entries.empty())
			hashtable.clear();

		ফেরত 1;
	}

	পূর্ণসংখ্যা do_lookup(ধ্রুবক K &চাবি, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty())
			ফেরত -1;

		যদি (entries.আকার_2() * hashtable_আকার_trigger > hashtable.আকার_2()) {
			do_rehash();
			hash = do_hash(চাবি);
		}

		ফেরত do_lookup_internal(চাবি, hash);
	}

	পূর্ণসংখ্যা do_lookup_internal(ধ্রুবক K &চাবি, Hasher::hash_t hash) ধ্রুবক
	{
		পূর্ণসংখ্যা সূচক = hashtable[hash];

		যখন (সূচক >= 0 && !ops.cmp(entries[সূচক].udata, চাবি)) {
			সূচক = entries[সূচক].পরবর্তী;
			do_assert(-1 <= সূচক && সূচক < পূর্ণসংখ্যা(entries.আকার_2()));
		}

		ফেরত সূচক;
	}

	পূর্ণসংখ্যা do_lookup_no_rehash(ধ্রুবক K &চাবি, Hasher::hash_t hash) ধ্রুবক
	{
		যদি (hashtable.empty())
			ফেরত -1;

		ফেরত do_lookup_internal(চাবি, hash);
	}

	পূর্ণসংখ্যা do_ঢোকাও(ধ্রুবক K &মান, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(মান, -1);
			do_rehash();
			hash = do_hash(মান);
		} অন্যথা {
			entries.emplace_back(মান, hashtable[hash]);
			hashtable[hash] = entries.আকার_2() - 1;
		}
		ফেরত entries.আকার_2() - 1;
	}

	পূর্ণসংখ্যা do_ঢোকাও(K &&rvalue, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} অন্যথা {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.আকার_2() - 1;
		}
		ফেরত entries.আকার_2() - 1;
	}

সর্বজনীন:
	class const_iterator
	{
		friend class pool;
	সুরক্ষিত:
		ধ্রুবক pool *ptr;
		পূর্ণসংখ্যা সূচক;
		const_iterator(ধ্রুবক pool *ptr, পূর্ণসংখ্যা সূচক) : ptr(ptr), সূচক(সূচক) { }
	সর্বজনীন:
		প্রকারলেখ std::forward_iterator_tag iterator_category;
		প্রকারলেখ K মান_type;
		প্রকারলেখ ptrdiff_t difference_type;
		প্রকারলেখ K* pointer;
		প্রকারলেখ K& reference;
		const_iterator() { }
		const_iterator operator++() { সূচক--; ফেরত *this; }
		bool operator==(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
		bool operator!=(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
		ধ্রুবক K &operator*() ধ্রুবক { ফেরত ptr->entries[সূচক].udata; }
		ধ্রুবক K *operator->() ধ্রুবক { ফেরত &ptr->entries[সূচক].udata; }
	};

	class iterator
	{
		friend class pool;
	সুরক্ষিত:
		pool *ptr;
		পূর্ণসংখ্যা সূচক;
		iterator(pool *ptr, পূর্ণসংখ্যা সূচক) : ptr(ptr), সূচক(সূচক) { }
	সর্বজনীন:
		প্রকারলেখ std::forward_iterator_tag iterator_category;
		প্রকারলেখ K মান_type;
		প্রকারলেখ ptrdiff_t difference_type;
		প্রকারলেখ K* pointer;
		প্রকারলেখ K& reference;
		iterator() { }
		iterator operator++() { সূচক--; ফেরত *this; }
		bool operator==(ধ্রুবক iterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
		bool operator!=(ধ্রুবক iterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
		K &operator*() { ফেরত ptr->entries[সূচক].udata; }
		K *operator->() { ফেরত &ptr->entries[সূচক].udata; }
		ধ্রুবক K &operator*() ধ্রুবক { ফেরত ptr->entries[সূচক].udata; }
		ধ্রুবক K *operator->() ধ্রুবক { ফেরত &ptr->entries[সূচক].udata; }
		operator const_iterator() ধ্রুবক { ফেরত const_iterator(ptr, সূচক); }
	};

	constexpr pool()
	{
	}

	pool(ধ্রুবক pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ধ্রুবক pool &other) {
		entries = other.entries;
		do_rehash();
		ফেরত *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		ফেরত *this;
	}

	pool(ধ্রুবক std::initializer_list<K> &list)
	{
		জন্য (auto &it : list)
			ঢোকাও(it);
	}

	template<class প্রবেশ_Iterator>
	pool(প্রবেশ_Iterator first, প্রবেশ_Iterator last)
	{
		ঢোকাও(first, last);
	}

	template<class প্রবেশ_Iterator>
	শূন্য ঢোকাও(প্রবেশ_Iterator first, প্রবেশ_Iterator last)
	{
		জন্য (; first != last; ++first)
			ঢোকাও(*first);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুবক K &মান)
	{
		Hasher::hash_t hash = do_hash(মান);
		পূর্ণসংখ্যা i = do_lookup(মান, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(মান, hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		পূর্ণসংখ্যা i = do_lookup(rvalue, hash);
		যদি (i >= 0)
			ফেরত std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::forward<K>(rvalue), hash);
		ফেরত std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		ফেরত ঢোকাও(K(std::forward<Args>(args)...));
	}

	পূর্ণসংখ্যা erase(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা সূচক = do_lookup(চাবি, hash);
		ফেরত do_erase(সূচক, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.সূচক, hash);
		ফেরত ++it;
	}

	পূর্ণসংখ্যা গুনতি(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		ফেরত i < 0 ? 0 : 1;
	}

	পূর্ণসংখ্যা গুনতি(ধ্রুবক K &চাবি, const_iterator it) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		ফেরত i < 0 || i > it.সূচক ? 0 : 1;
	}

	iterator find(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		যদি (i < 0)
			ফেরত end();
		ফেরত iterator(this, i);
	}

	const_iterator find(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ফেরত end();
		ফেরত const_iterator(this, i);
	}

	bool operator[](ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণসংখ্যা i = do_lookup(চাবি, hash);
		ফেরত i >= 0;
	}

	template<typename Compare = std::less<K>>
	শূন্য sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ধ্রুবক entry_t &a, ধ্রুবক entry_t &b){ ফেরত comp(b.udata, a.udata); });
		do_rehash();
	}

	K বের_করো()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ফেরত ret;
	}

	শূন্য swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ধ্রুবক pool &other) ধ্রুবক {
		যদি (আকার_2() != other.আকার_2())
			ফেরত false;
		জন্য (auto &it : entries)
			যদি (!other.গুনতি(it.udata))
				ফেরত false;
		ফেরত true;
	}

	bool operator!=(ধ্রুবক pool &other) ধ্রুবক {
		ফেরত !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
		commutative_hash comm;
		জন্য (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ফেরত comm.hash_into(h);
	}

	শূন্য reserve(আকার_t n) { entries.reserve(n); }
	আকার_t আকার_2() ধ্রুবক { ফেরত entries.আকার_2(); }
	bool empty() ধ্রুবক { ফেরত entries.empty(); }
	শূন্য clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ফেরত iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1); }
	iterator element(পূর্ণসংখ্যা n) { ফেরত iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1-n); }
	iterator end() { ফেরত iterator(nullptr, -1); }

	const_iterator begin() ধ্রুবক { ফেরত const_iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1); }
	const_iterator element(পূর্ণসংখ্যা n) ধ্রুবক { ফেরত const_iterator(this, পূর্ণসংখ্যা(entries.আকার_2())-1-n); }
	const_iterator end() ধ্রুবক { ফেরত const_iterator(nullptr, -1); }
};

template<typename K, পূর্ণসংখ্যা offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

সর্বজনীন:
	class const_iterator
	{
		friend class idict;
	সুরক্ষিত:
		ধ্রুবক idict &container;
		পূর্ণসংখ্যা সূচক;
		const_iterator(ধ্রুবক idict &container, পূর্ণসংখ্যা সূচক) : container(container), সূচক(সূচক) { }
	সর্বজনীন:
		প্রকারলেখ std::forward_iterator_tag iterator_category;
		প্রকারলেখ K মান_type;
		প্রকারলেখ ptrdiff_t difference_type;
		প্রকারলেখ K* pointer;
		প্রকারলেখ K& reference;
		const_iterator() { }
		const_iterator operator++() { সূচক++; ফেরত *this; }
		bool operator==(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
		bool operator!=(ধ্রুবক const_iterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
		ধ্রুবক K &operator*() ধ্রুবক { ফেরত container[সূচক]; }
		ধ্রুবক K *operator->() ধ্রুবক { ফেরত &container[সূচক]; }
	};

	constexpr idict()
	{
	}

	পূর্ণসংখ্যা operator()(ধ্রুবক K &চাবি)
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণসংখ্যা i = database.do_lookup(চাবি, hash);
		যদি (i < 0)
			i = database.do_ঢোকাও(চাবি, hash);
		ফেরত i + offset;
	}

	পূর্ণসংখ্যা at(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণসংখ্যা i = database.do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("idict::at()");
		ফেরত i + offset;
	}

	পূর্ণসংখ্যা at(ধ্রুবক K &চাবি, পূর্ণসংখ্যা defval) ধ্রুবক
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণসংখ্যা i = database.do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ফেরত defval;
		ফেরত i + offset;
	}

	পূর্ণসংখ্যা গুনতি(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণসংখ্যা i = database.do_lookup_no_rehash(চাবি, hash);
		ফেরত i < 0 ? 0 : 1;
	}

	পূর্ণসংখ্যা lookup(ধ্রুবক K &চাবি) ধ্রুবক
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		ফেরত database.do_lookup_no_rehash(চাবি, hash);
	}

	শূন্য expect(ধ্রুবক K &চাবি, পূর্ণসংখ্যা i)
	{
		পূর্ণসংখ্যা j = (*this)(চাবি);
		যদি (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ধ্রুবক K &operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক
	{
		ফেরত database.entries.at(সূচক - offset).udata;
	}

	শূন্য swap(idict &other)
	{
		database.swap(other.database);
	}

	শূন্য reserve(আকার_t n) { database.reserve(n); }
	আকার_t আকার_2() ধ্রুবক { ফেরত database.আকার_2(); }
	bool empty() ধ্রুবক { ফেরত database.empty(); }
	শূন্য clear() { database.clear(); }

	const_iterator begin() ধ্রুবক { ফেরত const_iterator(*this, offset); }
	const_iterator element(পূর্ণসংখ্যা n) ধ্রুবক { ফেরত const_iterator(*this, n); }
	const_iterator end() ধ্রুবক { ফেরত const_iterator(*this, offset + আকার_2()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED mfp
{
	idict<K, 0, OPS> database;
	class AtomicParent {
	সর্বজনীন:
		explicit AtomicParent(পূর্ণসংখ্যা p) : parent(p) {}
		AtomicParent(ধ্রুবক AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ধ্রুবক AtomicParent &other) { set(other.get()); ফেরত *this; }
		পূর্ণসংখ্যা get() ধ্রুবক { ফেরত parent.load(std::memory_order_relaxed); }
		শূন্য set(পূর্ণসংখ্যা p) { parent.store(p, std::memory_order_relaxed); }
	ব্যক্তিগত:
		std::atomic<পূর্ণসংখ্যা> parent;
	};
	std::vector<AtomicParent> parents;

সর্বজনীন:
	প্রকারলেখ typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	পূর্ণসংখ্যা operator()(ধ্রুবক K &চাবি)
	{
		পূর্ণসংখ্যা i = database(চাবি);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		যদি (parents.আকার_2() < database.আকার_2()) {
			parents.emplace_back(-1);
		}
		ফেরত i;
	}

	// Finds an element at given index
	ধ্রুবক K &operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক
	{
		ফেরত database[সূচক];
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
	পূর্ণসংখ্যা ifind(পূর্ণসংখ্যা i) ধ্রুবক
	{
		পূর্ণসংখ্যা p = i, k = i;

		যখন (true) {
			পূর্ণসংখ্যা pp = parents[p].get();
			যদি (pp < 0)
				বিরতি;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		যখন (k != p) {
			পূর্ণসংখ্যা পরবর্তী_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = পরবর্তী_k;
		}

		ফেরত p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	শূন্য imerge(পূর্ণসংখ্যা i, পূর্ণসংখ্যা j)
	{
		i = ifind(i);
		j = ifind(j);

		যদি (i != j)
			parents[i].set(j);
	}

	শূন্য ipromote(পূর্ণসংখ্যা i)
	{
		পূর্ণসংখ্যা k = i;

		যখন (k != -1) {
			পূর্ণসংখ্যা পরবর্তী_k = parents[k].get();
			parents[k].set(i);
			k = পরবর্তী_k;
		}

		parents[i].set(-1);
	}

	পূর্ণসংখ্যা lookup(ধ্রুবক K &a)
	{
		ফেরত ifind((*this)(a));
	}

	ধ্রুবক K &find(ধ্রুবক K &a) ধ্রুবক
	{
		পূর্ণসংখ্যা i = database.at(a, -1);
		যদি (i < 0)
			ফেরত a;
		ফেরত (*this)[ifind(i)];
	}

	শূন্য merge(ধ্রুবক K &a, ধ্রুবক K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	শূন্য promote(ধ্রুবক K &a)
	{
		পূর্ণসংখ্যা i = database.at(a, -1);
		যদি (i >= 0)
			ipromote(i);
	}

	শূন্য swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	শূন্য reserve(আকার_t n) { database.reserve(n); }
	আকার_t আকার_2() ধ্রুবক { ফেরত database.আকার_2(); }
	bool empty() ধ্রুবক { ফেরত database.empty(); }
	শূন্য clear() { database.clear(); parents.clear(); }

	const_iterator begin() ধ্রুবক { ফেরত database.begin(); }
	const_iterator element(পূর্ণসংখ্যা n) ধ্রুবক { ফেরত database.element(n); }
	const_iterator end() ধ্রুবক { ফেরত database.end(); }
};

} /* namespace hashlib */

#endif
