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

#ଅନ୍ତର୍ଭାଵ <array>
#ଅନ୍ତର୍ଭାଵ <atomic>
#ଅନ୍ତର୍ଭାଵ <stdexcept>
#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <variant>
#ଅନ୍ତର୍ଭାଵ <vector>
#ଅନ୍ତର୍ଭାଵ <type_traits>
#ଅନ୍ତର୍ଭାଵ <stdint.h>

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

#ଯଦି defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#ଅନ୍ୟଥା
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashtable_size_trigger = 2;
ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashtable_size_factor = 3;

namespace legacy {
	ଧାଡ଼ିଭିତରେ uint32_t djb2_add(uint32_t a, uint32_t b) {
		ଫେରନ୍ତୁ ((a << 5) + a) + b;
	}
};

template<typename T>
ସଂରଚନା hash_ops;

ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkhash_xorshift(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା a) {
	ଯଦି (ଆକାର(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} ଅନ୍ୟଥା ଯଦି (ଆକାର(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} ଅନ୍ୟଥା
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ଫେରନ୍ତୁ a;
}

class HasherDJB32 {
ସର୍ବସାଧାରଣ:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ବ୍ୟକ୍ତିଗତ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ସ୍ଥିର uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ଫେରନ୍ତୁ hash;
	}
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ଫେରନ୍ତୁ;
	}
	ଶୂନ୍ୟ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ଫେରନ୍ତୁ;
	}
	[[nodiscard]]
	hash_t yield() ଅଚଳ {
		ଫେରନ୍ତୁ (hash_t)state;
	}

	template<typename T>
	ଶୂନ୍ୟ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	ଶୂନ୍ୟ eat(ଅଚଳ T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	ଶୂନ୍ୟ commutative_eat(hash_t t) {
		state ^= t;
	}

	ଶୂନ୍ୟ force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_ଶୀର୍ଷ_LOOP_FST [[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash
#define HASH_ଶୀର୍ଷ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ଫେରନ୍ତୁ h; \
}

template<typename T>
ସଂରଚନା hash_ops {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ T &a, ଅଚଳ T &b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ T &a, Hasher h) {
		ଯଦି constexpr (std::is_integral_v<T>) {
			static_assert(ଆକାର(T) <= ଆକାର(uint64_t));
			ଯଦି (ଆକାର(T) == ଆକାର(uint64_t))
				h.hash64(a);
			ଅନ୍ୟଥା
				h.hash32(a);
			ଫେରନ୍ତୁ h;
		} ଅନ୍ୟଥା ଯଦି constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ଫେରନ୍ତୁ hash_ops<u_type>::hash_into((u_type) a, h);
		} ଅନ୍ୟଥା ଯଦି constexpr (std::is_pointer_v<T>) {
			ଫେରନ୍ତୁ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} ଅନ୍ୟଥା ଯଦି constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = a.size();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
			ଯେତେବେଳେ (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			ଫେରନ୍ତୁ h;
		} ଅନ୍ୟଥା {
			ଫେରନ୍ତୁ a.hash_into(h);
		}
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ T &a) HASH_ଶୀର୍ଷ_LOOP_SND
};

template<typename P, typename Q> ସଂରଚନା hash_ops<std::pair<P, Q>> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::pair<P, Q> &a, ଅଚଳ std::pair<P, Q> &b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		ଫେରନ୍ତୁ h;
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ std::pair<P, Q> &a) HASH_ଶୀର୍ଷ_LOOP_SND
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash(ଅଚଳ P &p, ଅଚଳ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ଫେରନ୍ତୁ h;
	}
};

template<typename... T> ସଂରଚନା hash_ops<std::tuple<T...>> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::tuple<T...> &a, ଅଚଳ std::tuple<T...> &b) {
		ଫେରନ୍ତୁ a == b;
	}
	template<size_t I = 0>
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ typename std::enable_if<I == ଆକାର...(T), Hasher>::type hash_into(ଅଚଳ std::tuple<T...> &, Hasher h) {
		ଫେରନ୍ତୁ h;
	}
	template<size_t I = 0>
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ typename std::enable_if<I != ଆକାର...(T), Hasher>::type hash_into(ଅଚଳ std::tuple<T...> &a, Hasher h) {
		ପ୍ରକାରବ୍ୟାଖ୍ୟା hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ଫେରନ୍ତୁ h;
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ std::tuple<T...> &a) HASH_ଶୀର୍ଷ_LOOP_SND
};

template<typename T> ସଂରଚନା hash_ops<std::vector<T>> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::vector<T> &a, ଅଚଳ std::vector<T> &b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		ପାଇଁ (auto k : a)
			h.eat(k);
		ଫେରନ୍ତୁ h;
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ std::vector<T> &a) HASH_ଶୀର୍ଷ_LOOP_SND
};

template<typename T, size_t N> ସଂରଚନା hash_ops<std::array<T, N>> {
    ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::array<T, N> &a, ଅଚଳ std::array<T, N> &b) {
        ଫେରନ୍ତୁ a == b;
    }
    [[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ std::array<T, N> &a, Hasher h) {
        ପାଇଁ (ଅଚଳ auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ଫେରନ୍ତୁ h;
    }
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ std::array<T, N> &a) HASH_ଶୀର୍ଷ_LOOP_SND
};

ସଂରଚନା hash_cstr_ops {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
		ଫେରନ୍ତୁ strcmp(a, b) == 0;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ ଅକ୍ଷର *a, Hasher h) {
		ଯେତେବେଳେ (*a)
			h.hash32(*(a++));
		ଫେରନ୍ତୁ h;
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ ଅକ୍ଷର *a) HASH_ଶୀର୍ଷ_LOOP_SND
};

template <> ସଂରଚନା hash_ops<ଅକ୍ଷର*> : hash_cstr_ops {};

ସଂରଚନା hash_ptr_ops {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ ଶୂନ୍ୟ *a, Hasher h) {
		ଫେରନ୍ତୁ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ ଶୂନ୍ୟ *a) HASH_ଶୀର୍ଷ_LOOP_SND
};

ସଂରଚନା hash_obj_ops {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b) {
		ଫେରନ୍ତୁ a == b;
	}
	template<typename T>
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ T *a, Hasher h) {
		ଯଦି (a)
			h = a->hash_into(h);
		ଅନ୍ୟଥା
			h.eat(0);
		ଫେରନ୍ତୁ h;
	}
	template<typename T>
	HASH_ଶୀର୍ଷ_LOOP_FST (ଅଚଳ T *a) HASH_ଶୀର୍ଷ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ଅଚଳ T& obj) {
	ଫେରନ୍ତୁ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkhash(ଅଚଳ T &v) {
	ଫେରନ୍ତୁ (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା) run_hash<T>(v);
}

template<> ସଂରଚନା hash_ops<std::monostate> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(std::monostate a, std::monostate b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(std::monostate, Hasher h) {
		ଫେରନ୍ତୁ h;
	}
};

template<typename... T> ସଂରଚନା hash_ops<std::variant<T...>> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::variant<T...> &a, ଅଚଳ std::variant<T...> &b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ std::variant<T...> &a, Hasher h) {
		std::visit([& h](ଅଚଳ auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		ଫେରନ୍ତୁ h;
	}
};

template<typename T> ସଂରଚନା hash_ops<std::optional<T>> {
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ std::optional<T> &a, ଅଚଳ std::optional<T> &b) {
		ଫେରନ୍ତୁ a == b;
	}
	[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ std::optional<T> &a, Hasher h) {
		ଯଦି(a.has_ମୂଲ୍ୟ())
			h.eat(*a);
		ଅନ୍ୟଥା
			h.eat(0);
		ଫେରନ୍ତୁ h;
	}
};

ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashtable_size(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_size)
{
	// Primes as generated by https://oeis.org/A175953
	ସ୍ଥିର std::vector<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା> zero_and_some_primes = {
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

	ପାଇଁ (auto p : zero_and_some_primes)
		ଯଦି (p >= min_size) ଫେରନ୍ତୁ p;

	ଯଦି (ଆକାର(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	ପାଇଁ (auto p : zero_and_some_primes)
		ଯଦି (100129 * p > min_size) ଫେରନ୍ତୁ 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
ସର୍ବସାଧାରଣ:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	ଶୂନ୍ୟ eat(ଅଚଳ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	ଶୂନ୍ୟ eat(ଅଚଳ Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		ପାଇଁ (auto b : buckets)
			h.eat(b);
		ଫେରନ୍ତୁ h;
	}
ବ୍ୟକ୍ତିଗତ:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	ସଂରଚନା entry_t
	{
		std::pair<K, T> udata;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା next;

		entry_t() { }
		entry_t(ଅଚଳ std::pair<K, T> &udata, ପୂର୍ଣ୍ଣସଂଖ୍ୟା next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, ପୂର୍ଣ୍ଣସଂଖ୍ୟା next) : udata(std::move(udata)), next(next) { }
		bool operator<(ଅଚଳ entry_t &other) ଅଚଳ { ଫେରନ୍ତୁ udata.first < other.udata.first; }
	};

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ do_assert(bool) { }
#ଅନ୍ୟଥା
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ do_assert(bool cond) {
		ଯଦି (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = 0;
		ଯଦି (!hashtable.empty())
			hash = ops.hash(ଚାବି).yield() % (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(hashtable.size());
		ଫେରନ୍ତୁ hash;
	}

	ଶୂନ୍ୟ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_erase(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, Hasher::hash_t hash)
	{
		do_assert(index < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
		ଯଦି (hashtable.empty() || index < 0)
			ଫେରନ୍ତୁ 0;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = hashtable[hash];
		do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));

		ଯଦି (k == index) {
			hashtable[hash] = entries[index].next;
		} ଅନ୍ୟଥା {
			ଯେତେବେଳେ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା back_idx = entries.size()-1;

		ଯଦି (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));

			ଯଦି (k == back_idx) {
				hashtable[back_hash] = index;
			} ଅନ୍ୟଥା {
				ଯେତେବେଳେ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.କାଢ଼_back();

		ଯଦି (entries.empty())
			hashtable.clear();

		ଫେରନ୍ତୁ 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup(ଅଚଳ K &ଚାବି, Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty())
			ଫେରନ୍ତୁ -1;

		ଯଦି (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ଚାବି);
		}

		ଫେରନ୍ତୁ do_lookup_internal(ଚାବି, hash);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup_internal(ଅଚଳ K &ଚାବି, Hasher::hash_t hash) ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = hashtable[hash];

		ଯେତେବେଳେ (index >= 0 && !ops.cmp(entries[index].udata.first, ଚାବି)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
		}

		ଫେରନ୍ତୁ index;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup_no_rehash(ଅଚଳ K &ଚାବି, Hasher::hash_t hash) ଅଚଳ
	{
		ଯଦି (hashtable.empty())
			ଫେରନ୍ତୁ -1;

		ଫେରନ୍ତୁ do_lookup_internal(ଚାବି, hash);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ଭର୍ତ୍ତି(ଅଚଳ K &ଚାବି, ଅଚଳ Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(ଚାବି, T()), -1);
			do_rehash();
		} ଅନ୍ୟଥା {
			entries.emplace_back(std::pair<K, T>(ଚାବି, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ଫେରନ୍ତୁ entries.size() - 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ଭର୍ତ୍ତି(ଅଚଳ std::pair<K, T> &ମୂଲ୍ୟ, ଅଚଳ Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty()) {
			entries.emplace_back(ମୂଲ୍ୟ, -1);
			do_rehash();
		} ଅନ୍ୟଥା {
			entries.emplace_back(ମୂଲ୍ୟ, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ଫେରନ୍ତୁ entries.size() - 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ଭର୍ତ୍ତି(std::pair<K, T> &&rvalue, ଅଚଳ Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} ଅନ୍ୟଥା {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ଫେରନ୍ତୁ entries.size() - 1;
	}

ସର୍ବସାଧାରଣ:
	class const_iterator
	{
		friend class dict;
	ସୁରକ୍ଷିତ:
		ଅଚଳ dict *ptr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
		const_iterator(ଅଚଳ dict *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : ptr(ptr), index(index) { }
	ସର୍ବସାଧାରଣ:
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::bidirectional_iterator_tag iterator_category;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<K, T> ମୂଲ୍ୟ_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅଚଳ std::pair<K, T>* pointer;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅଚଳ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ଫେରନ୍ତୁ *this; }
		const_iterator operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { const_iterator tmp = *this; index--; ଫେରନ୍ତୁ tmp; }
		const_iterator operator--() { index++; ଫେରନ୍ତୁ *this; }
		const_iterator operator--(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { const_iterator tmp = *this; index++; ଫେରନ୍ତୁ tmp; }
		const_iterator operator+=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା amt) { index -= amt; ଫେରନ୍ତୁ *this; }
		bool operator<(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index > other.index; }
		bool operator==(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
		bool operator!=(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
		ଅଚଳ std::pair<K, T> &operator*() ଅଚଳ { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		ଅଚଳ std::pair<K, T> *operator->() ଅଚଳ { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	ସୁରକ୍ଷିତ:
		dict *ptr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
		iterator(dict *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : ptr(ptr), index(index) { }
	ସର୍ବସାଧାରଣ:
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::forward_iterator_tag iterator_category;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<K, T> ମୂଲ୍ୟ_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<K, T>* pointer;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; ଫେରନ୍ତୁ *this; }
		iterator operator+=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା amt) { index -= amt; ଫେରନ୍ତୁ *this; }
		bool operator<(ଅଚଳ iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index > other.index; }
		bool operator==(ଅଚଳ iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
		bool operator!=(ଅଚଳ iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
		std::pair<K, T> &operator*() { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
		ଅଚଳ std::pair<K, T> &operator*() ଅଚଳ { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		ଅଚଳ std::pair<K, T> *operator->() ଅଚଳ { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
		operator const_iterator() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ଅଚଳ {
		ଫେରନ୍ତୁ std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ଅଚଳ {
		ଫେରନ୍ତୁ std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ଅଚଳ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ଅଚଳ dict &other) {
		entries = other.entries;
		do_rehash();
		ଫେରନ୍ତୁ *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		ଫେରନ୍ତୁ *this;
	}

	dict(ଅଚଳ std::initializer_list<std::pair<K, T>> &list)
	{
		ପାଇଁ (auto &it : list)
			ଭର୍ତ୍ତି(it);
	}

	template<class ନିବେଶ_Iterator>
	dict(ନିବେଶ_Iterator first, ନିବେଶ_Iterator last)
	{
		ଭର୍ତ୍ତି(first, last);
	}

	template<class ନିବେଶ_Iterator>
	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ନିବେଶ_Iterator first, ନିବେଶ_Iterator last)
	{
		ପାଇଁ (; first != last; ++first)
			ଭର୍ତ୍ତି(*first);
	}

	std::pair<iterator, bool> ଭର୍ତ୍ତି(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(ଚାବି, hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ଭର୍ତ୍ତି(ଅଚଳ std::pair<K, T> &ମୂଲ୍ୟ)
	{
		Hasher::hash_t hash = do_hash(ମୂଲ୍ୟ.first);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ମୂଲ୍ୟ.first, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(ମୂଲ୍ୟ, hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ଭର୍ତ୍ତି(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(rvalue.first, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::forward<std::pair<K, T>>(rvalue), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ଅଚଳ &ଚାବି, T ଅଚଳ &ମୂଲ୍ୟ)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::make_pair(ଚାବି, ମୂଲ୍ୟ), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ଅଚଳ &ଚାବି, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::make_pair(ଚାବି, std::forward<T>(rvalue)), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ଅଚଳ &ମୂଲ୍ୟ)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(rkey, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::make_pair(std::forward<K>(rkey), ମୂଲ୍ୟ), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(rkey, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା erase(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = do_lookup(ଚାବି, hash);
		ଫେରନ୍ତୁ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		ଫେରନ୍ତୁ ++it;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଫେରନ୍ତୁ i < 0 ? 0 : 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(ଅଚଳ K &ଚାବି, const_iterator it) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଫେରନ୍ତୁ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ end();
		ଫେରନ୍ତୁ iterator(this, i);
	}

	const_iterator find(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ end();
		ଫେରନ୍ତୁ const_iterator(this, i);
	}

	T& at(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i < 0)
			throw std::out_of_range("dict::at()");
		ଫେରନ୍ତୁ entries[i].udata.second;
	}

	ଅଚଳ T& at(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			throw std::out_of_range("dict::at()");
		ଫେରନ୍ତୁ entries[i].udata.second;
	}

	ଅଚଳ T& at(ଅଚଳ K &ଚାବି, ଅଚଳ T &defval) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ defval;
		ଫେରନ୍ତୁ entries[i].udata.second;
	}

	T& operator[](ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i < 0)
			i = do_ଭର୍ତ୍ତି(std::pair<K, T>(ଚାବି, T()), hash);
		ଫେରନ୍ତୁ entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	ଶୂନ୍ୟ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ଅଚଳ entry_t &a, ଅଚଳ entry_t &b){ ଫେରନ୍ତୁ comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	ଶୂନ୍ୟ swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ଅଚଳ dict &other) ଅଚଳ {
		ଯଦି (size() != other.size())
			ଫେରନ୍ତୁ false;
		ପାଇଁ (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			ଯଦି (oit == other.end() || !(oit->second == it.udata.second))
				ଫେରନ୍ତୁ false;
		}
		ଫେରନ୍ତୁ true;
	}

	bool operator!=(ଅଚଳ dict &other) ଅଚଳ {
		ଫେରନ୍ତୁ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		commutative_hash comm;
		ପାଇଁ (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		ଫେରନ୍ତୁ comm.hash_into(h);
	}

	ଶୂନ୍ୟ reserve(size_t n) { entries.reserve(n); }
	size_t size() ଅଚଳ { ଫେରନ୍ତୁ entries.size(); }
	bool empty() ଅଚଳ { ଫେରନ୍ତୁ entries.empty(); }
	ଶୂନ୍ୟ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ଫେରନ୍ତୁ iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1); }
	iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) { ଫେରନ୍ତୁ iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1-n); }
	iterator end() { ଫେରନ୍ତୁ iterator(nullptr, -1); }

	const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1); }
	const_iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ const_iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1-n); }
	const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, typename> friend class idict;

ସୁରକ୍ଷିତ:
	ସଂରଚନା entry_t
	{
		K udata;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା next;

		entry_t() { }
		entry_t(ଅଚଳ K &udata, ପୂର୍ଣ୍ଣସଂଖ୍ୟା next) : udata(udata), next(next) { }
		entry_t(K &&udata, ପୂର୍ଣ୍ଣସଂଖ୍ୟା next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ do_assert(bool) { }
#ଅନ୍ୟଥା
	ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ do_assert(bool cond) {
		ଯଦି (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = 0;
		ଯଦି (!hashtable.empty())
			hash = ops.hash(ଚାବି).yield() % (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(hashtable.size());
		ଫେରନ୍ତୁ hash;
	}

	ଶୂନ୍ୟ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_erase(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, Hasher::hash_t hash)
	{
		do_assert(index < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
		ଯଦି (hashtable.empty() || index < 0)
			ଫେରନ୍ତୁ 0;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = hashtable[hash];
		ଯଦି (k == index) {
			hashtable[hash] = entries[index].next;
		} ଅନ୍ୟଥା {
			ଯେତେବେଳେ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା back_idx = entries.size()-1;

		ଯଦି (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			ଯଦି (k == back_idx) {
				hashtable[back_hash] = index;
			} ଅନ୍ୟଥା {
				ଯେତେବେଳେ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.କାଢ଼_back();

		ଯଦି (entries.empty())
			hashtable.clear();

		ଫେରନ୍ତୁ 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup(ଅଚଳ K &ଚାବି, Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty())
			ଫେରନ୍ତୁ -1;

		ଯଦି (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ଚାବି);
		}

		ଫେରନ୍ତୁ do_lookup_internal(ଚାବି, hash);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup_internal(ଅଚଳ K &ଚାବି, Hasher::hash_t hash) ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = hashtable[hash];

		ଯେତେବେଳେ (index >= 0 && !ops.cmp(entries[index].udata, ଚାବି)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size()));
		}

		ଫେରନ୍ତୁ index;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lookup_no_rehash(ଅଚଳ K &ଚାବି, Hasher::hash_t hash) ଅଚଳ
	{
		ଯଦି (hashtable.empty())
			ଫେରନ୍ତୁ -1;

		ଫେରନ୍ତୁ do_lookup_internal(ଚାବି, hash);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ଭର୍ତ୍ତି(ଅଚଳ K &ମୂଲ୍ୟ, Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty()) {
			entries.emplace_back(ମୂଲ୍ୟ, -1);
			do_rehash();
			hash = do_hash(ମୂଲ୍ୟ);
		} ଅନ୍ୟଥା {
			entries.emplace_back(ମୂଲ୍ୟ, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ଫେରନ୍ତୁ entries.size() - 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ଭର୍ତ୍ତି(K &&rvalue, Hasher::hash_t &hash)
	{
		ଯଦି (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} ଅନ୍ୟଥା {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ଫେରନ୍ତୁ entries.size() - 1;
	}

ସର୍ବସାଧାରଣ:
	class const_iterator
	{
		friend class pool;
	ସୁରକ୍ଷିତ:
		ଅଚଳ pool *ptr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
		const_iterator(ଅଚଳ pool *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : ptr(ptr), index(index) { }
	ସର୍ବସାଧାରଣ:
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::forward_iterator_tag iterator_category;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K ମୂଲ୍ୟ_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K* pointer;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ଫେରନ୍ତୁ *this; }
		bool operator==(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
		bool operator!=(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
		ଅଚଳ K &operator*() ଅଚଳ { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		ଅଚଳ K *operator->() ଅଚଳ { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	ସୁରକ୍ଷିତ:
		pool *ptr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
		iterator(pool *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : ptr(ptr), index(index) { }
	ସର୍ବସାଧାରଣ:
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::forward_iterator_tag iterator_category;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K ମୂଲ୍ୟ_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K* pointer;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K& reference;
		iterator() { }
		iterator operator++() { index--; ଫେରନ୍ତୁ *this; }
		bool operator==(ଅଚଳ iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
		bool operator!=(ଅଚଳ iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
		K &operator*() { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		K *operator->() { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
		ଅଚଳ K &operator*() ଅଚଳ { ଫେରନ୍ତୁ ptr->entries[index].udata; }
		ଅଚଳ K *operator->() ଅଚଳ { ଫେରନ୍ତୁ &ptr->entries[index].udata; }
		operator const_iterator() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(ଅଚଳ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ଅଚଳ pool &other) {
		entries = other.entries;
		do_rehash();
		ଫେରନ୍ତୁ *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		ଫେରନ୍ତୁ *this;
	}

	pool(ଅଚଳ std::initializer_list<K> &list)
	{
		ପାଇଁ (auto &it : list)
			ଭର୍ତ୍ତି(it);
	}

	template<class ନିବେଶ_Iterator>
	pool(ନିବେଶ_Iterator first, ନିବେଶ_Iterator last)
	{
		ଭର୍ତ୍ତି(first, last);
	}

	template<class ନିବେଶ_Iterator>
	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ନିବେଶ_Iterator first, ନିବେଶ_Iterator last)
	{
		ପାଇଁ (; first != last; ++first)
			ଭର୍ତ୍ତି(*first);
	}

	std::pair<iterator, bool> ଭର୍ତ୍ତି(ଅଚଳ K &ମୂଲ୍ୟ)
	{
		Hasher::hash_t hash = do_hash(ମୂଲ୍ୟ);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ମୂଲ୍ୟ, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(ମୂଲ୍ୟ, hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ଭର୍ତ୍ତି(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(rvalue, hash);
		ଯଦି (i >= 0)
			ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ଭର୍ତ୍ତି(std::forward<K>(rvalue), hash);
		ଫେରନ୍ତୁ std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		ଫେରନ୍ତୁ ଭର୍ତ୍ତି(K(std::forward<Args>(args)...));
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା erase(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = do_lookup(ଚାବି, hash);
		ଫେରନ୍ତୁ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		ଫେରନ୍ତୁ ++it;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଫେରନ୍ତୁ i < 0 ? 0 : 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(ଅଚଳ K &ଚାବି, const_iterator it) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଫେରନ୍ତୁ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ end();
		ଫେରନ୍ତୁ iterator(this, i);
	}

	const_iterator find(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ end();
		ଫେରନ୍ତୁ const_iterator(this, i);
	}

	bool operator[](ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = do_lookup(ଚାବି, hash);
		ଫେରନ୍ତୁ i >= 0;
	}

	template<typename Compare = std::less<K>>
	ଶୂନ୍ୟ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ଅଚଳ entry_t &a, ଅଚଳ entry_t &b){ ଫେରନ୍ତୁ comp(b.udata, a.udata); });
		do_rehash();
	}

	K କାଢ଼()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ଫେରନ୍ତୁ ret;
	}

	ଶୂନ୍ୟ swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ଅଚଳ pool &other) ଅଚଳ {
		ଯଦି (size() != other.size())
			ଫେରନ୍ତୁ false;
		ପାଇଁ (auto &it : entries)
			ଯଦି (!other.ଗଣନା_2(it.udata))
				ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ true;
	}

	bool operator!=(ଅଚଳ pool &other) ଅଚଳ {
		ଫେରନ୍ତୁ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		commutative_hash comm;
		ପାଇଁ (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ଫେରନ୍ତୁ comm.hash_into(h);
	}

	ଶୂନ୍ୟ reserve(size_t n) { entries.reserve(n); }
	size_t size() ଅଚଳ { ଫେରନ୍ତୁ entries.size(); }
	bool empty() ଅଚଳ { ଫେରନ୍ତୁ entries.empty(); }
	ଶୂନ୍ୟ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ଫେରନ୍ତୁ iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1); }
	iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) { ଫେରନ୍ତୁ iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1-n); }
	iterator end() { ଫେରନ୍ତୁ iterator(nullptr, -1); }

	const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1); }
	const_iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ const_iterator(this, ପୂର୍ଣ୍ଣସଂଖ୍ୟା(entries.size())-1-n); }
	const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(nullptr, -1); }
};

template<typename K, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

ସର୍ବସାଧାରଣ:
	class const_iterator
	{
		friend class idict;
	ସୁରକ୍ଷିତ:
		ଅଚଳ idict &container;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
		const_iterator(ଅଚଳ idict &container, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : container(container), index(index) { }
	ସର୍ବସାଧାରଣ:
		ପ୍ରକାରବ୍ୟାଖ୍ୟା std::forward_iterator_tag iterator_category;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K ମୂଲ୍ୟ_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K* pointer;
		ପ୍ରକାରବ୍ୟାଖ୍ୟା K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; ଫେରନ୍ତୁ *this; }
		bool operator==(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
		bool operator!=(ଅଚଳ const_iterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
		ଅଚଳ K &operator*() ଅଚଳ { ଫେରନ୍ତୁ container[index]; }
		ଅଚଳ K *operator->() ଅଚଳ { ଫେରନ୍ତୁ &container[index]; }
	};

	constexpr idict()
	{
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator()(ଅଚଳ K &ଚାବି)
	{
		Hasher::hash_t hash = database.do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.do_lookup(ଚାବି, hash);
		ଯଦି (i < 0)
			i = database.do_ଭର୍ତ୍ତି(ଚାବି, hash);
		ଫେରନ୍ତୁ i + offset;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା at(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = database.do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			throw std::out_of_range("idict::at()");
		ଫେରନ୍ତୁ i + offset;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା at(ଅଚଳ K &ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା defval) ଅଚଳ
	{
		Hasher::hash_t hash = database.do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.do_lookup_no_rehash(ଚାବି, hash);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ defval;
		ଫେରନ୍ତୁ i + offset;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = database.do_hash(ଚାବି);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.do_lookup_no_rehash(ଚାବି, hash);
		ଫେରନ୍ତୁ i < 0 ? 0 : 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup(ଅଚଳ K &ଚାବି) ଅଚଳ
	{
		Hasher::hash_t hash = database.do_hash(ଚାବି);
		ଫେରନ୍ତୁ database.do_lookup_no_rehash(ଚାବି, hash);
	}

	ଶୂନ୍ୟ expect(ଅଚଳ K &ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = (*this)(ଚାବି);
		ଯଦି (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ଅଚଳ K &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ
	{
		ଫେରନ୍ତୁ database.entries.at(index - offset).udata;
	}

	ଶୂନ୍ୟ swap(idict &other)
	{
		database.swap(other.database);
	}

	ଶୂନ୍ୟ reserve(size_t n) { database.reserve(n); }
	size_t size() ଅଚଳ { ଫେରନ୍ତୁ database.size(); }
	bool empty() ଅଚଳ { ଫେରନ୍ତୁ database.empty(); }
	ଶୂନ୍ୟ clear() { database.clear(); }

	const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(*this, offset); }
	const_iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ const_iterator(*this, n); }
	const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(*this, offset + size()); }
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
	ସର୍ବସାଧାରଣ:
		explicit AtomicParent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା p) : parent(p) {}
		AtomicParent(ଅଚଳ AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ଅଚଳ AtomicParent &other) { set(other.get()); ଫେରନ୍ତୁ *this; }
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା get() ଅଚଳ { ଫେରନ୍ତୁ parent.load(std::memory_order_relaxed); }
		ଶୂନ୍ୟ set(ପୂର୍ଣ୍ଣସଂଖ୍ୟା p) { parent.store(p, std::memory_order_relaxed); }
	ବ୍ୟକ୍ତିଗତ:
		std::atomic<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> parent;
	};
	std::vector<AtomicParent> parents;

ସର୍ବସାଧାରଣ:
	ପ୍ରକାରବ୍ୟାଖ୍ୟା typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator()(ଅଚଳ K &ଚାବି)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database(ଚାବି);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		ଯଦି (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		ଫେରନ୍ତୁ i;
	}

	// Finds an element at given index
	ଅଚଳ K &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ
	{
		ଫେରନ୍ତୁ database[index];
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifind(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା p = i, k = i;

		ଯେତେବେଳେ (true) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା pp = parents[p].get();
			ଯଦି (pp < 0)
				ଭାଙ୍ଗ;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		ଯେତେବେଳେ (k != p) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		ଫେରନ୍ତୁ p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	ଶୂନ୍ୟ imerge(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ପୂର୍ଣ୍ଣସଂଖ୍ୟା j)
	{
		i = ifind(i);
		j = ifind(j);

		ଯଦି (i != j)
			parents[i].set(j);
	}

	ଶୂନ୍ୟ ipromote(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = i;

		ଯେତେବେଳେ (k != -1) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup(ଅଚଳ K &a)
	{
		ଫେରନ୍ତୁ ifind((*this)(a));
	}

	ଅଚଳ K &find(ଅଚଳ K &a) ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.at(a, -1);
		ଯଦି (i < 0)
			ଫେରନ୍ତୁ a;
		ଫେରନ୍ତୁ (*this)[ifind(i)];
	}

	ଶୂନ୍ୟ merge(ଅଚଳ K &a, ଅଚଳ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	ଶୂନ୍ୟ promote(ଅଚଳ K &a)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = database.at(a, -1);
		ଯଦି (i >= 0)
			ipromote(i);
	}

	ଶୂନ୍ୟ swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	ଶୂନ୍ୟ reserve(size_t n) { database.reserve(n); }
	size_t size() ଅଚଳ { ଫେରନ୍ତୁ database.size(); }
	bool empty() ଅଚଳ { ଫେରନ୍ତୁ database.empty(); }
	ଶୂନ୍ୟ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ database.begin(); }
	const_iterator element(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ database.element(n); }
	const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ database.end(); }
};

} /* namespace hashlib */

#endif
