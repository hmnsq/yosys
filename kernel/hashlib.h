// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HASHLIB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHLIB_H

#ਅਨ੍ਤਰ੍ਭਾਵ <array>
#ਅਨ੍ਤਰ੍ਭਾਵ <atomic>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdexcept>
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <variant>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_HASHING_ਸੰਸਕਰਣ 1

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

#ਜੇ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ __attribute__((warn_ਅਣਵਰਤਿਆ))
#ਨਹੀਂਤਾਂ
#  define HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

ਅਚਲ ਪੂਰਨ hashtable_ਮਾਪ_trigger = 2;
ਅਚਲ ਪੂਰਨ hashtable_ਮਾਪ_ਗੁਣਕ = 3;

namespace legacy {
	ਸਤਰਅੰਦਰ uint32_t djb2_ਜੋੜੋ(uint32_t a, uint32_t b) {
		ਵਾਪਸ ((a << 5) + a) + b;
	}
};

ਸਾਂਚਾ<typename T>
ਬਣਤਰ hash_ops;

ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mkhash_xorshift(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ a) {
	ਜੇ (ਆਕਾਰ(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} ਨਹੀਂਤਾਂ ਜੇ (ਆਕਾਰ(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} ਨਹੀਂਤਾਂ
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ਵਾਪਸ a;
}

class HasherDJB32 {
ਜਨਤਕ:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ਨਿਜੀ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ਸਥਿਰ uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ਵਾਪਸ hash;
	}
ਜਨਤਕ:
	ਖਾਲੀ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ਵਾਪਸ;
	}
	ਖਾਲੀ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ਵਾਪਸ;
	}
	[[nodiscard]]
	hash_t yield() ਅਚਲ {
		ਵਾਪਸ (hash_t)state;
	}

	ਸਾਂਚਾ<typename T>
	ਖਾਲੀ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	ਸਾਂਚਾ<typename T>
	ਖਾਲੀ eat(ਅਚਲ T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	ਖਾਲੀ commutative_eat(hash_t t) {
		state ^= t;
	}

	ਖਾਲੀ force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_ਸਿਖਰ_LOOP_FST [[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_ਸਿਖਰ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ਵਾਪਸ h; \
}

ਸਾਂਚਾ<typename T>
ਬਣਤਰ hash_ops {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ T &a, ਅਚਲ T &b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ T &a, Hasher h) {
		ਜੇ constexpr (std::is_integral_v<T>) {
			static_assert(ਆਕਾਰ(T) <= ਆਕਾਰ(uint64_t));
			ਜੇ (ਆਕਾਰ(T) == ਆਕਾਰ(uint64_t))
				h.hash64(a);
			ਨਹੀਂਤਾਂ
				h.hash32(a);
			ਵਾਪਸ h;
		} ਨਹੀਂਤਾਂ ਜੇ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ਵਾਪਸ hash_ops<u_type>::hash_into((u_type) a, h);
		} ਨਹੀਂਤਾਂ ਜੇ constexpr (std::is_ਸੂਚਕ_v<T>) {
			ਵਾਪਸ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} ਨਹੀਂਤਾਂ ਜੇ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			ਪੂਰਨ ਮਾਪ = a.ਮਾਪ();
			ਪੂਰਨ i = 0;
			ਜਦੋਂ (i + 8 < ਮਾਪ) {
				uint64_t v;
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(&v, a.ਅੰਕੜੇ() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&v, a.ਅੰਕੜੇ() + i, ਮਾਪ - i);
			h.hash64(v);
			ਵਾਪਸ h;
		} ਨਹੀਂਤਾਂ {
			ਵਾਪਸ a.hash_into(h);
		}
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ T &a) HASH_ਸਿਖਰ_LOOP_SND
};

ਸਾਂਚਾ<typename P, typename Q> ਬਣਤਰ hash_ops<std::pair<P, Q>> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::pair<P, Q> &a, ਅਚਲ std::pair<P, Q> &b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.ਪਹਿਲਾ, h);
		h = hash_ops<Q>::hash_into(a.ਦੂਜਾ, h);
		ਵਾਪਸ h;
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ std::pair<P, Q> &a) HASH_ਸਿਖਰ_LOOP_SND
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash(ਅਚਲ P &p, ਅਚਲ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ਵਾਪਸ h;
	}
};

ਸਾਂਚਾ<typename... T> ਬਣਤਰ hash_ops<std::tuple<T...>> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::tuple<T...> &a, ਅਚਲ std::tuple<T...> &b) {
		ਵਾਪਸ a == b;
	}
	ਸਾਂਚਾ<ਮਾਪ_t I = 0>
	ਸਥਿਰ ਸਤਰਅੰਦਰ typename std::enable_if<I == ਆਕਾਰ...(T), Hasher>::type hash_into(ਅਚਲ std::tuple<T...> &, Hasher h) {
		ਵਾਪਸ h;
	}
	ਸਾਂਚਾ<ਮਾਪ_t I = 0>
	ਸਥਿਰ ਸਤਰਅੰਦਰ typename std::enable_if<I != ਆਕਾਰ...(T), Hasher>::type hash_into(ਅਚਲ std::tuple<T...> &a, Hasher h) {
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ਵਾਪਸ h;
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ std::tuple<T...> &a) HASH_ਸਿਖਰ_LOOP_SND
};

ਸਾਂਚਾ<typename T> ਬਣਤਰ hash_ops<std::ਸਦਿਸ਼<T>> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::ਸਦਿਸ਼<T> &a, ਅਚਲ std::ਸਦਿਸ਼<T> &b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ std::ਸਦਿਸ਼<T> &a, Hasher h) {
		h.eat((uint32_t)a.ਮਾਪ());
		ਲਈ (ਸਵੈਚਾਲਿਤ k : a)
			h.eat(k);
		ਵਾਪਸ h;
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ std::ਸਦਿਸ਼<T> &a) HASH_ਸਿਖਰ_LOOP_SND
};

ਸਾਂਚਾ<typename T, ਮਾਪ_t N> ਬਣਤਰ hash_ops<std::array<T, N>> {
    ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::array<T, N> &a, ਅਚਲ std::array<T, N> &b) {
        ਵਾਪਸ a == b;
    }
    [[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ std::array<T, N> &a, Hasher h) {
        ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ਵਾਪਸ h;
    }
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ std::array<T, N> &a) HASH_ਸਿਖਰ_LOOP_SND
};

ਬਣਤਰ hash_cstr_ops {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
		ਵਾਪਸ strcmp(a, b) == 0;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ ਅੱਖਰ *a, Hasher h) {
		ਜਦੋਂ (*a)
			h.hash32(*(a++));
		ਵਾਪਸ h;
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ ਅੱਖਰ *a) HASH_ਸਿਖਰ_LOOP_SND
};

ਸਾਂਚਾ <> ਬਣਤਰ hash_ops<ਅੱਖਰ*> : hash_cstr_ops {};

ਬਣਤਰ hash_ptr_ops {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ ਖਾਲੀ *a, Hasher h) {
		ਵਾਪਸ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ ਖਾਲੀ *a) HASH_ਸਿਖਰ_LOOP_SND
};

ਬਣਤਰ hash_obj_ops {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b) {
		ਵਾਪਸ a == b;
	}
	ਸਾਂਚਾ<typename T>
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ T *a, Hasher h) {
		ਜੇ (a)
			h = a->hash_into(h);
		ਨਹੀਂਤਾਂ
			h.eat(0);
		ਵਾਪਸ h;
	}
	ਸਾਂਚਾ<typename T>
	HASH_ਸਿਖਰ_LOOP_FST (ਅਚਲ T *a) HASH_ਸਿਖਰ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
ਸਾਂਚਾ<typename T>
[[nodiscard]]
Hasher::hash_t ਲੜੀਵਾਰ_hash(ਅਚਲ T& obj) {
	ਵਾਪਸ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
ਸਾਂਚਾ<typename T>
[[nodiscard]]
[[deprecated]]
ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mkhash(ਅਚਲ T &v) {
	ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) ਲੜੀਵਾਰ_hash<T>(v);
}

ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<std::monostate> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(std::monostate a, std::monostate b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(std::monostate, Hasher h) {
		ਵਾਪਸ h;
	}
};

ਸਾਂਚਾ<typename... T> ਬਣਤਰ hash_ops<std::variant<T...>> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::variant<T...> &a, ਅਚਲ std::variant<T...> &b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ std::variant<T...> &a, Hasher h) {
		std::ਦੇਖੋ([& h](ਅਚਲ ਸਵੈਚਾਲਿਤ &v) { h.eat(v); }, a);
		h.eat(a.ਸੂਚਕਾਂਕ());
		ਵਾਪਸ h;
	}
};

ਸਾਂਚਾ<typename T> ਬਣਤਰ hash_ops<std::optional<T>> {
	ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ std::optional<T> &a, ਅਚਲ std::optional<T> &b) {
		ਵਾਪਸ a == b;
	}
	[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ std::optional<T> &a, Hasher h) {
		ਜੇ(a.has_ਮੁੱਲ())
			h.eat(*a);
		ਨਹੀਂਤਾਂ
			h.eat(0);
		ਵਾਪਸ h;
	}
};

ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashtable_ਮਾਪ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ min_ਮਾਪ)
{
	// Primes as generated by https://oeis.org/A175953
	ਸਥਿਰ std::ਸਦਿਸ਼<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ> zero_and_some_ਅਭਾਜ_ਸੰਖਿਆਵਾਂ = {
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

	ਲਈ (ਸਵੈਚਾਲਿਤ p : zero_and_some_ਅਭਾਜ_ਸੰਖਿਆਵਾਂ)
		ਜੇ (p >= min_ਮਾਪ) ਵਾਪਸ p;

	ਜੇ (ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) == 4)
		throw std::ਲੰਮਾਈ_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	ਲਈ (ਸਵੈਚਾਲਿਤ p : zero_and_some_ਅਭਾਜ_ਸੰਖਿਆਵਾਂ)
		ਜੇ (100129 * p > min_ਮਾਪ) ਵਾਪਸ 100129 * p;

	throw std::ਲੰਮਾਈ_error("hash table exceeded maximum size.");
}

ਸਾਂਚਾ<typename K, typename T, typename OPS = hash_ops<K>> class dict;
ਸਾਂਚਾ<typename K, ਪੂਰਨ ਵਿੱਥ = 0, typename OPS = hash_ops<K>> class idict;
ਸਾਂਚਾ<typename K, typename OPS = hash_ops<K>> class pool;
ਸਾਂਚਾ<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
ਜਨਤਕ:
	commutative_hash() {
		buckets.fill(0);
	}
	ਸਾਂਚਾ <typename T>
	ਖਾਲੀ eat(ਅਚਲ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	ਖਾਲੀ eat(ਅਚਲ Hasher &h) {
		Hasher::hash_t v = h.yield();
		ਮਾਪ_t ਸੂਚਕਾਂਕ = v & (buckets.ਮਾਪ() - 1);
		buckets[ਸੂਚਕਾਂਕ] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		ਲਈ (ਸਵੈਚਾਲਿਤ b : buckets)
			h.eat(b);
		ਵਾਪਸ h;
	}
ਨਿਜੀ:
	std::array<Hasher::hash_t, 4> buckets;
};

ਸਾਂਚਾ<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ dict {
	ਬਣਤਰ ਇੰਦਰਾਜ_t
	{
		std::pair<K, T> udata;
		ਪੂਰਨ ਅਗਲਾ;

		ਇੰਦਰਾਜ_t() { }
		ਇੰਦਰਾਜ_t(ਅਚਲ std::pair<K, T> &udata, ਪੂਰਨ ਅਗਲਾ) : udata(udata), ਅਗਲਾ(ਅਗਲਾ) { }
		ਇੰਦਰਾਜ_t(std::pair<K, T> &&udata, ਪੂਰਨ ਅਗਲਾ) : udata(std::move(udata)), ਅਗਲਾ(ਅਗਲਾ) { }
		bool ਸੰਚਾਲਕ<(ਅਚਲ ਇੰਦਰਾਜ_t &other) ਅਚਲ { ਵਾਪਸ udata.ਪਹਿਲਾ < other.udata.ਪਹਿਲਾ; }
	};

	std::ਸਦਿਸ਼<ਪੂਰਨ> hashtable;
	std::ਸਦਿਸ਼<ਇੰਦਰਾਜ_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NDEBUG
	ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ do_assert(bool) { }
#ਨਹੀਂਤਾਂ
	ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ do_assert(bool cond) {
		ਜੇ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#ਜੇ_ਅੰਤ

	Hasher::hash_t do_hash(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = 0;
		ਜੇ (!hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			hash = ops.hash(ਕੁੰਜੀ).yield() % (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(hashtable.ਮਾਪ());
		ਵਾਪਸ hash;
	}

	ਖਾਲੀ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ਮਾਪ(entries.ਸਮਰੱਥਾ() * hashtable_ਮਾਪ_ਗੁਣਕ), -1);

		ਲਈ (ਪੂਰਨ i = 0; i < ਪੂਰਨ(entries.ਮਾਪ()); i++) {
			do_assert(-1 <= entries[i].ਅਗਲਾ && entries[i].ਅਗਲਾ < ਪੂਰਨ(entries.ਮਾਪ()));
			Hasher::hash_t hash = do_hash(entries[i].udata.ਪਹਿਲਾ);
			entries[i].ਅਗਲਾ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ਪੂਰਨ do_erase(ਪੂਰਨ ਸੂਚਕਾਂਕ, Hasher::hash_t hash)
	{
		do_assert(ਸੂਚਕਾਂਕ < ਪੂਰਨ(entries.ਮਾਪ()));
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ() || ਸੂਚਕਾਂਕ < 0)
			ਵਾਪਸ 0;

		ਪੂਰਨ k = hashtable[hash];
		do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));

		ਜੇ (k == ਸੂਚਕਾਂਕ) {
			hashtable[hash] = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
		} ਨਹੀਂਤਾਂ {
			ਜਦੋਂ (entries[k].ਅਗਲਾ != ਸੂਚਕਾਂਕ) {
				k = entries[k].ਅਗਲਾ;
				do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));
			}
			entries[k].ਅਗਲਾ = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
		}

		ਪੂਰਨ ਪਿੱਠ_idx = entries.ਮਾਪ()-1;

		ਜੇ (ਸੂਚਕਾਂਕ != ਪਿੱਠ_idx)
		{
			Hasher::hash_t ਪਿੱਠ_hash = do_hash(entries[ਪਿੱਠ_idx].udata.ਪਹਿਲਾ);

			k = hashtable[ਪਿੱਠ_hash];
			do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));

			ਜੇ (k == ਪਿੱਠ_idx) {
				hashtable[ਪਿੱਠ_hash] = ਸੂਚਕਾਂਕ;
			} ਨਹੀਂਤਾਂ {
				ਜਦੋਂ (entries[k].ਅਗਲਾ != ਪਿੱਠ_idx) {
					k = entries[k].ਅਗਲਾ;
					do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));
				}
				entries[k].ਅਗਲਾ = ਸੂਚਕਾਂਕ;
			}

			entries[ਸੂਚਕਾਂਕ] = std::move(entries[ਪਿੱਠ_idx]);
		}

		entries.ਕੱਢੋ_ਪਿੱਠ();

		ਜੇ (entries.ਖਾਲੀ_ਥਾਵਾਂ())
			hashtable.clear();

		ਵਾਪਸ 1;
	}

	ਪੂਰਨ do_lookup(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ -1;

		ਜੇ (entries.ਮਾਪ() * hashtable_ਮਾਪ_trigger > hashtable.ਮਾਪ()) {
			do_rehash();
			hash = do_hash(ਕੁੰਜੀ);
		}

		ਵਾਪਸ do_lookup_ਅੰਦਰੂਨੀ(ਕੁੰਜੀ, hash);
	}

	ਪੂਰਨ do_lookup_ਅੰਦਰੂਨੀ(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t hash) ਅਚਲ
	{
		ਪੂਰਨ ਸੂਚਕਾਂਕ = hashtable[hash];

		ਜਦੋਂ (ਸੂਚਕਾਂਕ >= 0 && !ops.cmp(entries[ਸੂਚਕਾਂਕ].udata.ਪਹਿਲਾ, ਕੁੰਜੀ)) {
			ਸੂਚਕਾਂਕ = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
			do_assert(-1 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < ਪੂਰਨ(entries.ਮਾਪ()));
		}

		ਵਾਪਸ ਸੂਚਕਾਂਕ;
	}

	ਪੂਰਨ do_lookup_no_rehash(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t hash) ਅਚਲ
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ -1;

		ਵਾਪਸ do_lookup_ਅੰਦਰੂਨੀ(ਕੁੰਜੀ, hash);
	}

	ਪੂਰਨ do_ਪਾਓ(ਅਚਲ K &ਕੁੰਜੀ, ਅਚਲ Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ()) {
			entries.emplace_ਪਿੱਠ(std::pair<K, T>(ਕੁੰਜੀ, T()), -1);
			do_rehash();
		} ਨਹੀਂਤਾਂ {
			entries.emplace_ਪਿੱਠ(std::pair<K, T>(ਕੁੰਜੀ, T()), hashtable[hash]);
			hashtable[hash] = entries.ਮਾਪ() - 1;
		}
		ਵਾਪਸ entries.ਮਾਪ() - 1;
	}

	ਪੂਰਨ do_ਪਾਓ(ਅਚਲ std::pair<K, T> &ਮੁੱਲ, ਅਚਲ Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ()) {
			entries.emplace_ਪਿੱਠ(ਮੁੱਲ, -1);
			do_rehash();
		} ਨਹੀਂਤਾਂ {
			entries.emplace_ਪਿੱਠ(ਮੁੱਲ, hashtable[hash]);
			hashtable[hash] = entries.ਮਾਪ() - 1;
		}
		ਵਾਪਸ entries.ਮਾਪ() - 1;
	}

	ਪੂਰਨ do_ਪਾਓ(std::pair<K, T> &&rvalue, ਅਚਲ Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ()) {
			entries.emplace_ਪਿੱਠ(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} ਨਹੀਂਤਾਂ {
			entries.emplace_ਪਿੱਠ(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ਮਾਪ() - 1;
		}
		ਵਾਪਸ entries.ਮਾਪ() - 1;
	}

ਜਨਤਕ:
	class const_iterator
	{
		ਮਿੱਤਰ class dict;
	ਸੁਰੱਖਿਅਤ:
		ਅਚਲ dict *ptr;
		ਪੂਰਨ ਸੂਚਕਾਂਕ;
		const_iterator(ਅਚਲ dict *ptr, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ptr(ptr), ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) { }
	ਜਨਤਕ:
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::bidirectional_iterator_ਟੈਗ iterator_category;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<K, T> ਮੁੱਲ_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ std::pair<K, T>* ਸੂਚਕ;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ--; ਵਾਪਸ *this; }
		const_iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) { const_iterator tmp = *this; ਸੂਚਕਾਂਕ--; ਵਾਪਸ tmp; }
		const_iterator ਸੰਚਾਲਕ--() { ਸੂਚਕਾਂਕ++; ਵਾਪਸ *this; }
		const_iterator ਸੰਚਾਲਕ--(ਪੂਰਨ) { const_iterator tmp = *this; ਸੂਚਕਾਂਕ++; ਵਾਪਸ tmp; }
		const_iterator ਸੰਚਾਲਕ+=(ਪੂਰਨ amt) { ਸੂਚਕਾਂਕ -= amt; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ<(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ > other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ==(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
		ਅਚਲ std::pair<K, T> &ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ std::pair<K, T> *ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
	};

	class iterator
	{
		ਮਿੱਤਰ class dict;
	ਸੁਰੱਖਿਅਤ:
		dict *ptr;
		ਪੂਰਨ ਸੂਚਕਾਂਕ;
		iterator(dict *ptr, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ptr(ptr), ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) { }
	ਜਨਤਕ:
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::forward_iterator_ਟੈਗ iterator_category;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<K, T> ਮੁੱਲ_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<K, T>* ਸੂਚਕ;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<K, T>& reference;
		iterator() { }
		iterator ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ--; ਵਾਪਸ *this; }
		iterator ਸੰਚਾਲਕ+=(ਪੂਰਨ amt) { ਸੂਚਕਾਂਕ -= amt; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ<(ਅਚਲ iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ > other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ==(ਅਚਲ iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
		std::pair<K, T> &ਸੰਚਾਲਕ*() { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		std::pair<K, T> *ਸੰਚਾਲਕ->() { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ std::pair<K, T> &ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ std::pair<K, T> *ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਸੰਚਾਲਕ const_iterator() ਅਚਲ { ਵਾਪਸ const_iterator(ptr, ਸੂਚਕਾਂਕ); }
	};
	using ਉਲਟ_iterator = std::ਉਲਟ_iterator<const_iterator>;
	ਉਲਟ_iterator rbegin() ਅਚਲ {
		ਵਾਪਸ std::make_ਉਲਟ_iterator(ਸਿਰਾ());
	}
	ਉਲਟ_iterator rend() ਅਚਲ {
		ਵਾਪਸ std::make_ਉਲਟ_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ਅਚਲ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		ਅਦਲਾ_ਬਦਲੀ(other);
	}

	dict &ਸੰਚਾਲਕ=(ਅਚਲ dict &other) {
		entries = other.entries;
		do_rehash();
		ਵਾਪਸ *this;
	}

	dict &ਸੰਚਾਲਕ=(dict &&other) noexcept {
		clear();
		ਅਦਲਾ_ਬਦਲੀ(other);
		ਵਾਪਸ *this;
	}

	dict(ਅਚਲ std::initializer_list<std::pair<K, T>> &list)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : list)
			ਪਾਓ(it);
	}

	ਸਾਂਚਾ<class ਨਿਵੇਸ਼_Iterator>
	dict(ਨਿਵੇਸ਼_Iterator ਪਹਿਲਾ, ਨਿਵੇਸ਼_Iterator ਆਖਰੀ)
	{
		ਪਾਓ(ਪਹਿਲਾ, ਆਖਰੀ);
	}

	ਸਾਂਚਾ<class ਨਿਵੇਸ਼_Iterator>
	ਖਾਲੀ ਪਾਓ(ਨਿਵੇਸ਼_Iterator ਪਹਿਲਾ, ਨਿਵੇਸ਼_Iterator ਆਖਰੀ)
	{
		ਲਈ (; ਪਹਿਲਾ != ਆਖਰੀ; ++ਪਹਿਲਾ)
			ਪਾਓ(*ਪਹਿਲਾ);
	}

	std::pair<iterator, bool> ਪਾਓ(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(ਕੁੰਜੀ, hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ਪਾਓ(ਅਚਲ std::pair<K, T> &ਮੁੱਲ)
	{
		Hasher::hash_t hash = do_hash(ਮੁੱਲ.ਪਹਿਲਾ);
		ਪੂਰਨ i = do_lookup(ਮੁੱਲ.ਪਹਿਲਾ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(ਮੁੱਲ, hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ਪਾਓ(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.ਪਹਿਲਾ);
		ਪੂਰਨ i = do_lookup(rvalue.ਪਹਿਲਾ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::forward<std::pair<K, T>>(rvalue), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ਅਚਲ &ਕੁੰਜੀ, T ਅਚਲ &ਮੁੱਲ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::make_pair(ਕੁੰਜੀ, ਮੁੱਲ), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ਅਚਲ &ਕੁੰਜੀ, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::make_pair(ਕੁੰਜੀ, std::forward<T>(rvalue)), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ਅਚਲ &ਮੁੱਲ)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ਪੂਰਨ i = do_lookup(rkey, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::make_pair(std::forward<K>(rkey), ਮੁੱਲ), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ਪੂਰਨ i = do_lookup(rkey, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ਪੂਰਨ erase(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ ਸੂਚਕਾਂਕ = do_lookup(ਕੁੰਜੀ, hash);
		ਵਾਪਸ do_erase(ਸੂਚਕਾਂਕ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->ਪਹਿਲਾ);
		do_erase(it.ਸੂਚਕਾਂਕ, hash);
		ਵਾਪਸ ++it;
	}

	ਪੂਰਨ ਗਿਣਤੀ_2(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i < 0 ? 0 : 1;
	}

	ਪੂਰਨ ਗਿਣਤੀ_2(ਅਚਲ K &ਕੁੰਜੀ, const_iterator it) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i < 0 || i > it.ਸੂਚਕਾਂਕ ? 0 : 1;
	}

	iterator ਲੱਭੋ(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ ਸਿਰਾ();
		ਵਾਪਸ iterator(this, i);
	}

	const_iterator ਲੱਭੋ(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ ਸਿਰਾ();
		ਵਾਪਸ const_iterator(this, i);
	}

	T& at(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			throw std::ਬਾਹਰਲਾ_ਦਾ_range("dict::at()");
		ਵਾਪਸ entries[i].udata.ਦੂਜਾ;
	}

	ਅਚਲ T& at(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			throw std::ਬਾਹਰਲਾ_ਦਾ_range("dict::at()");
		ਵਾਪਸ entries[i].udata.ਦੂਜਾ;
	}

	ਅਚਲ T& at(ਅਚਲ K &ਕੁੰਜੀ, ਅਚਲ T &defval) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ defval;
		ਵਾਪਸ entries[i].udata.ਦੂਜਾ;
	}

	T& ਸੰਚਾਲਕ[](ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			i = do_ਪਾਓ(std::pair<K, T>(ਕੁੰਜੀ, T()), hash);
		ਵਾਪਸ entries[i].udata.ਦੂਜਾ;
	}

	ਸਾਂਚਾ<typename Compare = std::less<K>>
	ਖਾਲੀ ਛਾਂਟੋ(Compare comp = Compare())
	{
		std::ਛਾਂਟੋ(entries.begin(), entries.ਸਿਰਾ(), [comp](ਅਚਲ ਇੰਦਰਾਜ_t &a, ਅਚਲ ਇੰਦਰਾਜ_t &b){ ਵਾਪਸ comp(b.udata.ਪਹਿਲਾ, a.udata.ਪਹਿਲਾ); });
		do_rehash();
	}

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(dict &other)
	{
		hashtable.ਅਦਲਾ_ਬਦਲੀ(other.hashtable);
		entries.ਅਦਲਾ_ਬਦਲੀ(other.entries);
	}

	bool ਸੰਚਾਲਕ==(ਅਚਲ dict &other) ਅਚਲ {
		ਜੇ (ਮਾਪ() != other.ਮਾਪ())
			ਵਾਪਸ false;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : entries) {
			ਸਵੈਚਾਲਿਤ oit = other.ਲੱਭੋ(it.udata.ਪਹਿਲਾ);
			ਜੇ (oit == other.ਸਿਰਾ() || !(oit->ਦੂਜਾ == it.udata.ਦੂਜਾ))
				ਵਾਪਸ false;
		}
		ਵਾਪਸ true;
	}

	bool ਸੰਚਾਲਕ!=(ਅਚਲ dict &other) ਅਚਲ {
		ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		commutative_hash comm;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : entries) {
			Hasher ਇੰਦਰਾਜ_hash;
			ਇੰਦਰਾਜ_hash.eat(it.udata.ਪਹਿਲਾ);
			ਇੰਦਰਾਜ_hash.eat(it.udata.ਦੂਜਾ);
			comm.eat(ਇੰਦਰਾਜ_hash);
		}
		ਵਾਪਸ comm.hash_into(h);
	}

	ਖਾਲੀ reserve(ਮਾਪ_t n) { entries.reserve(n); }
	ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ entries.ਮਾਪ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ entries.ਖਾਲੀ_ਥਾਵਾਂ(); }
	ਖਾਲੀ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ਵਾਪਸ iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1); }
	iterator element(ਪੂਰਨ n) { ਵਾਪਸ iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1-n); }
	iterator ਸਿਰਾ() { ਵਾਪਸ iterator(nullptr, -1); }

	const_iterator begin() ਅਚਲ { ਵਾਪਸ const_iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1); }
	const_iterator element(ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ const_iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1-n); }
	const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ const_iterator(this, -1); }
};

ਸਾਂਚਾ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ pool
{
	ਸਾਂਚਾ<typename, ਪੂਰਨ, typename> ਮਿੱਤਰ class idict;

ਸੁਰੱਖਿਅਤ:
	ਬਣਤਰ ਇੰਦਰਾਜ_t
	{
		K udata;
		ਪੂਰਨ ਅਗਲਾ;

		ਇੰਦਰਾਜ_t() { }
		ਇੰਦਰਾਜ_t(ਅਚਲ K &udata, ਪੂਰਨ ਅਗਲਾ) : udata(udata), ਅਗਲਾ(ਅਗਲਾ) { }
		ਇੰਦਰਾਜ_t(K &&udata, ਪੂਰਨ ਅਗਲਾ) : udata(std::move(udata)), ਅਗਲਾ(ਅਗਲਾ) { }
	};

	std::ਸਦਿਸ਼<ਪੂਰਨ> hashtable;
	std::ਸਦਿਸ਼<ਇੰਦਰਾਜ_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NDEBUG
	ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ do_assert(bool) { }
#ਨਹੀਂਤਾਂ
	ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ do_assert(bool cond) {
		ਜੇ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#ਜੇ_ਅੰਤ

	Hasher::hash_t do_hash(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = 0;
		ਜੇ (!hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			hash = ops.hash(ਕੁੰਜੀ).yield() % (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(hashtable.ਮਾਪ());
		ਵਾਪਸ hash;
	}

	ਖਾਲੀ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ਮਾਪ(entries.ਸਮਰੱਥਾ() * hashtable_ਮਾਪ_ਗੁਣਕ), -1);

		ਲਈ (ਪੂਰਨ i = 0; i < ਪੂਰਨ(entries.ਮਾਪ()); i++) {
			do_assert(-1 <= entries[i].ਅਗਲਾ && entries[i].ਅਗਲਾ < ਪੂਰਨ(entries.ਮਾਪ()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].ਅਗਲਾ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ਪੂਰਨ do_erase(ਪੂਰਨ ਸੂਚਕਾਂਕ, Hasher::hash_t hash)
	{
		do_assert(ਸੂਚਕਾਂਕ < ਪੂਰਨ(entries.ਮਾਪ()));
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ() || ਸੂਚਕਾਂਕ < 0)
			ਵਾਪਸ 0;

		ਪੂਰਨ k = hashtable[hash];
		ਜੇ (k == ਸੂਚਕਾਂਕ) {
			hashtable[hash] = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
		} ਨਹੀਂਤਾਂ {
			ਜਦੋਂ (entries[k].ਅਗਲਾ != ਸੂਚਕਾਂਕ) {
				k = entries[k].ਅਗਲਾ;
				do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));
			}
			entries[k].ਅਗਲਾ = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
		}

		ਪੂਰਨ ਪਿੱਠ_idx = entries.ਮਾਪ()-1;

		ਜੇ (ਸੂਚਕਾਂਕ != ਪਿੱਠ_idx)
		{
			Hasher::hash_t ਪਿੱਠ_hash = do_hash(entries[ਪਿੱਠ_idx].udata);

			k = hashtable[ਪਿੱਠ_hash];
			ਜੇ (k == ਪਿੱਠ_idx) {
				hashtable[ਪਿੱਠ_hash] = ਸੂਚਕਾਂਕ;
			} ਨਹੀਂਤਾਂ {
				ਜਦੋਂ (entries[k].ਅਗਲਾ != ਪਿੱਠ_idx) {
					k = entries[k].ਅਗਲਾ;
					do_assert(0 <= k && k < ਪੂਰਨ(entries.ਮਾਪ()));
				}
				entries[k].ਅਗਲਾ = ਸੂਚਕਾਂਕ;
			}

			entries[ਸੂਚਕਾਂਕ] = std::move(entries[ਪਿੱਠ_idx]);
		}

		entries.ਕੱਢੋ_ਪਿੱਠ();

		ਜੇ (entries.ਖਾਲੀ_ਥਾਵਾਂ())
			hashtable.clear();

		ਵਾਪਸ 1;
	}

	ਪੂਰਨ do_lookup(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ -1;

		ਜੇ (entries.ਮਾਪ() * hashtable_ਮਾਪ_trigger > hashtable.ਮਾਪ()) {
			do_rehash();
			hash = do_hash(ਕੁੰਜੀ);
		}

		ਵਾਪਸ do_lookup_ਅੰਦਰੂਨੀ(ਕੁੰਜੀ, hash);
	}

	ਪੂਰਨ do_lookup_ਅੰਦਰੂਨੀ(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t hash) ਅਚਲ
	{
		ਪੂਰਨ ਸੂਚਕਾਂਕ = hashtable[hash];

		ਜਦੋਂ (ਸੂਚਕਾਂਕ >= 0 && !ops.cmp(entries[ਸੂਚਕਾਂਕ].udata, ਕੁੰਜੀ)) {
			ਸੂਚਕਾਂਕ = entries[ਸੂਚਕਾਂਕ].ਅਗਲਾ;
			do_assert(-1 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < ਪੂਰਨ(entries.ਮਾਪ()));
		}

		ਵਾਪਸ ਸੂਚਕਾਂਕ;
	}

	ਪੂਰਨ do_lookup_no_rehash(ਅਚਲ K &ਕੁੰਜੀ, Hasher::hash_t hash) ਅਚਲ
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ -1;

		ਵਾਪਸ do_lookup_ਅੰਦਰੂਨੀ(ਕੁੰਜੀ, hash);
	}

	ਪੂਰਨ do_ਪਾਓ(ਅਚਲ K &ਮੁੱਲ, Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ()) {
			entries.emplace_ਪਿੱਠ(ਮੁੱਲ, -1);
			do_rehash();
			hash = do_hash(ਮੁੱਲ);
		} ਨਹੀਂਤਾਂ {
			entries.emplace_ਪਿੱਠ(ਮੁੱਲ, hashtable[hash]);
			hashtable[hash] = entries.ਮਾਪ() - 1;
		}
		ਵਾਪਸ entries.ਮਾਪ() - 1;
	}

	ਪੂਰਨ do_ਪਾਓ(K &&rvalue, Hasher::hash_t &hash)
	{
		ਜੇ (hashtable.ਖਾਲੀ_ਥਾਵਾਂ()) {
			entries.emplace_ਪਿੱਠ(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.ਪਿੱਠ().udata);
		} ਨਹੀਂਤਾਂ {
			entries.emplace_ਪਿੱਠ(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ਮਾਪ() - 1;
		}
		ਵਾਪਸ entries.ਮਾਪ() - 1;
	}

ਜਨਤਕ:
	class const_iterator
	{
		ਮਿੱਤਰ class pool;
	ਸੁਰੱਖਿਅਤ:
		ਅਚਲ pool *ptr;
		ਪੂਰਨ ਸੂਚਕਾਂਕ;
		const_iterator(ਅਚਲ pool *ptr, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ptr(ptr), ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) { }
	ਜਨਤਕ:
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::forward_iterator_ਟੈਗ iterator_category;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K ਮੁੱਲ_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K* ਸੂਚਕ;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K& reference;
		const_iterator() { }
		const_iterator ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ--; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ==(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
		ਅਚਲ K &ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ K *ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
	};

	class iterator
	{
		ਮਿੱਤਰ class pool;
	ਸੁਰੱਖਿਅਤ:
		pool *ptr;
		ਪੂਰਨ ਸੂਚਕਾਂਕ;
		iterator(pool *ptr, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ptr(ptr), ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) { }
	ਜਨਤਕ:
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::forward_iterator_ਟੈਗ iterator_category;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K ਮੁੱਲ_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K* ਸੂਚਕ;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K& reference;
		iterator() { }
		iterator ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ--; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ==(ਅਚਲ iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
		K &ਸੰਚਾਲਕ*() { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		K *ਸੰਚਾਲਕ->() { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ K &ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਅਚਲ K *ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ &ptr->entries[ਸੂਚਕਾਂਕ].udata; }
		ਸੰਚਾਲਕ const_iterator() ਅਚਲ { ਵਾਪਸ const_iterator(ptr, ਸੂਚਕਾਂਕ); }
	};

	constexpr pool()
	{
	}

	pool(ਅਚਲ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		ਅਦਲਾ_ਬਦਲੀ(other);
	}

	pool &ਸੰਚਾਲਕ=(ਅਚਲ pool &other) {
		entries = other.entries;
		do_rehash();
		ਵਾਪਸ *this;
	}

	pool &ਸੰਚਾਲਕ=(pool &&other) noexcept {
		clear();
		ਅਦਲਾ_ਬਦਲੀ(other);
		ਵਾਪਸ *this;
	}

	pool(ਅਚਲ std::initializer_list<K> &list)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : list)
			ਪਾਓ(it);
	}

	ਸਾਂਚਾ<class ਨਿਵੇਸ਼_Iterator>
	pool(ਨਿਵੇਸ਼_Iterator ਪਹਿਲਾ, ਨਿਵੇਸ਼_Iterator ਆਖਰੀ)
	{
		ਪਾਓ(ਪਹਿਲਾ, ਆਖਰੀ);
	}

	ਸਾਂਚਾ<class ਨਿਵੇਸ਼_Iterator>
	ਖਾਲੀ ਪਾਓ(ਨਿਵੇਸ਼_Iterator ਪਹਿਲਾ, ਨਿਵੇਸ਼_Iterator ਆਖਰੀ)
	{
		ਲਈ (; ਪਹਿਲਾ != ਆਖਰੀ; ++ਪਹਿਲਾ)
			ਪਾਓ(*ਪਹਿਲਾ);
	}

	std::pair<iterator, bool> ਪਾਓ(ਅਚਲ K &ਮੁੱਲ)
	{
		Hasher::hash_t hash = do_hash(ਮੁੱਲ);
		ਪੂਰਨ i = do_lookup(ਮੁੱਲ, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(ਮੁੱਲ, hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ਪਾਓ(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		ਪੂਰਨ i = do_lookup(rvalue, hash);
		ਜੇ (i >= 0)
			ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ਪਾਓ(std::forward<K>(rvalue), hash);
		ਵਾਪਸ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ਸਾਂਚਾ<typename... ਦਲੀਲਾਂ_3>
	std::pair<iterator, bool> emplace(ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2)
	{
		ਵਾਪਸ ਪਾਓ(K(std::forward<ਦਲੀਲਾਂ_3>(ਦਲੀਲਾਂ_2)...));
	}

	ਪੂਰਨ erase(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ ਸੂਚਕਾਂਕ = do_lookup(ਕੁੰਜੀ, hash);
		ਵਾਪਸ do_erase(ਸੂਚਕਾਂਕ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.ਸੂਚਕਾਂਕ, hash);
		ਵਾਪਸ ++it;
	}

	ਪੂਰਨ ਗਿਣਤੀ_2(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i < 0 ? 0 : 1;
	}

	ਪੂਰਨ ਗਿਣਤੀ_2(ਅਚਲ K &ਕੁੰਜੀ, const_iterator it) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i < 0 || i > it.ਸੂਚਕਾਂਕ ? 0 : 1;
	}

	iterator ਲੱਭੋ(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ ਸਿਰਾ();
		ਵਾਪਸ iterator(this, i);
	}

	const_iterator ਲੱਭੋ(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ ਸਿਰਾ();
		ਵਾਪਸ const_iterator(this, i);
	}

	bool ਸੰਚਾਲਕ[](ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = do_lookup(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i >= 0;
	}

	ਸਾਂਚਾ<typename Compare = std::less<K>>
	ਖਾਲੀ ਛਾਂਟੋ(Compare comp = Compare())
	{
		std::ਛਾਂਟੋ(entries.begin(), entries.ਸਿਰਾ(), [comp](ਅਚਲ ਇੰਦਰਾਜ_t &a, ਅਚਲ ਇੰਦਰਾਜ_t &b){ ਵਾਪਸ comp(b.udata, a.udata); });
		do_rehash();
	}

	K ਕੱਢੋ()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ਵਾਪਸ ret;
	}

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(pool &other)
	{
		hashtable.ਅਦਲਾ_ਬਦਲੀ(other.hashtable);
		entries.ਅਦਲਾ_ਬਦਲੀ(other.entries);
	}

	bool ਸੰਚਾਲਕ==(ਅਚਲ pool &other) ਅਚਲ {
		ਜੇ (ਮਾਪ() != other.ਮਾਪ())
			ਵਾਪਸ false;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : entries)
			ਜੇ (!other.ਗਿਣਤੀ_2(it.udata))
				ਵਾਪਸ false;
		ਵਾਪਸ true;
	}

	bool ਸੰਚਾਲਕ!=(ਅਚਲ pool &other) ਅਚਲ {
		ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		commutative_hash comm;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ਵਾਪਸ comm.hash_into(h);
	}

	ਖਾਲੀ reserve(ਮਾਪ_t n) { entries.reserve(n); }
	ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ entries.ਮਾਪ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ entries.ਖਾਲੀ_ਥਾਵਾਂ(); }
	ਖਾਲੀ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ਵਾਪਸ iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1); }
	iterator element(ਪੂਰਨ n) { ਵਾਪਸ iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1-n); }
	iterator ਸਿਰਾ() { ਵਾਪਸ iterator(nullptr, -1); }

	const_iterator begin() ਅਚਲ { ਵਾਪਸ const_iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1); }
	const_iterator element(ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ const_iterator(this, ਪੂਰਨ(entries.ਮਾਪ())-1-n); }
	const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ const_iterator(nullptr, -1); }
};

ਸਾਂਚਾ<typename K, ਪੂਰਨ ਵਿੱਥ, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ idict
{
	pool<K, OPS> database;

ਜਨਤਕ:
	class const_iterator
	{
		ਮਿੱਤਰ class idict;
	ਸੁਰੱਖਿਅਤ:
		ਅਚਲ idict &container;
		ਪੂਰਨ ਸੂਚਕਾਂਕ;
		const_iterator(ਅਚਲ idict &container, ਪੂਰਨ ਸੂਚਕਾਂਕ) : container(container), ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ) { }
	ਜਨਤਕ:
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::forward_iterator_ਟੈਗ iterator_category;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K ਮੁੱਲ_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K* ਸੂਚਕ;
		ਕਿਸਮਪਰਿਭਾਸ਼ਾ K& reference;
		const_iterator() { }
		const_iterator ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ++; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ==(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ const_iterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
		ਅਚਲ K &ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ container[ਸੂਚਕਾਂਕ]; }
		ਅਚਲ K *ਸੰਚਾਲਕ->() ਅਚਲ { ਵਾਪਸ &container[ਸੂਚਕਾਂਕ]; }
	};

	constexpr idict()
	{
	}

	ਪੂਰਨ ਸੰਚਾਲਕ()(ਅਚਲ K &ਕੁੰਜੀ)
	{
		Hasher::hash_t hash = database.do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = database.do_lookup(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			i = database.do_ਪਾਓ(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i + ਵਿੱਥ;
	}

	ਪੂਰਨ at(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = database.do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = database.do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			throw std::ਬਾਹਰਲਾ_ਦਾ_range("idict::at()");
		ਵਾਪਸ i + ਵਿੱਥ;
	}

	ਪੂਰਨ at(ਅਚਲ K &ਕੁੰਜੀ, ਪੂਰਨ defval) ਅਚਲ
	{
		Hasher::hash_t hash = database.do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = database.do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਜੇ (i < 0)
			ਵਾਪਸ defval;
		ਵਾਪਸ i + ਵਿੱਥ;
	}

	ਪੂਰਨ ਗਿਣਤੀ_2(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = database.do_hash(ਕੁੰਜੀ);
		ਪੂਰਨ i = database.do_lookup_no_rehash(ਕੁੰਜੀ, hash);
		ਵਾਪਸ i < 0 ? 0 : 1;
	}

	ਪੂਰਨ lookup(ਅਚਲ K &ਕੁੰਜੀ) ਅਚਲ
	{
		Hasher::hash_t hash = database.do_hash(ਕੁੰਜੀ);
		ਵਾਪਸ database.do_lookup_no_rehash(ਕੁੰਜੀ, hash);
	}

	ਖਾਲੀ expect(ਅਚਲ K &ਕੁੰਜੀ, ਪੂਰਨ i)
	{
		ਪੂਰਨ j = (*this)(ਕੁੰਜੀ);
		ਜੇ (i != j)
			throw std::ਬਾਹਰਲਾ_ਦਾ_range("idict::expect()");
	}

	ਅਚਲ K &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ
	{
		ਵਾਪਸ database.entries.at(ਸੂਚਕਾਂਕ - ਵਿੱਥ).udata;
	}

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(idict &other)
	{
		database.ਅਦਲਾ_ਬਦਲੀ(other.database);
	}

	ਖਾਲੀ reserve(ਮਾਪ_t n) { database.reserve(n); }
	ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ database.ਮਾਪ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ database.ਖਾਲੀ_ਥਾਵਾਂ(); }
	ਖਾਲੀ clear() { database.clear(); }

	const_iterator begin() ਅਚਲ { ਵਾਪਸ const_iterator(*this, ਵਿੱਥ); }
	const_iterator element(ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ const_iterator(*this, n); }
	const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ const_iterator(*this, ਵਿੱਥ + ਮਾਪ()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
ਸਾਂਚਾ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ਅਣਵਰਤਿਆ mfp
{
	idict<K, 0, OPS> database;
	class Atomic_ਮਾਪਾ {
	ਜਨਤਕ:
		explicit Atomic_ਮਾਪਾ(ਪੂਰਨ p) : ਮਾਪਾ(p) {}
		Atomic_ਮਾਪਾ(ਅਚਲ Atomic_ਮਾਪਾ &other) : ਮਾਪਾ(other.get()) {}
		Atomic_ਮਾਪਾ &ਸੰਚਾਲਕ=(ਅਚਲ Atomic_ਮਾਪਾ &other) { ਸੈੱਟ(other.get()); ਵਾਪਸ *this; }
		ਪੂਰਨ get() ਅਚਲ { ਵਾਪਸ ਮਾਪਾ.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed); }
		ਖਾਲੀ ਸੈੱਟ(ਪੂਰਨ p) { ਮਾਪਾ.store(p, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed); }
	ਨਿਜੀ:
		std::atomic<ਪੂਰਨ> ਮਾਪਾ;
	};
	std::ਸਦਿਸ਼<Atomic_ਮਾਪਾ> parents;

ਜਨਤਕ:
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	ਪੂਰਨ ਸੰਚਾਲਕ()(ਅਚਲ K &ਕੁੰਜੀ)
	{
		ਪੂਰਨ i = database(ਕੁੰਜੀ);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		ਜੇ (parents.ਮਾਪ() < database.ਮਾਪ()) {
			parents.emplace_ਪਿੱਠ(-1);
		}
		ਵਾਪਸ i;
	}

	// Finds an element at given index
	ਅਚਲ K &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ
	{
		ਵਾਪਸ database[ਸੂਚਕਾਂਕ];
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
	ਪੂਰਨ ifind(ਪੂਰਨ i) ਅਚਲ
	{
		ਪੂਰਨ p = i, k = i;

		ਜਦੋਂ (true) {
			ਪੂਰਨ pp = parents[p].get();
			ਜੇ (pp < 0)
				ਰੋਕੋ;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		ਜਦੋਂ (k != p) {
			ਪੂਰਨ ਅਗਲਾ_k = parents[k].get();
			const_cast<Atomic_ਮਾਪਾ*>(&parents[k])->ਸੈੱਟ(p);
			k = ਅਗਲਾ_k;
		}

		ਵਾਪਸ p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	ਖਾਲੀ imerge(ਪੂਰਨ i, ਪੂਰਨ j)
	{
		i = ifind(i);
		j = ifind(j);

		ਜੇ (i != j)
			parents[i].ਸੈੱਟ(j);
	}

	ਖਾਲੀ ipromote(ਪੂਰਨ i)
	{
		ਪੂਰਨ k = i;

		ਜਦੋਂ (k != -1) {
			ਪੂਰਨ ਅਗਲਾ_k = parents[k].get();
			parents[k].ਸੈੱਟ(i);
			k = ਅਗਲਾ_k;
		}

		parents[i].ਸੈੱਟ(-1);
	}

	ਪੂਰਨ lookup(ਅਚਲ K &a)
	{
		ਵਾਪਸ ifind((*this)(a));
	}

	ਅਚਲ K &ਲੱਭੋ(ਅਚਲ K &a) ਅਚਲ
	{
		ਪੂਰਨ i = database.at(a, -1);
		ਜੇ (i < 0)
			ਵਾਪਸ a;
		ਵਾਪਸ (*this)[ifind(i)];
	}

	ਖਾਲੀ ਮਿਲਾਓ(ਅਚਲ K &a, ਅਚਲ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	ਖਾਲੀ promote(ਅਚਲ K &a)
	{
		ਪੂਰਨ i = database.at(a, -1);
		ਜੇ (i >= 0)
			ipromote(i);
	}

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(mfp &other)
	{
		database.ਅਦਲਾ_ਬਦਲੀ(other.database);
		parents.ਅਦਲਾ_ਬਦਲੀ(other.parents);
	}

	ਖਾਲੀ reserve(ਮਾਪ_t n) { database.reserve(n); }
	ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ database.ਮਾਪ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ database.ਖਾਲੀ_ਥਾਵਾਂ(); }
	ਖਾਲੀ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ਅਚਲ { ਵਾਪਸ database.begin(); }
	const_iterator element(ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ database.element(n); }
	const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ database.ਸਿਰਾ(); }
};

} /* namespace hashlib */

#ਜੇ_ਅੰਤ
