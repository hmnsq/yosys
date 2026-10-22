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

#ಅನ್ತರ್ಭಾವ <array>
#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <stdexcept>
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <variant>
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ <stdint.h>

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

#ಯದಿ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#ಅನ್ಯಥಾ
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ hashtable_size_trigger = 2;
ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ hashtable_size_factor = 3;

namespace legacy {
	ಅನ್ತಃಸ್ಥ uint32_t djb2_add(uint32_t a, uint32_t b) {
		ಪ್ರತ್ಯಾಗಮ ((a << 5) + a) + b;
	}
};

template<typename T>
ಸಂರಚನಾ hash_ops;

ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ mkhash_xorshift(unsigned ಪೂರ್ಣಾಙ್ಕ a) {
	ಯದಿ (ಪರಿಮಾಣ(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} ಅನ್ಯಥಾ ಯದಿ (ಪರಿಮಾಣ(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} ಅನ್ಯಥಾ
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ಪ್ರತ್ಯಾಗಮ a;
}

class HasherDJB32 {
ಸಾರ್ವಜನಿಕ:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ಗುಪ್ತ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ಸ್ಥಿರ uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ಪ್ರತ್ಯಾಗಮ hash;
	}
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಶೂನ್ಯ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ಪ್ರತ್ಯಾಗಮ;
	}
	[[nodiscard]]
	hash_t yield() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ (hash_t)state;
	}

	template<typename T>
	ಶೂನ್ಯ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	ಶೂನ್ಯ eat(ಧ್ರುವ T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	ಶೂನ್ಯ commutative_eat(hash_t t) {
		state ^= t;
	}

	ಶೂನ್ಯ force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_ತುದಿ_LOOP_FST [[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash
#define HASH_ತುದಿ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ಪ್ರತ್ಯಾಗಮ h; \
}

template<typename T>
ಸಂರಚನಾ hash_ops {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ T &a, ಧ್ರುವ T &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ T &a, Hasher h) {
		ಯದಿ constexpr (std::is_integral_v<T>) {
			static_assert(ಪರಿಮಾಣ(T) <= ಪರಿಮಾಣ(uint64_t));
			ಯದಿ (ಪರಿಮಾಣ(T) == ಪರಿಮಾಣ(uint64_t))
				h.hash64(a);
			ಅನ್ಯಥಾ
				h.hash32(a);
			ಪ್ರತ್ಯಾಗಮ h;
		} ಅನ್ಯಥಾ ಯದಿ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ಪ್ರತ್ಯಾಗಮ hash_ops<u_type>::hash_into((u_type) a, h);
		} ಅನ್ಯಥಾ ಯದಿ constexpr (std::is_pointer_v<T>) {
			ಪ್ರತ್ಯಾಗಮ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} ಅನ್ಯಥಾ ಯದಿ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			ಪೂರ್ಣಾಙ್ಕ size = a.size();
			ಪೂರ್ಣಾಙ್ಕ i = 0;
			ಯಾವತ್ (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			ಪ್ರತ್ಯಾಗಮ h;
		} ಅನ್ಯಥಾ {
			ಪ್ರತ್ಯಾಗಮ a.hash_into(h);
		}
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ T &a) HASH_ತುದಿ_LOOP_SND
};

template<typename P, typename Q> ಸಂರಚನಾ hash_ops<std::pair<P, Q>> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::pair<P, Q> &a, ಧ್ರುವ std::pair<P, Q> &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		ಪ್ರತ್ಯಾಗಮ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ std::pair<P, Q> &a) HASH_ತುದಿ_LOOP_SND
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash(ಧ್ರುವ P &p, ಧ್ರುವ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

template<typename... T> ಸಂರಚನಾ hash_ops<std::tuple<T...>> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::tuple<T...> &a, ಧ್ರುವ std::tuple<T...> &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	template<size_t I = 0>
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ typename std::enable_if<I == ಪರಿಮಾಣ...(T), Hasher>::type hash_into(ಧ್ರುವ std::tuple<T...> &, Hasher h) {
		ಪ್ರತ್ಯಾಗಮ h;
	}
	template<size_t I = 0>
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ typename std::enable_if<I != ಪರಿಮಾಣ...(T), Hasher>::type hash_into(ಧ್ರುವ std::tuple<T...> &a, Hasher h) {
		ಪ್ರಕಾರನಿರ್ದೇಶ hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ಪ್ರತ್ಯಾಗಮ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ std::tuple<T...> &a) HASH_ತುದಿ_LOOP_SND
};

template<typename T> ಸಂರಚನಾ hash_ops<std::vector<T>> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::vector<T> &a, ಧ್ರುವ std::vector<T> &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		ಪ್ರತಿ (auto k : a)
			h.eat(k);
		ಪ್ರತ್ಯಾಗಮ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ std::vector<T> &a) HASH_ತುದಿ_LOOP_SND
};

template<typename T, size_t N> ಸಂರಚನಾ hash_ops<std::array<T, N>> {
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::array<T, N> &a, ಧ್ರುವ std::array<T, N> &b) {
        ಪ್ರತ್ಯಾಗಮ a == b;
    }
    [[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ std::array<T, N> &a, Hasher h) {
        ಪ್ರತಿ (ಧ್ರುವ auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ಪ್ರತ್ಯಾಗಮ h;
    }
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ std::array<T, N> &a) HASH_ತುದಿ_LOOP_SND
};

ಸಂರಚನಾ hash_cstr_ops {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
		ಪ್ರತ್ಯಾಗಮ strcmp(a, b) == 0;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ ವರ್ಣ *a, Hasher h) {
		ಯಾವತ್ (*a)
			h.hash32(*(a++));
		ಪ್ರತ್ಯಾಗಮ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ ವರ್ಣ *a) HASH_ತುದಿ_LOOP_SND
};

template <> ಸಂರಚನಾ hash_ops<ವರ್ಣ*> : hash_cstr_ops {};

ಸಂರಚನಾ hash_ptr_ops {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ ಶೂನ್ಯ *a, Hasher h) {
		ಪ್ರತ್ಯಾಗಮ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ ಶೂನ್ಯ *a) HASH_ತುದಿ_LOOP_SND
};

ಸಂರಚನಾ hash_obj_ops {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	template<typename T>
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ T *a, Hasher h) {
		ಯದಿ (a)
			h = a->hash_into(h);
		ಅನ್ಯಥಾ
			h.eat(0);
		ಪ್ರತ್ಯಾಗಮ h;
	}
	template<typename T>
	HASH_ತುದಿ_LOOP_FST (ಧ್ರುವ T *a) HASH_ತುದಿ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ಧ್ರುವ T& obj) {
	ಪ್ರತ್ಯಾಗಮ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ mkhash(ಧ್ರುವ T &v) {
	ಪ್ರತ್ಯಾಗಮ (unsigned ಪೂರ್ಣಾಙ್ಕ) run_hash<T>(v);
}

template<> ಸಂರಚನಾ hash_ops<std::monostate> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(std::monostate a, std::monostate b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(std::monostate, Hasher h) {
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

template<typename... T> ಸಂರಚನಾ hash_ops<std::variant<T...>> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::variant<T...> &a, ಧ್ರುವ std::variant<T...> &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ std::variant<T...> &a, Hasher h) {
		std::visit([& h](ಧ್ರುವ auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

template<typename T> ಸಂರಚನಾ hash_ops<std::optional<T>> {
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ std::optional<T> &a, ಧ್ರುವ std::optional<T> &b) {
		ಪ್ರತ್ಯಾಗಮ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ std::optional<T> &a, Hasher h) {
		ಯದಿ(a.has_ಮೌಲ್ಯ())
			h.eat(*a);
		ಅನ್ಯಥಾ
			h.eat(0);
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ hashtable_size(unsigned ಪೂರ್ಣಾಙ್ಕ min_size)
{
	// Primes as generated by https://oeis.org/A175953
	ಸ್ಥಿರ std::vector<unsigned ಪೂರ್ಣಾಙ್ಕ> zero_and_some_primes = {
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

	ಪ್ರತಿ (auto p : zero_and_some_primes)
		ಯದಿ (p >= min_size) ಪ್ರತ್ಯಾಗಮ p;

	ಯದಿ (ಪರಿಮಾಣ(unsigned ಪೂರ್ಣಾಙ್ಕ) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	ಪ್ರತಿ (auto p : zero_and_some_primes)
		ಯದಿ (100129 * p > min_size) ಪ್ರತ್ಯಾಗಮ 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, ಪೂರ್ಣಾಙ್ಕ offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
ಸಾರ್ವಜನಿಕ:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	ಶೂನ್ಯ eat(ಧ್ರುವ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	ಶೂನ್ಯ eat(ಧ್ರುವ Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		ಪ್ರತಿ (auto b : buckets)
			h.eat(b);
		ಪ್ರತ್ಯಾಗಮ h;
	}
ಗುಪ್ತ:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	ಸಂರಚನಾ entry_t
	{
		std::pair<K, T> udata;
		ಪೂರ್ಣಾಙ್ಕ next;

		entry_t() { }
		entry_t(ಧ್ರುವ std::pair<K, T> &udata, ಪೂರ್ಣಾಙ್ಕ next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, ಪೂರ್ಣಾಙ್ಕ next) : udata(std::move(udata)), next(next) { }
		bool operator<(ಧ್ರುವ entry_t &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ udata.first < other.udata.first; }
	};

	std::vector<ಪೂರ್ಣಾಙ್ಕ> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ do_assert(bool) { }
#ಅನ್ಯಥಾ
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ do_assert(bool cond) {
		ಯದಿ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = 0;
		ಯದಿ (!hashtable.empty())
			hash = ops.hash(ಕೀಲಿ).yield() % (unsigned ಪೂರ್ಣಾಙ್ಕ)(hashtable.size());
		ಪ್ರತ್ಯಾಗಮ hash;
	}

	ಶೂನ್ಯ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಪೂರ್ಣಾಙ್ಕ(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ಪೂರ್ಣಾಙ್ಕ do_erase(ಪೂರ್ಣಾಙ್ಕ index, Hasher::hash_t hash)
	{
		do_assert(index < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
		ಯದಿ (hashtable.empty() || index < 0)
			ಪ್ರತ್ಯಾಗಮ 0;

		ಪೂರ್ಣಾಙ್ಕ k = hashtable[hash];
		do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));

		ಯದಿ (k == index) {
			hashtable[hash] = entries[index].next;
		} ಅನ್ಯಥಾ {
			ಯಾವತ್ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ಪೂರ್ಣಾಙ್ಕ back_idx = entries.size()-1;

		ಯದಿ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));

			ಯದಿ (k == back_idx) {
				hashtable[back_hash] = index;
			} ಅನ್ಯಥಾ {
				ಯಾವತ್ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.ತೆಗೆ_back();

		ಯದಿ (entries.empty())
			hashtable.clear();

		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty())
			ಪ್ರತ್ಯಾಗಮ -1;

		ಯದಿ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ಕೀಲಿ);
		}

		ಪ್ರತ್ಯಾಗಮ do_lookup_internal(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup_internal(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t hash) ಧ್ರುವ
	{
		ಪೂರ್ಣಾಙ್ಕ index = hashtable[hash];

		ಯಾವತ್ (index >= 0 && !ops.cmp(entries[index].udata.first, ಕೀಲಿ)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
		}

		ಪ್ರತ್ಯಾಗಮ index;
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup_no_rehash(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t hash) ಧ್ರುವ
	{
		ಯದಿ (hashtable.empty())
			ಪ್ರತ್ಯಾಗಮ -1;

		ಪ್ರತ್ಯಾಗಮ do_lookup_internal(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಙ್ಕ do_ಸೇರಿಸು(ಧ್ರುವ K &ಕೀಲಿ, ಧ್ರುವ Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(ಕೀಲಿ, T()), -1);
			do_rehash();
		} ಅನ್ಯಥಾ {
			entries.emplace_back(std::pair<K, T>(ಕೀಲಿ, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ಪ್ರತ್ಯಾಗಮ entries.size() - 1;
	}

	ಪೂರ್ಣಾಙ್ಕ do_ಸೇರಿಸು(ಧ್ರುವ std::pair<K, T> &ಮೌಲ್ಯ, ಧ್ರುವ Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty()) {
			entries.emplace_back(ಮೌಲ್ಯ, -1);
			do_rehash();
		} ಅನ್ಯಥಾ {
			entries.emplace_back(ಮೌಲ್ಯ, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ಪ್ರತ್ಯಾಗಮ entries.size() - 1;
	}

	ಪೂರ್ಣಾಙ್ಕ do_ಸೇರಿಸು(std::pair<K, T> &&rvalue, ಧ್ರುವ Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} ಅನ್ಯಥಾ {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ಪ್ರತ್ಯಾಗಮ entries.size() - 1;
	}

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		friend class dict;
	ರಕ್ಷಿತ:
		ಧ್ರುವ dict *ptr;
		ಪೂರ್ಣಾಙ್ಕ index;
		const_iterator(ಧ್ರುವ dict *ptr, ಪೂರ್ಣಾಙ್ಕ index) : ptr(ptr), index(index) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರನಿರ್ದೇಶ std::bidirectional_iterator_tag iterator_category;
		ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<K, T> ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ಧ್ರುವ std::pair<K, T>* pointer;
		ಪ್ರಕಾರನಿರ್ದೇಶ ಧ್ರುವ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ಪ್ರತ್ಯಾಗಮ *this; }
		const_iterator operator++(ಪೂರ್ಣಾಙ್ಕ) { const_iterator tmp = *this; index--; ಪ್ರತ್ಯಾಗಮ tmp; }
		const_iterator operator--() { index++; ಪ್ರತ್ಯಾಗಮ *this; }
		const_iterator operator--(ಪೂರ್ಣಾಙ್ಕ) { const_iterator tmp = *this; index++; ಪ್ರತ್ಯಾಗಮ tmp; }
		const_iterator operator+=(ಪೂರ್ಣಾಙ್ಕ amt) { index -= amt; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator<(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index > other.index; }
		bool operator==(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
		bool operator!=(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
		ಧ್ರುವ std::pair<K, T> &operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		ಧ್ರುವ std::pair<K, T> *operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	ರಕ್ಷಿತ:
		dict *ptr;
		ಪೂರ್ಣಾಙ್ಕ index;
		iterator(dict *ptr, ಪೂರ್ಣಾಙ್ಕ index) : ptr(ptr), index(index) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರನಿರ್ದೇಶ std::forward_iterator_tag iterator_category;
		ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<K, T> ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<K, T>* pointer;
		ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; ಪ್ರತ್ಯಾಗಮ *this; }
		iterator operator+=(ಪೂರ್ಣಾಙ್ಕ amt) { index -= amt; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator<(ಧ್ರುವ iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index > other.index; }
		bool operator==(ಧ್ರುವ iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
		bool operator!=(ಧ್ರುವ iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
		std::pair<K, T> &operator*() { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
		ಧ್ರುವ std::pair<K, T> &operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		ಧ್ರುವ std::pair<K, T> *operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
		operator const_iterator() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ಧ್ರುವ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ಧ್ರುವ dict &other) {
		entries = other.entries;
		do_rehash();
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	dict(ಧ್ರುವ std::initializer_list<std::pair<K, T>> &list)
	{
		ಪ್ರತಿ (auto &it : list)
			ಸೇರಿಸು(it);
	}

	template<class ಒಳಹರಿವು_Iterator>
	dict(ಒಳಹರಿವು_Iterator first, ಒಳಹರಿವು_Iterator last)
	{
		ಸೇರಿಸು(first, last);
	}

	template<class ಒಳಹರಿವು_Iterator>
	ಶೂನ್ಯ ಸೇರಿಸು(ಒಳಹರಿವು_Iterator first, ಒಳಹರಿವು_Iterator last)
	{
		ಪ್ರತಿ (; first != last; ++first)
			ಸೇರಿಸು(*first);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಧ್ರುವ std::pair<K, T> &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಮೌಲ್ಯ.first);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಮೌಲ್ಯ.first, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಮೌಲ್ಯ, hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(rvalue.first, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::forward<std::pair<K, T>>(rvalue), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ಧ್ರುವ &ಕೀಲಿ, T ಧ್ರುವ &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(ಕೀಲಿ, ಮೌಲ್ಯ), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ಧ್ರುವ &ಕೀಲಿ, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(ಕೀಲಿ, std::forward<T>(rvalue)), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ಧ್ರುವ &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(rkey, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(std::forward<K>(rkey), ಮೌಲ್ಯ), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(rkey, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ಪೂರ್ಣಾಙ್ಕ erase(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ index = do_lookup(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		ಪ್ರತ್ಯಾಗಮ ++it;
	}

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(ಧ್ರುವ K &ಕೀಲಿ, const_iterator it) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ end();
		ಪ್ರತ್ಯಾಗಮ iterator(this, i);
	}

	const_iterator find(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ end();
		ಪ್ರತ್ಯಾಗಮ const_iterator(this, i);
	}

	T& at(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			throw std::out_of_range("dict::at()");
		ಪ್ರತ್ಯಾಗಮ entries[i].udata.second;
	}

	ಧ್ರುವ T& at(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			throw std::out_of_range("dict::at()");
		ಪ್ರತ್ಯಾಗಮ entries[i].udata.second;
	}

	ಧ್ರುವ T& at(ಧ್ರುವ K &ಕೀಲಿ, ಧ್ರುವ T &defval) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ defval;
		ಪ್ರತ್ಯಾಗಮ entries[i].udata.second;
	}

	T& operator[](ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			i = do_ಸೇರಿಸು(std::pair<K, T>(ಕೀಲಿ, T()), hash);
		ಪ್ರತ್ಯಾಗಮ entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	ಶೂನ್ಯ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ಧ್ರುವ entry_t &a, ಧ್ರುವ entry_t &b){ ಪ್ರತ್ಯಾಗಮ comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	ಶೂನ್ಯ swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ಧ್ರುವ dict &other) ಧ್ರುವ {
		ಯದಿ (size() != other.size())
			ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತಿ (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			ಯದಿ (oit == other.end() || !(oit->second == it.udata.second))
				ಪ್ರತ್ಯಾಗಮ false;
		}
		ಪ್ರತ್ಯಾಗಮ true;
	}

	bool operator!=(ಧ್ರುವ dict &other) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		commutative_hash comm;
		ಪ್ರತಿ (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		ಪ್ರತ್ಯಾಗಮ comm.hash_into(h);
	}

	ಶೂನ್ಯ reserve(size_t n) { entries.reserve(n); }
	size_t size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ entries.size(); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ entries.empty(); }
	ಶೂನ್ಯ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ಪ್ರತ್ಯಾಗಮ iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1); }
	iterator element(ಪೂರ್ಣಾಙ್ಕ n) { ಪ್ರತ್ಯಾಗಮ iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1-n); }
	iterator end() { ಪ್ರತ್ಯಾಗಮ iterator(nullptr, -1); }

	const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1); }
	const_iterator element(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1-n); }
	const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, ಪೂರ್ಣಾಙ್ಕ, typename> friend class idict;

ರಕ್ಷಿತ:
	ಸಂರಚನಾ entry_t
	{
		K udata;
		ಪೂರ್ಣಾಙ್ಕ next;

		entry_t() { }
		entry_t(ಧ್ರುವ K &udata, ಪೂರ್ಣಾಙ್ಕ next) : udata(udata), next(next) { }
		entry_t(K &&udata, ಪೂರ್ಣಾಙ್ಕ next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<ಪೂರ್ಣಾಙ್ಕ> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ do_assert(bool) { }
#ಅನ್ಯಥಾ
	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ do_assert(bool cond) {
		ಯದಿ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = 0;
		ಯದಿ (!hashtable.empty())
			hash = ops.hash(ಕೀಲಿ).yield() % (unsigned ಪೂರ್ಣಾಙ್ಕ)(hashtable.size());
		ಪ್ರತ್ಯಾಗಮ hash;
	}

	ಶೂನ್ಯ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಪೂರ್ಣಾಙ್ಕ(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ಪೂರ್ಣಾಙ್ಕ do_erase(ಪೂರ್ಣಾಙ್ಕ index, Hasher::hash_t hash)
	{
		do_assert(index < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
		ಯದಿ (hashtable.empty() || index < 0)
			ಪ್ರತ್ಯಾಗಮ 0;

		ಪೂರ್ಣಾಙ್ಕ k = hashtable[hash];
		ಯದಿ (k == index) {
			hashtable[hash] = entries[index].next;
		} ಅನ್ಯಥಾ {
			ಯಾವತ್ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ಪೂರ್ಣಾಙ್ಕ back_idx = entries.size()-1;

		ಯದಿ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			ಯದಿ (k == back_idx) {
				hashtable[back_hash] = index;
			} ಅನ್ಯಥಾ {
				ಯಾವತ್ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.ತೆಗೆ_back();

		ಯದಿ (entries.empty())
			hashtable.clear();

		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty())
			ಪ್ರತ್ಯಾಗಮ -1;

		ಯದಿ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ಕೀಲಿ);
		}

		ಪ್ರತ್ಯಾಗಮ do_lookup_internal(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup_internal(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t hash) ಧ್ರುವ
	{
		ಪೂರ್ಣಾಙ್ಕ index = hashtable[hash];

		ಯಾವತ್ (index >= 0 && !ops.cmp(entries[index].udata, ಕೀಲಿ)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ಪೂರ್ಣಾಙ್ಕ(entries.size()));
		}

		ಪ್ರತ್ಯಾಗಮ index;
	}

	ಪೂರ್ಣಾಙ್ಕ do_lookup_no_rehash(ಧ್ರುವ K &ಕೀಲಿ, Hasher::hash_t hash) ಧ್ರುವ
	{
		ಯದಿ (hashtable.empty())
			ಪ್ರತ್ಯಾಗಮ -1;

		ಪ್ರತ್ಯಾಗಮ do_lookup_internal(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಙ್ಕ do_ಸೇರಿಸು(ಧ್ರುವ K &ಮೌಲ್ಯ, Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty()) {
			entries.emplace_back(ಮೌಲ್ಯ, -1);
			do_rehash();
			hash = do_hash(ಮೌಲ್ಯ);
		} ಅನ್ಯಥಾ {
			entries.emplace_back(ಮೌಲ್ಯ, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ಪ್ರತ್ಯಾಗಮ entries.size() - 1;
	}

	ಪೂರ್ಣಾಙ್ಕ do_ಸೇರಿಸು(K &&rvalue, Hasher::hash_t &hash)
	{
		ಯದಿ (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} ಅನ್ಯಥಾ {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ಪ್ರತ್ಯಾಗಮ entries.size() - 1;
	}

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		friend class pool;
	ರಕ್ಷಿತ:
		ಧ್ರುವ pool *ptr;
		ಪೂರ್ಣಾಙ್ಕ index;
		const_iterator(ಧ್ರುವ pool *ptr, ಪೂರ್ಣಾಙ್ಕ index) : ptr(ptr), index(index) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರನಿರ್ದೇಶ std::forward_iterator_tag iterator_category;
		ಪ್ರಕಾರನಿರ್ದೇಶ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ K* pointer;
		ಪ್ರಕಾರನಿರ್ದೇಶ K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator==(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
		bool operator!=(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
		ಧ್ರುವ K &operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		ಧ್ರುವ K *operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	ರಕ್ಷಿತ:
		pool *ptr;
		ಪೂರ್ಣಾಙ್ಕ index;
		iterator(pool *ptr, ಪೂರ್ಣಾಙ್ಕ index) : ptr(ptr), index(index) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರನಿರ್ದೇಶ std::forward_iterator_tag iterator_category;
		ಪ್ರಕಾರನಿರ್ದೇಶ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ K* pointer;
		ಪ್ರಕಾರನಿರ್ದೇಶ K& reference;
		iterator() { }
		iterator operator++() { index--; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator==(ಧ್ರುವ iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
		bool operator!=(ಧ್ರುವ iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
		K &operator*() { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		K *operator->() { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
		ಧ್ರುವ K &operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ptr->entries[index].udata; }
		ಧ್ರುವ K *operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ &ptr->entries[index].udata; }
		operator const_iterator() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(ಧ್ರುವ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ಧ್ರುವ pool &other) {
		entries = other.entries;
		do_rehash();
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	pool(ಧ್ರುವ std::initializer_list<K> &list)
	{
		ಪ್ರತಿ (auto &it : list)
			ಸೇರಿಸು(it);
	}

	template<class ಒಳಹರಿವು_Iterator>
	pool(ಒಳಹರಿವು_Iterator first, ಒಳಹರಿವು_Iterator last)
	{
		ಸೇರಿಸು(first, last);
	}

	template<class ಒಳಹರಿವು_Iterator>
	ಶೂನ್ಯ ಸೇರಿಸು(ಒಳಹರಿವು_Iterator first, ಒಳಹರಿವು_Iterator last)
	{
		ಪ್ರತಿ (; first != last; ++first)
			ಸೇರಿಸು(*first);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಧ್ರುವ K &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಮೌಲ್ಯ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಮೌಲ್ಯ, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಮೌಲ್ಯ, hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(rvalue, hash);
		ಯದಿ (i >= 0)
			ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::forward<K>(rvalue), hash);
		ಪ್ರತ್ಯಾಗಮ std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		ಪ್ರತ್ಯಾಗಮ ಸೇರಿಸು(K(std::forward<Args>(args)...));
	}

	ಪೂರ್ಣಾಙ್ಕ erase(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ index = do_lookup(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		ಪ್ರತ್ಯಾಗಮ ++it;
	}

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(ಧ್ರುವ K &ಕೀಲಿ, const_iterator it) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ end();
		ಪ್ರತ್ಯಾಗಮ iterator(this, i);
	}

	const_iterator find(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ end();
		ಪ್ರತ್ಯಾಗಮ const_iterator(this, i);
	}

	bool operator[](ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = do_lookup(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i >= 0;
	}

	template<typename Compare = std::less<K>>
	ಶೂನ್ಯ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ಧ್ರುವ entry_t &a, ಧ್ರುವ entry_t &b){ ಪ್ರತ್ಯಾಗಮ comp(b.udata, a.udata); });
		do_rehash();
	}

	K ತೆಗೆ()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಶೂನ್ಯ swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ಧ್ರುವ pool &other) ಧ್ರುವ {
		ಯದಿ (size() != other.size())
			ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತಿ (auto &it : entries)
			ಯದಿ (!other.ಎಣಿಕೆ(it.udata))
				ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ true;
	}

	bool operator!=(ಧ್ರುವ pool &other) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		commutative_hash comm;
		ಪ್ರತಿ (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ಪ್ರತ್ಯಾಗಮ comm.hash_into(h);
	}

	ಶೂನ್ಯ reserve(size_t n) { entries.reserve(n); }
	size_t size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ entries.size(); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ entries.empty(); }
	ಶೂನ್ಯ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ಪ್ರತ್ಯಾಗಮ iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1); }
	iterator element(ಪೂರ್ಣಾಙ್ಕ n) { ಪ್ರತ್ಯಾಗಮ iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1-n); }
	iterator end() { ಪ್ರತ್ಯಾಗಮ iterator(nullptr, -1); }

	const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1); }
	const_iterator element(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(this, ಪೂರ್ಣಾಙ್ಕ(entries.size())-1-n); }
	const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(nullptr, -1); }
};

template<typename K, ಪೂರ್ಣಾಙ್ಕ offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		friend class idict;
	ರಕ್ಷಿತ:
		ಧ್ರುವ idict &container;
		ಪೂರ್ಣಾಙ್ಕ index;
		const_iterator(ಧ್ರುವ idict &container, ಪೂರ್ಣಾಙ್ಕ index) : container(container), index(index) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರನಿರ್ದೇಶ std::forward_iterator_tag iterator_category;
		ಪ್ರಕಾರನಿರ್ದೇಶ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
		ಪ್ರಕಾರನಿರ್ದೇಶ K* pointer;
		ಪ್ರಕಾರನಿರ್ದೇಶ K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator==(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
		bool operator!=(ಧ್ರುವ const_iterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
		ಧ್ರುವ K &operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ container[index]; }
		ಧ್ರುವ K *operator->() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ &container[index]; }
	};

	constexpr idict()
	{
	}

	ಪೂರ್ಣಾಙ್ಕ operator()(ಧ್ರುವ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = database.do_lookup(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			i = database.do_ಸೇರಿಸು(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i + offset;
	}

	ಪೂರ್ಣಾಙ್ಕ at(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			throw std::out_of_range("idict::at()");
		ಪ್ರತ್ಯಾಗಮ i + offset;
	}

	ಪೂರ್ಣಾಙ್ಕ at(ಧ್ರುವ K &ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ defval) ಧ್ರುವ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ defval;
		ಪ್ರತ್ಯಾಗಮ i + offset;
	}

	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಙ್ಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಪ್ರತ್ಯಾಗಮ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಙ್ಕ lookup(ಧ್ರುವ K &ಕೀಲಿ) ಧ್ರುವ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪ್ರತ್ಯಾಗಮ database.do_lookup_no_rehash(ಕೀಲಿ, hash);
	}

	ಶೂನ್ಯ expect(ಧ್ರುವ K &ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ i)
	{
		ಪೂರ್ಣಾಙ್ಕ j = (*this)(ಕೀಲಿ);
		ಯದಿ (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ಧ್ರುವ K &operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ database.entries.at(index - offset).udata;
	}

	ಶೂನ್ಯ swap(idict &other)
	{
		database.swap(other.database);
	}

	ಶೂನ್ಯ reserve(size_t n) { database.reserve(n); }
	size_t size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.size(); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.empty(); }
	ಶೂನ್ಯ clear() { database.clear(); }

	const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(*this, offset); }
	const_iterator element(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(*this, n); }
	const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(*this, offset + size()); }
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
	ಸಾರ್ವಜನಿಕ:
		explicit AtomicParent(ಪೂರ್ಣಾಙ್ಕ p) : parent(p) {}
		AtomicParent(ಧ್ರುವ AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ಧ್ರುವ AtomicParent &other) { set(other.get()); ಪ್ರತ್ಯಾಗಮ *this; }
		ಪೂರ್ಣಾಙ್ಕ get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ parent.load(std::memory_order_relaxed); }
		ಶೂನ್ಯ set(ಪೂರ್ಣಾಙ್ಕ p) { parent.store(p, std::memory_order_relaxed); }
	ಗುಪ್ತ:
		std::atomic<ಪೂರ್ಣಾಙ್ಕ> parent;
	};
	std::vector<AtomicParent> parents;

ಸಾರ್ವಜನಿಕ:
	ಪ್ರಕಾರನಿರ್ದೇಶ typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	ಪೂರ್ಣಾಙ್ಕ operator()(ಧ್ರುವ K &ಕೀಲಿ)
	{
		ಪೂರ್ಣಾಙ್ಕ i = database(ಕೀಲಿ);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		ಯದಿ (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		ಪ್ರತ್ಯಾಗಮ i;
	}

	// Finds an element at given index
	ಧ್ರುವ K &operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ database[index];
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
	ಪೂರ್ಣಾಙ್ಕ ifind(ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
	{
		ಪೂರ್ಣಾಙ್ಕ p = i, k = i;

		ಯಾವತ್ (true) {
			ಪೂರ್ಣಾಙ್ಕ pp = parents[p].get();
			ಯದಿ (pp < 0)
				ಭಙ್ಗ;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		ಯಾವತ್ (k != p) {
			ಪೂರ್ಣಾಙ್ಕ next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		ಪ್ರತ್ಯಾಗಮ p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	ಶೂನ್ಯ imerge(ಪೂರ್ಣಾಙ್ಕ i, ಪೂರ್ಣಾಙ್ಕ j)
	{
		i = ifind(i);
		j = ifind(j);

		ಯದಿ (i != j)
			parents[i].set(j);
	}

	ಶೂನ್ಯ ipromote(ಪೂರ್ಣಾಙ್ಕ i)
	{
		ಪೂರ್ಣಾಙ್ಕ k = i;

		ಯಾವತ್ (k != -1) {
			ಪೂರ್ಣಾಙ್ಕ next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	ಪೂರ್ಣಾಙ್ಕ lookup(ಧ್ರುವ K &a)
	{
		ಪ್ರತ್ಯಾಗಮ ifind((*this)(a));
	}

	ಧ್ರುವ K &find(ಧ್ರುವ K &a) ಧ್ರುವ
	{
		ಪೂರ್ಣಾಙ್ಕ i = database.at(a, -1);
		ಯದಿ (i < 0)
			ಪ್ರತ್ಯಾಗಮ a;
		ಪ್ರತ್ಯಾಗಮ (*this)[ifind(i)];
	}

	ಶೂನ್ಯ merge(ಧ್ರುವ K &a, ಧ್ರುವ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	ಶೂನ್ಯ promote(ಧ್ರುವ K &a)
	{
		ಪೂರ್ಣಾಙ್ಕ i = database.at(a, -1);
		ಯದಿ (i >= 0)
			ipromote(i);
	}

	ಶೂನ್ಯ swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	ಶೂನ್ಯ reserve(size_t n) { database.reserve(n); }
	size_t size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.size(); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.empty(); }
	ಶೂನ್ಯ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.begin(); }
	const_iterator element(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.element(n); }
	const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ database.end(); }
};

} /* namespace hashlib */

#endif
