// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HASHLIB_H
#ವ್ಯಾಖ್ಯಾನಿಸು HASHLIB_H

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

#ವ್ಯಾಖ್ಯಾನಿಸು YS_HASHING_ಆವೃತ್ತಿ 1

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

#ಆದರೆ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ __attribute__((warn_ಬಳಸದ))
#ಇಲ್ಲದಿದ್ದರೆ
#  define HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ hashtable_ಗಾತ್ರ_trigger = 2;
ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ hashtable_ಗಾತ್ರ_ಗುಣಕ = 3;

namespace legacy {
	ಸಾಲೊಳಗೆ uint32_t djb2_ಕೂಡಿಸು(uint32_t a, uint32_t b) {
		ಹಿಂತಿರುಗಿ ((a << 5) + a) + b;
	}
};

ಮಾದರಿ<typename T>
ರಚನೆ hash_ops;

ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mkhash_xorshift(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ a) {
	ಆದರೆ (ಗಾತ್ರ(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} ಇಲ್ಲದಿದ್ದರೆ
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ಹಿಂತಿರುಗಿ a;
}

class HasherDJB32 {
ಸಾರ್ವಜನಿಕ:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ಖಾಸಗಿ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ಸ್ಥಿರ uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ಹಿಂತಿರುಗಿ hash;
	}
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ಹಿಂತಿರುಗಿ;
	}
	ಶೂನ್ಯ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ಹಿಂತಿರುಗಿ;
	}
	[[nodiscard]]
	hash_t yield() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ (hash_t)state;
	}

	ಮಾದರಿ<typename T>
	ಶೂನ್ಯ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	ಮಾದರಿ<typename T>
	ಶೂನ್ಯ eat(ಸ್ಥಿರಾಂಕ T& t) {
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
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_ತುದಿ_LOOP_FST [[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_ತುದಿ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ಹಿಂತಿರುಗಿ h; \
}

ಮಾದರಿ<typename T>
ರಚನೆ hash_ops {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ T &a, ಸ್ಥಿರಾಂಕ T &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ T &a, Hasher h) {
		ಆದರೆ constexpr (std::is_integral_v<T>) {
			static_assert(ಗಾತ್ರ(T) <= ಗಾತ್ರ(uint64_t));
			ಆದರೆ (ಗಾತ್ರ(T) == ಗಾತ್ರ(uint64_t))
				h.hash64(a);
			ಇಲ್ಲದಿದ್ದರೆ
				h.hash32(a);
			ಹಿಂತಿರುಗಿ h;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ಹಿಂತಿರುಗಿ hash_ops<u_type>::hash_into((u_type) a, h);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (std::is_ಸೂಚಕ_v<T>) {
			ಹಿಂತಿರುಗಿ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = a.ಗಾತ್ರ_2();
			ಪೂರ್ಣಾಂಕ i = 0;
			ಆಗ (i + 8 < ಗಾತ್ರ_2) {
				uint64_t v;
				ಸ್ಮರಣೆ_ನಕಲು(&v, a.ದತ್ತಾಂಶ() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			ಸ್ಮರಣೆ_ನಕಲು(&v, a.ದತ್ತಾಂಶ() + i, ಗಾತ್ರ_2 - i);
			h.hash64(v);
			ಹಿಂತಿರುಗಿ h;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಹಿಂತಿರುಗಿ a.hash_into(h);
		}
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ T &a) HASH_ತುದಿ_LOOP_SND
};

ಮಾದರಿ<typename P, typename Q> ರಚನೆ hash_ops<std::pair<P, Q>> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::pair<P, Q> &a, ಸ್ಥಿರಾಂಕ std::pair<P, Q> &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.ಮೊದಲ, h);
		h = hash_ops<Q>::hash_into(a.ಎರಡನೇ, h);
		ಹಿಂತಿರುಗಿ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ std::pair<P, Q> &a) HASH_ತುದಿ_LOOP_SND
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash(ಸ್ಥಿರಾಂಕ P &p, ಸ್ಥಿರಾಂಕ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ಹಿಂತಿರುಗಿ h;
	}
};

ಮಾದರಿ<typename... T> ರಚನೆ hash_ops<std::tuple<T...>> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::tuple<T...> &a, ಸ್ಥಿರಾಂಕ std::tuple<T...> &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	ಮಾದರಿ<ಗಾತ್ರ_t I = 0>
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ typename std::enable_if<I == ಗಾತ್ರ...(T), Hasher>::type hash_into(ಸ್ಥಿರಾಂಕ std::tuple<T...> &, Hasher h) {
		ಹಿಂತಿರುಗಿ h;
	}
	ಮಾದರಿ<ಗಾತ್ರ_t I = 0>
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ typename std::enable_if<I != ಗಾತ್ರ...(T), Hasher>::type hash_into(ಸ್ಥಿರಾಂಕ std::tuple<T...> &a, Hasher h) {
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ಹಿಂತಿರುಗಿ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ std::tuple<T...> &a) HASH_ತುದಿ_LOOP_SND
};

ಮಾದರಿ<typename T> ರಚನೆ hash_ops<std::ಸದಿಶ<T>> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &a, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &a, Hasher h) {
		h.eat((uint32_t)a.ಗಾತ್ರ_2());
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ k : a)
			h.eat(k);
		ಹಿಂತಿರುಗಿ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &a) HASH_ತುದಿ_LOOP_SND
};

ಮಾದರಿ<typename T, ಗಾತ್ರ_t N> ರಚನೆ hash_ops<std::array<T, N>> {
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::array<T, N> &a, ಸ್ಥಿರಾಂಕ std::array<T, N> &b) {
        ಹಿಂತಿರುಗಿ a == b;
    }
    [[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ std::array<T, N> &a, Hasher h) {
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ಹಿಂತಿರುಗಿ h;
    }
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ std::array<T, N> &a) HASH_ತುದಿ_LOOP_SND
};

ರಚನೆ hash_cstr_ops {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
		ಹಿಂತಿರುಗಿ strcmp(a, b) == 0;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, Hasher h) {
		ಆಗ (*a)
			h.hash32(*(a++));
		ಹಿಂತಿರುಗಿ h;
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a) HASH_ತುದಿ_LOOP_SND
};

ಮಾದರಿ <> ರಚನೆ hash_ops<ಅಕ್ಷರ*> : hash_cstr_ops {};

ರಚನೆ hash_ptr_ops {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, Hasher h) {
		ಹಿಂತಿರುಗಿ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a) HASH_ತುದಿ_LOOP_SND
};

ರಚನೆ hash_obj_ops {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	ಮಾದರಿ<typename T>
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ T *a, Hasher h) {
		ಆದರೆ (a)
			h = a->hash_into(h);
		ಇಲ್ಲದಿದ್ದರೆ
			h.eat(0);
		ಹಿಂತಿರುಗಿ h;
	}
	ಮಾದರಿ<typename T>
	HASH_ತುದಿ_LOOP_FST (ಸ್ಥಿರಾಂಕ T *a) HASH_ತುದಿ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
ಮಾದರಿ<typename T>
[[nodiscard]]
Hasher::hash_t ಸತತ_hash(ಸ್ಥಿರಾಂಕ T& obj) {
	ಹಿಂತಿರುಗಿ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
ಮಾದರಿ<typename T>
[[nodiscard]]
[[deprecated]]
ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mkhash(ಸ್ಥಿರಾಂಕ T &v) {
	ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) ಸತತ_hash<T>(v);
}

ಮಾದರಿ<> ರಚನೆ hash_ops<std::monostate> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(std::monostate a, std::monostate b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(std::monostate, Hasher h) {
		ಹಿಂತಿರುಗಿ h;
	}
};

ಮಾದರಿ<typename... T> ರಚನೆ hash_ops<std::variant<T...>> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::variant<T...> &a, ಸ್ಥಿರಾಂಕ std::variant<T...> &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ std::variant<T...> &a, Hasher h) {
		std::ಭೇಟಿ([& h](ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &v) { h.eat(v); }, a);
		h.eat(a.ಸೂಚ್ಯಂಕ());
		ಹಿಂತಿರುಗಿ h;
	}
};

ಮಾದರಿ<typename T> ರಚನೆ hash_ops<std::optional<T>> {
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ std::optional<T> &a, ಸ್ಥಿರಾಂಕ std::optional<T> &b) {
		ಹಿಂತಿರುಗಿ a == b;
	}
	[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ std::optional<T> &a, Hasher h) {
		ಆದರೆ(a.has_ಮೌಲ್ಯ())
			h.eat(*a);
		ಇಲ್ಲದಿದ್ದರೆ
			h.eat(0);
		ಹಿಂತಿರುಗಿ h;
	}
};

ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashtable_ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ min_ಗಾತ್ರ)
{
	// Primes as generated by https://oeis.org/A175953
	ಸ್ಥಿರ std::ಸದಿಶ<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ> zero_and_some_ಅವಿಭಾಜ್ಯಗಳು = {
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

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ p : zero_and_some_ಅವಿಭಾಜ್ಯಗಳು)
		ಆದರೆ (p >= min_ಗಾತ್ರ) ಹಿಂತಿರುಗಿ p;

	ಆದರೆ (ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) == 4)
		throw std::ಉದ್ದಳತೆ_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ p : zero_and_some_ಅವಿಭಾಜ್ಯಗಳು)
		ಆದರೆ (100129 * p > min_ಗಾತ್ರ) ಹಿಂತಿರುಗಿ 100129 * p;

	throw std::ಉದ್ದಳತೆ_error("hash table exceeded maximum size.");
}

ಮಾದರಿ<typename K, typename T, typename OPS = hash_ops<K>> class dict;
ಮಾದರಿ<typename K, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0, typename OPS = hash_ops<K>> class idict;
ಮಾದರಿ<typename K, typename OPS = hash_ops<K>> class pool;
ಮಾದರಿ<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
ಸಾರ್ವಜನಿಕ:
	commutative_hash() {
		buckets.fill(0);
	}
	ಮಾದರಿ <typename T>
	ಶೂನ್ಯ eat(ಸ್ಥಿರಾಂಕ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	ಶೂನ್ಯ eat(ಸ್ಥಿರಾಂಕ Hasher &h) {
		Hasher::hash_t v = h.yield();
		ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = v & (buckets.ಗಾತ್ರ_2() - 1);
		buckets[ಸೂಚ್ಯಂಕ] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ b : buckets)
			h.eat(b);
		ಹಿಂತಿರುಗಿ h;
	}
ಖಾಸಗಿ:
	std::array<Hasher::hash_t, 4> buckets;
};

ಮಾದರಿ<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ dict {
	ರಚನೆ ನಮೂದು_t
	{
		std::pair<K, T> udata;
		ಪೂರ್ಣಾಂಕ ಮುಂದಿನ;

		ನಮೂದು_t() { }
		ನಮೂದು_t(ಸ್ಥಿರಾಂಕ std::pair<K, T> &udata, ಪೂರ್ಣಾಂಕ ಮುಂದಿನ) : udata(udata), ಮುಂದಿನ(ಮುಂದಿನ) { }
		ನಮೂದು_t(std::pair<K, T> &&udata, ಪೂರ್ಣಾಂಕ ಮುಂದಿನ) : udata(std::move(udata)), ಮುಂದಿನ(ಮುಂದಿನ) { }
		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ ನಮೂದು_t &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ udata.ಮೊದಲ < other.udata.ಮೊದಲ; }
	};

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> hashtable;
	std::ಸದಿಶ<ನಮೂದು_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NDEBUG
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ do_assert(bool) { }
#ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ do_assert(bool cond) {
		ಆದರೆ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#ಆದರೆ_ಅಂತ್ಯ

	Hasher::hash_t do_hash(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = 0;
		ಆದರೆ (!hashtable.ಖಾಲಿಗಳು())
			hash = ops.hash(ಕೀಲಿ).yield() % (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(hashtable.ಗಾತ್ರ_2());
		ಹಿಂತಿರುಗಿ hash;
	}

	ಶೂನ್ಯ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ಗಾತ್ರ(entries.ಸಾಮರ್ಥ್ಯ() * hashtable_ಗಾತ್ರ_ಗುಣಕ), -1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()); i++) {
			do_assert(-1 <= entries[i].ಮುಂದಿನ && entries[i].ಮುಂದಿನ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
			Hasher::hash_t hash = do_hash(entries[i].udata.ಮೊದಲ);
			entries[i].ಮುಂದಿನ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ಪೂರ್ಣಾಂಕ do_erase(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, Hasher::hash_t hash)
	{
		do_assert(ಸೂಚ್ಯಂಕ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು() || ಸೂಚ್ಯಂಕ < 0)
			ಹಿಂತಿರುಗಿ 0;

		ಪೂರ್ಣಾಂಕ k = hashtable[hash];
		do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));

		ಆದರೆ (k == ಸೂಚ್ಯಂಕ) {
			hashtable[hash] = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆಗ (entries[k].ಮುಂದಿನ != ಸೂಚ್ಯಂಕ) {
				k = entries[k].ಮುಂದಿನ;
				do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
			}
			entries[k].ಮುಂದಿನ = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
		}

		ಪೂರ್ಣಾಂಕ ಹಿಂಭಾಗ_idx = entries.ಗಾತ್ರ_2()-1;

		ಆದರೆ (ಸೂಚ್ಯಂಕ != ಹಿಂಭಾಗ_idx)
		{
			Hasher::hash_t ಹಿಂಭಾಗ_hash = do_hash(entries[ಹಿಂಭಾಗ_idx].udata.ಮೊದಲ);

			k = hashtable[ಹಿಂಭಾಗ_hash];
			do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));

			ಆದರೆ (k == ಹಿಂಭಾಗ_idx) {
				hashtable[ಹಿಂಭಾಗ_hash] = ಸೂಚ್ಯಂಕ;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆಗ (entries[k].ಮುಂದಿನ != ಹಿಂಭಾಗ_idx) {
					k = entries[k].ಮುಂದಿನ;
					do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
				}
				entries[k].ಮುಂದಿನ = ಸೂಚ್ಯಂಕ;
			}

			entries[ಸೂಚ್ಯಂಕ] = std::move(entries[ಹಿಂಭಾಗ_idx]);
		}

		entries.ತೆಗೆ_ಹಿಂಭಾಗ();

		ಆದರೆ (entries.ಖಾಲಿಗಳು())
			hashtable.clear();

		ಹಿಂತಿರುಗಿ 1;
	}

	ಪೂರ್ಣಾಂಕ do_lookup(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ -1;

		ಆದರೆ (entries.ಗಾತ್ರ_2() * hashtable_ಗಾತ್ರ_trigger > hashtable.ಗಾತ್ರ_2()) {
			do_rehash();
			hash = do_hash(ಕೀಲಿ);
		}

		ಹಿಂತಿರುಗಿ do_lookup_ಆಂತರಿಕ(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಂಕ do_lookup_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t hash) ಸ್ಥಿರಾಂಕ
	{
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = hashtable[hash];

		ಆಗ (ಸೂಚ್ಯಂಕ >= 0 && !ops.cmp(entries[ಸೂಚ್ಯಂಕ].udata.ಮೊದಲ, ಕೀಲಿ)) {
			ಸೂಚ್ಯಂಕ = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
			do_assert(-1 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
		}

		ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
	}

	ಪೂರ್ಣಾಂಕ do_lookup_no_rehash(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t hash) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ -1;

		ಹಿಂತಿರುಗಿ do_lookup_ಆಂತರಿಕ(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಂಕ do_ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, ಸ್ಥಿರಾಂಕ Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು()) {
			entries.emplace_ಹಿಂಭಾಗ(std::pair<K, T>(ಕೀಲಿ, T()), -1);
			do_rehash();
		} ಇಲ್ಲದಿದ್ದರೆ {
			entries.emplace_ಹಿಂಭಾಗ(std::pair<K, T>(ಕೀಲಿ, T()), hashtable[hash]);
			hashtable[hash] = entries.ಗಾತ್ರ_2() - 1;
		}
		ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2() - 1;
	}

	ಪೂರ್ಣಾಂಕ do_ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ std::pair<K, T> &ಮೌಲ್ಯ, ಸ್ಥಿರಾಂಕ Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು()) {
			entries.emplace_ಹಿಂಭಾಗ(ಮೌಲ್ಯ, -1);
			do_rehash();
		} ಇಲ್ಲದಿದ್ದರೆ {
			entries.emplace_ಹಿಂಭಾಗ(ಮೌಲ್ಯ, hashtable[hash]);
			hashtable[hash] = entries.ಗಾತ್ರ_2() - 1;
		}
		ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2() - 1;
	}

	ಪೂರ್ಣಾಂಕ do_ಸೇರಿಸು(std::pair<K, T> &&rvalue, ಸ್ಥಿರಾಂಕ Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು()) {
			entries.emplace_ಹಿಂಭಾಗ(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} ಇಲ್ಲದಿದ್ದರೆ {
			entries.emplace_ಹಿಂಭಾಗ(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ಗಾತ್ರ_2() - 1;
		}
		ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2() - 1;
	}

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		ಮಿತ್ರ class dict;
	ಸಂರಕ್ಷಿತ:
		ಸ್ಥಿರಾಂಕ dict *ptr;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
		const_iterator(ಸ್ಥಿರಾಂಕ dict *ptr, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ptr(ptr), ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::bidirectional_iterator_ಟ್ಯಾಗ್ iterator_category;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<K, T> ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ std::pair<K, T>* ಸೂಚಕ;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ--; ಹಿಂತಿರುಗಿ *this; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) { const_iterator tmp = *this; ಸೂಚ್ಯಂಕ--; ಹಿಂತಿರುಗಿ tmp; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ--() { ಸೂಚ್ಯಂಕ++; ಹಿಂತಿರುಗಿ *this; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) { const_iterator tmp = *this; ಸೂಚ್ಯಂಕ++; ಹಿಂತಿರುಗಿ tmp; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ+=(ಪೂರ್ಣಾಂಕ amt) { ಸೂಚ್ಯಂಕ -= amt; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ > other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
		ಸ್ಥಿರಾಂಕ std::pair<K, T> &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ std::pair<K, T> *ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
	};

	class iterator
	{
		ಮಿತ್ರ class dict;
	ಸಂರಕ್ಷಿತ:
		dict *ptr;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
		iterator(dict *ptr, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ptr(ptr), ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::forward_iterator_ಟ್ಯಾಗ್ iterator_category;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<K, T> ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<K, T>* ಸೂಚಕ;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<K, T>& reference;
		iterator() { }
		iterator ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ--; ಹಿಂತಿರುಗಿ *this; }
		iterator ಕ್ರಿಯಾಕಾರಕ+=(ಪೂರ್ಣಾಂಕ amt) { ಸೂಚ್ಯಂಕ -= amt; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ > other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
		std::pair<K, T> &ಕ್ರಿಯಾಕಾರಕ*() { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		std::pair<K, T> *ಕ್ರಿಯಾಕಾರಕ->() { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ std::pair<K, T> &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ std::pair<K, T> *ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಕ್ರಿಯಾಕಾರಕ const_iterator() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(ptr, ಸೂಚ್ಯಂಕ); }
	};
	using ಹಿಮ್ಮುಖ_iterator = std::ಹಿಮ್ಮುಖ_iterator<const_iterator>;
	ಹಿಮ್ಮುಖ_iterator rbegin() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(ಕೊನೆ());
	}
	ಹಿಮ್ಮುಖ_iterator rend() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ಸ್ಥಿರಾಂಕ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		ಅದಲುಬದಲು(other);
	}

	dict &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ dict &other) {
		entries = other.entries;
		do_rehash();
		ಹಿಂತಿರುಗಿ *this;
	}

	dict &ಕ್ರಿಯಾಕಾರಕ=(dict &&other) noexcept {
		clear();
		ಅದಲುಬದಲು(other);
		ಹಿಂತಿರುಗಿ *this;
	}

	dict(ಸ್ಥಿರಾಂಕ std::initializer_list<std::pair<K, T>> &list)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : list)
			ಸೇರಿಸು(it);
	}

	ಮಾದರಿ<class ಒಳಹರಿವು_Iterator>
	dict(ಒಳಹರಿವು_Iterator ಮೊದಲ, ಒಳಹರಿವು_Iterator ಕೊನೆಯ)
	{
		ಸೇರಿಸು(ಮೊದಲ, ಕೊನೆಯ);
	}

	ಮಾದರಿ<class ಒಳಹರಿವು_Iterator>
	ಶೂನ್ಯ ಸೇರಿಸು(ಒಳಹರಿವು_Iterator ಮೊದಲ, ಒಳಹರಿವು_Iterator ಕೊನೆಯ)
	{
		ಗಾಗಿ (; ಮೊದಲ != ಕೊನೆಯ; ++ಮೊದಲ)
			ಸೇರಿಸು(*ಮೊದಲ);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ std::pair<K, T> &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಮೌಲ್ಯ.ಮೊದಲ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಮೌಲ್ಯ.ಮೊದಲ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಮೌಲ್ಯ, hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.ಮೊದಲ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(rvalue.ಮೊದಲ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::forward<std::pair<K, T>>(rvalue), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ಸ್ಥಿರಾಂಕ &ಕೀಲಿ, T ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(ಕೀಲಿ, ಮೌಲ್ಯ), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ಸ್ಥಿರಾಂಕ &ಕೀಲಿ, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(ಕೀಲಿ, std::forward<T>(rvalue)), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ಸ್ಥಿರಾಂಕ &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ಪೂರ್ಣಾಂಕ i = do_lookup(rkey, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(std::forward<K>(rkey), ಮೌಲ್ಯ), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ಪೂರ್ಣಾಂಕ i = do_lookup(rkey, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ಪೂರ್ಣಾಂಕ erase(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = do_lookup(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ do_erase(ಸೂಚ್ಯಂಕ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->ಮೊದಲ);
		do_erase(it.ಸೂಚ್ಯಂಕ, hash);
		ಹಿಂತಿರುಗಿ ++it;
	}

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, const_iterator it) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i < 0 || i > it.ಸೂಚ್ಯಂಕ ? 0 : 1;
	}

	iterator ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ ಕೊನೆ();
		ಹಿಂತಿರುಗಿ iterator(this, i);
	}

	const_iterator ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ ಕೊನೆ();
		ಹಿಂತಿರುಗಿ const_iterator(this, i);
	}

	T& at(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			throw std::ಹೊರಹೋಗು_ದ_range("dict::at()");
		ಹಿಂತಿರುಗಿ entries[i].udata.ಎರಡನೇ;
	}

	ಸ್ಥಿರಾಂಕ T& at(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			throw std::ಹೊರಹೋಗು_ದ_range("dict::at()");
		ಹಿಂತಿರುಗಿ entries[i].udata.ಎರಡನೇ;
	}

	ಸ್ಥಿರಾಂಕ T& at(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, ಸ್ಥಿರಾಂಕ T &defval) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ defval;
		ಹಿಂತಿರುಗಿ entries[i].udata.ಎರಡನೇ;
	}

	T& ಕ್ರಿಯಾಕಾರಕ[](ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			i = do_ಸೇರಿಸು(std::pair<K, T>(ಕೀಲಿ, T()), hash);
		ಹಿಂತಿರುಗಿ entries[i].udata.ಎರಡನೇ;
	}

	ಮಾದರಿ<typename Compare = std::less<K>>
	ಶೂನ್ಯ ವಿಂಗಡಿಸು(Compare comp = Compare())
	{
		std::ವಿಂಗಡಿಸು(entries.begin(), entries.ಕೊನೆ(), [comp](ಸ್ಥಿರಾಂಕ ನಮೂದು_t &a, ಸ್ಥಿರಾಂಕ ನಮೂದು_t &b){ ಹಿಂತಿರುಗಿ comp(b.udata.ಮೊದಲ, a.udata.ಮೊದಲ); });
		do_rehash();
	}

	ಶೂನ್ಯ ಅದಲುಬದಲು(dict &other)
	{
		hashtable.ಅದಲುಬದಲು(other.hashtable);
		entries.ಅದಲುಬದಲು(other.entries);
	}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ dict &other) ಸ್ಥಿರಾಂಕ {
		ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
			ಹಿಂತಿರುಗಿ false;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : entries) {
			ಸ್ವಯಂಚಾಲಿತ oit = other.ಕಂಡುಹಿಡಿ(it.udata.ಮೊದಲ);
			ಆದರೆ (oit == other.ಕೊನೆ() || !(oit->ಎರಡನೇ == it.udata.ಎರಡನೇ))
				ಹಿಂತಿರುಗಿ false;
		}
		ಹಿಂತಿರುಗಿ true;
	}

	bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ dict &other) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		commutative_hash comm;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : entries) {
			Hasher ನಮೂದು_hash;
			ನಮೂದು_hash.eat(it.udata.ಮೊದಲ);
			ನಮೂದು_hash.eat(it.udata.ಎರಡನೇ);
			comm.eat(ನಮೂದು_hash);
		}
		ಹಿಂತಿರುಗಿ comm.hash_into(h);
	}

	ಶೂನ್ಯ reserve(ಗಾತ್ರ_t n) { entries.reserve(n); }
	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ entries.ಖಾಲಿಗಳು(); }
	ಶೂನ್ಯ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ಹಿಂತಿರುಗಿ iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1); }
	iterator element(ಪೂರ್ಣಾಂಕ n) { ಹಿಂತಿರುಗಿ iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1-n); }
	iterator ಕೊನೆ() { ಹಿಂತಿರುಗಿ iterator(nullptr, -1); }

	const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1); }
	const_iterator element(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1-n); }
	const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(this, -1); }
};

ಮಾದರಿ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ pool
{
	ಮಾದರಿ<typename, ಪೂರ್ಣಾಂಕ, typename> ಮಿತ್ರ class idict;

ಸಂರಕ್ಷಿತ:
	ರಚನೆ ನಮೂದು_t
	{
		K udata;
		ಪೂರ್ಣಾಂಕ ಮುಂದಿನ;

		ನಮೂದು_t() { }
		ನಮೂದು_t(ಸ್ಥಿರಾಂಕ K &udata, ಪೂರ್ಣಾಂಕ ಮುಂದಿನ) : udata(udata), ಮುಂದಿನ(ಮುಂದಿನ) { }
		ನಮೂದು_t(K &&udata, ಪೂರ್ಣಾಂಕ ಮುಂದಿನ) : udata(std::move(udata)), ಮುಂದಿನ(ಮುಂದಿನ) { }
	};

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> hashtable;
	std::ಸದಿಶ<ನಮೂದು_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NDEBUG
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ do_assert(bool) { }
#ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ do_assert(bool cond) {
		ಆದರೆ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#ಆದರೆ_ಅಂತ್ಯ

	Hasher::hash_t do_hash(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = 0;
		ಆದರೆ (!hashtable.ಖಾಲಿಗಳು())
			hash = ops.hash(ಕೀಲಿ).yield() % (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(hashtable.ಗಾತ್ರ_2());
		ಹಿಂತಿರುಗಿ hash;
	}

	ಶೂನ್ಯ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ಗಾತ್ರ(entries.ಸಾಮರ್ಥ್ಯ() * hashtable_ಗಾತ್ರ_ಗುಣಕ), -1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()); i++) {
			do_assert(-1 <= entries[i].ಮುಂದಿನ && entries[i].ಮುಂದಿನ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].ಮುಂದಿನ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ಪೂರ್ಣಾಂಕ do_erase(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, Hasher::hash_t hash)
	{
		do_assert(ಸೂಚ್ಯಂಕ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು() || ಸೂಚ್ಯಂಕ < 0)
			ಹಿಂತಿರುಗಿ 0;

		ಪೂರ್ಣಾಂಕ k = hashtable[hash];
		ಆದರೆ (k == ಸೂಚ್ಯಂಕ) {
			hashtable[hash] = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆಗ (entries[k].ಮುಂದಿನ != ಸೂಚ್ಯಂಕ) {
				k = entries[k].ಮುಂದಿನ;
				do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
			}
			entries[k].ಮುಂದಿನ = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
		}

		ಪೂರ್ಣಾಂಕ ಹಿಂಭಾಗ_idx = entries.ಗಾತ್ರ_2()-1;

		ಆದರೆ (ಸೂಚ್ಯಂಕ != ಹಿಂಭಾಗ_idx)
		{
			Hasher::hash_t ಹಿಂಭಾಗ_hash = do_hash(entries[ಹಿಂಭಾಗ_idx].udata);

			k = hashtable[ಹಿಂಭಾಗ_hash];
			ಆದರೆ (k == ಹಿಂಭಾಗ_idx) {
				hashtable[ಹಿಂಭಾಗ_hash] = ಸೂಚ್ಯಂಕ;
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆಗ (entries[k].ಮುಂದಿನ != ಹಿಂಭಾಗ_idx) {
					k = entries[k].ಮುಂದಿನ;
					do_assert(0 <= k && k < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
				}
				entries[k].ಮುಂದಿನ = ಸೂಚ್ಯಂಕ;
			}

			entries[ಸೂಚ್ಯಂಕ] = std::move(entries[ಹಿಂಭಾಗ_idx]);
		}

		entries.ತೆಗೆ_ಹಿಂಭಾಗ();

		ಆದರೆ (entries.ಖಾಲಿಗಳು())
			hashtable.clear();

		ಹಿಂತಿರುಗಿ 1;
	}

	ಪೂರ್ಣಾಂಕ do_lookup(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ -1;

		ಆದರೆ (entries.ಗಾತ್ರ_2() * hashtable_ಗಾತ್ರ_trigger > hashtable.ಗಾತ್ರ_2()) {
			do_rehash();
			hash = do_hash(ಕೀಲಿ);
		}

		ಹಿಂತಿರುಗಿ do_lookup_ಆಂತರಿಕ(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಂಕ do_lookup_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t hash) ಸ್ಥಿರಾಂಕ
	{
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = hashtable[hash];

		ಆಗ (ಸೂಚ್ಯಂಕ >= 0 && !ops.cmp(entries[ಸೂಚ್ಯಂಕ].udata, ಕೀಲಿ)) {
			ಸೂಚ್ಯಂಕ = entries[ಸೂಚ್ಯಂಕ].ಮುಂದಿನ;
			do_assert(-1 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2()));
		}

		ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
	}

	ಪೂರ್ಣಾಂಕ do_lookup_no_rehash(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, Hasher::hash_t hash) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ -1;

		ಹಿಂತಿರುಗಿ do_lookup_ಆಂತರಿಕ(ಕೀಲಿ, hash);
	}

	ಪೂರ್ಣಾಂಕ do_ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ K &ಮೌಲ್ಯ, Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು()) {
			entries.emplace_ಹಿಂಭಾಗ(ಮೌಲ್ಯ, -1);
			do_rehash();
			hash = do_hash(ಮೌಲ್ಯ);
		} ಇಲ್ಲದಿದ್ದರೆ {
			entries.emplace_ಹಿಂಭಾಗ(ಮೌಲ್ಯ, hashtable[hash]);
			hashtable[hash] = entries.ಗಾತ್ರ_2() - 1;
		}
		ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2() - 1;
	}

	ಪೂರ್ಣಾಂಕ do_ಸೇರಿಸು(K &&rvalue, Hasher::hash_t &hash)
	{
		ಆದರೆ (hashtable.ಖಾಲಿಗಳು()) {
			entries.emplace_ಹಿಂಭಾಗ(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.ಹಿಂಭಾಗ().udata);
		} ಇಲ್ಲದಿದ್ದರೆ {
			entries.emplace_ಹಿಂಭಾಗ(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ಗಾತ್ರ_2() - 1;
		}
		ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2() - 1;
	}

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		ಮಿತ್ರ class pool;
	ಸಂರಕ್ಷಿತ:
		ಸ್ಥಿರಾಂಕ pool *ptr;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
		const_iterator(ಸ್ಥಿರಾಂಕ pool *ptr, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ptr(ptr), ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::forward_iterator_ಟ್ಯಾಗ್ iterator_category;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K* ಸೂಚಕ;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K& reference;
		const_iterator() { }
		const_iterator ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ--; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
		ಸ್ಥಿರಾಂಕ K &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ K *ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
	};

	class iterator
	{
		ಮಿತ್ರ class pool;
	ಸಂರಕ್ಷಿತ:
		pool *ptr;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
		iterator(pool *ptr, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ptr(ptr), ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::forward_iterator_ಟ್ಯಾಗ್ iterator_category;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K* ಸೂಚಕ;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K& reference;
		iterator() { }
		iterator ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ--; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
		K &ಕ್ರಿಯಾಕಾರಕ*() { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		K *ಕ್ರಿಯಾಕಾರಕ->() { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ K &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಸ್ಥಿರಾಂಕ K *ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ &ptr->entries[ಸೂಚ್ಯಂಕ].udata; }
		ಕ್ರಿಯಾಕಾರಕ const_iterator() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(ptr, ಸೂಚ್ಯಂಕ); }
	};

	constexpr pool()
	{
	}

	pool(ಸ್ಥಿರಾಂಕ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		ಅದಲುಬದಲು(other);
	}

	pool &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ pool &other) {
		entries = other.entries;
		do_rehash();
		ಹಿಂತಿರುಗಿ *this;
	}

	pool &ಕ್ರಿಯಾಕಾರಕ=(pool &&other) noexcept {
		clear();
		ಅದಲುಬದಲು(other);
		ಹಿಂತಿರುಗಿ *this;
	}

	pool(ಸ್ಥಿರಾಂಕ std::initializer_list<K> &list)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : list)
			ಸೇರಿಸು(it);
	}

	ಮಾದರಿ<class ಒಳಹರಿವು_Iterator>
	pool(ಒಳಹರಿವು_Iterator ಮೊದಲ, ಒಳಹರಿವು_Iterator ಕೊನೆಯ)
	{
		ಸೇರಿಸು(ಮೊದಲ, ಕೊನೆಯ);
	}

	ಮಾದರಿ<class ಒಳಹರಿವು_Iterator>
	ಶೂನ್ಯ ಸೇರಿಸು(ಒಳಹರಿವು_Iterator ಮೊದಲ, ಒಳಹರಿವು_Iterator ಕೊನೆಯ)
	{
		ಗಾಗಿ (; ಮೊದಲ != ಕೊನೆಯ; ++ಮೊದಲ)
			ಸೇರಿಸು(*ಮೊದಲ);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ K &ಮೌಲ್ಯ)
	{
		Hasher::hash_t hash = do_hash(ಮೌಲ್ಯ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಮೌಲ್ಯ, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(ಮೌಲ್ಯ, hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ಸೇರಿಸು(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		ಪೂರ್ಣಾಂಕ i = do_lookup(rvalue, hash);
		ಆದರೆ (i >= 0)
			ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ಸೇರಿಸು(std::forward<K>(rvalue), hash);
		ಹಿಂತಿರುಗಿ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ಮಾದರಿ<typename... ವಾದಗಳು_3>
	std::pair<iterator, bool> emplace(ವಾದಗಳು_3&&... ವಾದಗಳು_2)
	{
		ಹಿಂತಿರುಗಿ ಸೇರಿಸು(K(std::forward<ವಾದಗಳು_3>(ವಾದಗಳು_2)...));
	}

	ಪೂರ್ಣಾಂಕ erase(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = do_lookup(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ do_erase(ಸೂಚ್ಯಂಕ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.ಸೂಚ್ಯಂಕ, hash);
		ಹಿಂತಿರುಗಿ ++it;
	}

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, const_iterator it) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i < 0 || i > it.ಸೂಚ್ಯಂಕ ? 0 : 1;
	}

	iterator ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ ಕೊನೆ();
		ಹಿಂತಿರುಗಿ iterator(this, i);
	}

	const_iterator ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ ಕೊನೆ();
		ಹಿಂತಿರುಗಿ const_iterator(this, i);
	}

	bool ಕ್ರಿಯಾಕಾರಕ[](ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = do_lookup(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i >= 0;
	}

	ಮಾದರಿ<typename Compare = std::less<K>>
	ಶೂನ್ಯ ವಿಂಗಡಿಸು(Compare comp = Compare())
	{
		std::ವಿಂಗಡಿಸು(entries.begin(), entries.ಕೊನೆ(), [comp](ಸ್ಥಿರಾಂಕ ನಮೂದು_t &a, ಸ್ಥಿರಾಂಕ ನಮೂದು_t &b){ ಹಿಂತಿರುಗಿ comp(b.udata, a.udata); });
		do_rehash();
	}

	K ತೆಗೆ_2()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ಹಿಂತಿರುಗಿ ret;
	}

	ಶೂನ್ಯ ಅದಲುಬದಲು(pool &other)
	{
		hashtable.ಅದಲುಬದಲು(other.hashtable);
		entries.ಅದಲುಬದಲು(other.entries);
	}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ pool &other) ಸ್ಥಿರಾಂಕ {
		ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
			ಹಿಂತಿರುಗಿ false;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : entries)
			ಆದರೆ (!other.ಎಣಿಕೆ_2(it.udata))
				ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ true;
	}

	bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ pool &other) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		commutative_hash comm;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ಹಿಂತಿರುಗಿ comm.hash_into(h);
	}

	ಶೂನ್ಯ reserve(ಗಾತ್ರ_t n) { entries.reserve(n); }
	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ entries.ಗಾತ್ರ_2(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ entries.ಖಾಲಿಗಳು(); }
	ಶೂನ್ಯ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ಹಿಂತಿರುಗಿ iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1); }
	iterator element(ಪೂರ್ಣಾಂಕ n) { ಹಿಂತಿರುಗಿ iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1-n); }
	iterator ಕೊನೆ() { ಹಿಂತಿರುಗಿ iterator(nullptr, -1); }

	const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1); }
	const_iterator element(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(this, ಪೂರ್ಣಾಂಕ(entries.ಗಾತ್ರ_2())-1-n); }
	const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(nullptr, -1); }
};

ಮಾದರಿ<typename K, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ idict
{
	pool<K, OPS> database;

ಸಾರ್ವಜನಿಕ:
	class const_iterator
	{
		ಮಿತ್ರ class idict;
	ಸಂರಕ್ಷಿತ:
		ಸ್ಥಿರಾಂಕ idict &container;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
		const_iterator(ಸ್ಥಿರಾಂಕ idict &container, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : container(container), ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ) { }
	ಸಾರ್ವಜನಿಕ:
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::forward_iterator_ಟ್ಯಾಗ್ iterator_category;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K ಮೌಲ್ಯ_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K* ಸೂಚಕ;
		ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ K& reference;
		const_iterator() { }
		const_iterator ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ++; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ const_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
		ಸ್ಥಿರಾಂಕ K &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ container[ಸೂಚ್ಯಂಕ]; }
		ಸ್ಥಿರಾಂಕ K *ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ &container[ಸೂಚ್ಯಂಕ]; }
	};

	constexpr idict()
	{
	}

	ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = database.do_lookup(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			i = database.do_ಸೇರಿಸು(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i + ಸ್ಥಾನಾಂತರ;
	}

	ಪೂರ್ಣಾಂಕ at(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			throw std::ಹೊರಹೋಗು_ದ_range("idict::at()");
		ಹಿಂತಿರುಗಿ i + ಸ್ಥಾನಾಂತರ;
	}

	ಪೂರ್ಣಾಂಕ at(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, ಪೂರ್ಣಾಂಕ defval) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ defval;
		ಹಿಂತಿರುಗಿ i + ಸ್ಥಾನಾಂತರ;
	}

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಪೂರ್ಣಾಂಕ i = database.do_lookup_no_rehash(ಕೀಲಿ, hash);
		ಹಿಂತಿರುಗಿ i < 0 ? 0 : 1;
	}

	ಪೂರ್ಣಾಂಕ lookup(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
	{
		Hasher::hash_t hash = database.do_hash(ಕೀಲಿ);
		ಹಿಂತಿರುಗಿ database.do_lookup_no_rehash(ಕೀಲಿ, hash);
	}

	ಶೂನ್ಯ expect(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ, ಪೂರ್ಣಾಂಕ i)
	{
		ಪೂರ್ಣಾಂಕ j = (*this)(ಕೀಲಿ);
		ಆದರೆ (i != j)
			throw std::ಹೊರಹೋಗು_ದ_range("idict::expect()");
	}

	ಸ್ಥಿರಾಂಕ K &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ database.entries.at(ಸೂಚ್ಯಂಕ - ಸ್ಥಾನಾಂತರ).udata;
	}

	ಶೂನ್ಯ ಅದಲುಬದಲು(idict &other)
	{
		database.ಅದಲುಬದಲು(other.database);
	}

	ಶೂನ್ಯ reserve(ಗಾತ್ರ_t n) { database.reserve(n); }
	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.ಗಾತ್ರ_2(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.ಖಾಲಿಗಳು(); }
	ಶೂನ್ಯ clear() { database.clear(); }

	const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(*this, ಸ್ಥಾನಾಂತರ); }
	const_iterator element(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(*this, n); }
	const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(*this, ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ_2()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
ಮಾದರಿ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ಬಳಸದ mfp
{
	idict<K, 0, OPS> database;
	class Atomic_ಪೋಷಕ {
	ಸಾರ್ವಜನಿಕ:
		explicit Atomic_ಪೋಷಕ(ಪೂರ್ಣಾಂಕ p) : ಪೋಷಕ(p) {}
		Atomic_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ Atomic_ಪೋಷಕ &other) : ಪೋಷಕ(other.get()) {}
		Atomic_ಪೋಷಕ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Atomic_ಪೋಷಕ &other) { ಗಣ(other.get()); ಹಿಂತಿರುಗಿ *this; }
		ಪೂರ್ಣಾಂಕ get() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪೋಷಕ.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed); }
		ಶೂನ್ಯ ಗಣ(ಪೂರ್ಣಾಂಕ p) { ಪೋಷಕ.store(p, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed); }
	ಖಾಸಗಿ:
		std::atomic<ಪೂರ್ಣಾಂಕ> ಪೋಷಕ;
	};
	std::ಸದಿಶ<Atomic_ಪೋಷಕ> parents;

ಸಾರ್ವಜನಿಕ:
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ K &ಕೀಲಿ)
	{
		ಪೂರ್ಣಾಂಕ i = database(ಕೀಲಿ);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		ಆದರೆ (parents.ಗಾತ್ರ_2() < database.ಗಾತ್ರ_2()) {
			parents.emplace_ಹಿಂಭಾಗ(-1);
		}
		ಹಿಂತಿರುಗಿ i;
	}

	// Finds an element at given index
	ಸ್ಥಿರಾಂಕ K &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ database[ಸೂಚ್ಯಂಕ];
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
	ಪೂರ್ಣಾಂಕ ifind(ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
	{
		ಪೂರ್ಣಾಂಕ p = i, k = i;

		ಆಗ (true) {
			ಪೂರ್ಣಾಂಕ pp = parents[p].get();
			ಆದರೆ (pp < 0)
				ನಿಲ್ಲಿಸು;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		ಆಗ (k != p) {
			ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_k = parents[k].get();
			const_cast<Atomic_ಪೋಷಕ*>(&parents[k])->ಗಣ(p);
			k = ಮುಂದಿನ_k;
		}

		ಹಿಂತಿರುಗಿ p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	ಶೂನ್ಯ imerge(ಪೂರ್ಣಾಂಕ i, ಪೂರ್ಣಾಂಕ j)
	{
		i = ifind(i);
		j = ifind(j);

		ಆದರೆ (i != j)
			parents[i].ಗಣ(j);
	}

	ಶೂನ್ಯ ipromote(ಪೂರ್ಣಾಂಕ i)
	{
		ಪೂರ್ಣಾಂಕ k = i;

		ಆಗ (k != -1) {
			ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_k = parents[k].get();
			parents[k].ಗಣ(i);
			k = ಮುಂದಿನ_k;
		}

		parents[i].ಗಣ(-1);
	}

	ಪೂರ್ಣಾಂಕ lookup(ಸ್ಥಿರಾಂಕ K &a)
	{
		ಹಿಂತಿರುಗಿ ifind((*this)(a));
	}

	ಸ್ಥಿರಾಂಕ K &ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ K &a) ಸ್ಥಿರಾಂಕ
	{
		ಪೂರ್ಣಾಂಕ i = database.at(a, -1);
		ಆದರೆ (i < 0)
			ಹಿಂತಿರುಗಿ a;
		ಹಿಂತಿರುಗಿ (*this)[ifind(i)];
	}

	ಶೂನ್ಯ ವಿಲೀನ(ಸ್ಥಿರಾಂಕ K &a, ಸ್ಥಿರಾಂಕ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	ಶೂನ್ಯ promote(ಸ್ಥಿರಾಂಕ K &a)
	{
		ಪೂರ್ಣಾಂಕ i = database.at(a, -1);
		ಆದರೆ (i >= 0)
			ipromote(i);
	}

	ಶೂನ್ಯ ಅದಲುಬದಲು(mfp &other)
	{
		database.ಅದಲುಬದಲು(other.database);
		parents.ಅದಲುಬದಲು(other.parents);
	}

	ಶೂನ್ಯ reserve(ಗಾತ್ರ_t n) { database.reserve(n); }
	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.ಗಾತ್ರ_2(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.ಖಾಲಿಗಳು(); }
	ಶೂನ್ಯ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.begin(); }
	const_iterator element(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.element(n); }
	const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ database.ಕೊನೆ(); }
};

} /* namespace hashlib */

#ಆದರೆ_ಅಂತ್ಯ
