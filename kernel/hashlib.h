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

#অন্তর্ভাৱ <array>
#অন্তর্ভাৱ <atomic>
#অন্তর্ভাৱ <stdexcept>
#অন্তর্ভাৱ <algorithm>
#অন্তর্ভাৱ <optional>
#অন্তর্ভাৱ <string>
#অন্তর্ভাৱ <variant>
#অন্তর্ভাৱ <vector>
#অন্তর্ভাৱ <type_traits>
#অন্তর্ভাৱ <stdint.h>

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

ধ্রুৱ পূর্ণাঙ্ক hashtable_size_trigger = 2;
ধ্রুৱ পূর্ণাঙ্ক hashtable_size_factor = 3;

namespace legacy {
	অন্তঃস্থ uint32_t djb2_add(uint32_t a, uint32_t b) {
		ঘূৰাই ((a << 5) + a) + b;
	}
};

template<typename T>
সংরচনা hash_ops;

অন্তঃস্থ unsigned পূর্ণাঙ্ক mkhash_xorshift(unsigned পূর্ণাঙ্ক a) {
	যদি (পরিমাণ(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} অন্যথা যদি (পরিমাণ(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} অন্যথা
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ঘূৰাই a;
}

class HasherDJB32 {
সার্ৱজনিক:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

গুপ্ত:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	স্থির uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ঘূৰাই hash;
	}
সার্ৱজনিক:
	শূন্য hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ঘূৰাই;
	}
	শূন্য hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ঘূৰাই;
	}
	[[nodiscard]]
	hash_t yield() ধ্রুৱ {
		ঘূৰাই (hash_t)state;
	}

	template<typename T>
	শূন্য eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	শূন্য eat(ধ্রুৱ T& t) {
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
#define HASH_শীৰ্ষ_LOOP_FST [[nodiscard]] স্থির অন্তঃস্থ Hasher hash
#define HASH_শীৰ্ষ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ঘূৰাই h; \
}

template<typename T>
সংরচনা hash_ops {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ T &a, ধ্রুৱ T &b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ T &a, Hasher h) {
		যদি constexpr (std::is_integral_v<T>) {
			static_assert(পরিমাণ(T) <= পরিমাণ(uint64_t));
			যদি (পরিমাণ(T) == পরিমাণ(uint64_t))
				h.hash64(a);
			অন্যথা
				h.hash32(a);
			ঘূৰাই h;
		} অন্যথা যদি constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ঘূৰাই hash_ops<u_type>::hash_into((u_type) a, h);
		} অন্যথা যদি constexpr (std::is_pointer_v<T>) {
			ঘূৰাই hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} অন্যথা যদি constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			পূর্ণাঙ্ক size = a.size();
			পূর্ণাঙ্ক i = 0;
			যেতিয়ালৈ (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			ঘূৰাই h;
		} অন্যথা {
			ঘূৰাই a.hash_into(h);
		}
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ T &a) HASH_শীৰ্ষ_LOOP_SND
};

template<typename P, typename Q> সংরচনা hash_ops<std::pair<P, Q>> {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::pair<P, Q> &a, ধ্রুৱ std::pair<P, Q> &b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		ঘূৰাই h;
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ std::pair<P, Q> &a) HASH_শীৰ্ষ_LOOP_SND
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash(ধ্রুৱ P &p, ধ্রুৱ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ঘূৰাই h;
	}
};

template<typename... T> সংরচনা hash_ops<std::tuple<T...>> {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::tuple<T...> &a, ধ্রুৱ std::tuple<T...> &b) {
		ঘূৰাই a == b;
	}
	template<size_t I = 0>
	স্থির অন্তঃস্থ typename std::enable_if<I == পরিমাণ...(T), Hasher>::type hash_into(ধ্রুৱ std::tuple<T...> &, Hasher h) {
		ঘূৰাই h;
	}
	template<size_t I = 0>
	স্থির অন্তঃস্থ typename std::enable_if<I != পরিমাণ...(T), Hasher>::type hash_into(ধ্রুৱ std::tuple<T...> &a, Hasher h) {
		প্রকারনির্দেশ hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ঘূৰাই h;
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ std::tuple<T...> &a) HASH_শীৰ্ষ_LOOP_SND
};

template<typename T> সংরচনা hash_ops<std::vector<T>> {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::vector<T> &a, ধ্রুৱ std::vector<T> &b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		বাবে (auto k : a)
			h.eat(k);
		ঘূৰাই h;
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ std::vector<T> &a) HASH_শীৰ্ষ_LOOP_SND
};

template<typename T, size_t N> সংরচনা hash_ops<std::array<T, N>> {
    স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::array<T, N> &a, ধ্রুৱ std::array<T, N> &b) {
        ঘূৰাই a == b;
    }
    [[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ std::array<T, N> &a, Hasher h) {
        বাবে (ধ্রুৱ auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ঘূৰাই h;
    }
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ std::array<T, N> &a) HASH_শীৰ্ষ_LOOP_SND
};

সংরচনা hash_cstr_ops {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) {
		ঘূৰাই strcmp(a, b) == 0;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ ৱর্ণ *a, Hasher h) {
		যেতিয়ালৈ (*a)
			h.hash32(*(a++));
		ঘূৰাই h;
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ ৱর্ণ *a) HASH_শীৰ্ষ_LOOP_SND
};

template <> সংরচনা hash_ops<ৱর্ণ*> : hash_cstr_ops {};

সংরচনা hash_ptr_ops {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ শূন্য *a, Hasher h) {
		ঘূৰাই hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ শূন্য *a) HASH_শীৰ্ষ_LOOP_SND
};

সংরচনা hash_obj_ops {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b) {
		ঘূৰাই a == b;
	}
	template<typename T>
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ T *a, Hasher h) {
		যদি (a)
			h = a->hash_into(h);
		অন্যথা
			h.eat(0);
		ঘূৰাই h;
	}
	template<typename T>
	HASH_শীৰ্ষ_LOOP_FST (ধ্রুৱ T *a) HASH_শীৰ্ষ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ধ্রুৱ T& obj) {
	ঘূৰাই hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
অন্তঃস্থ unsigned পূর্ণাঙ্ক mkhash(ধ্রুৱ T &v) {
	ঘূৰাই (unsigned পূর্ণাঙ্ক) run_hash<T>(v);
}

template<> সংরচনা hash_ops<std::monostate> {
	স্থির অন্তঃস্থ bool cmp(std::monostate a, std::monostate b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(std::monostate, Hasher h) {
		ঘূৰাই h;
	}
};

template<typename... T> সংরচনা hash_ops<std::variant<T...>> {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::variant<T...> &a, ধ্রুৱ std::variant<T...> &b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ std::variant<T...> &a, Hasher h) {
		std::visit([& h](ধ্রুৱ auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		ঘূৰাই h;
	}
};

template<typename T> সংরচনা hash_ops<std::optional<T>> {
	স্থির অন্তঃস্থ bool cmp(ধ্রুৱ std::optional<T> &a, ধ্রুৱ std::optional<T> &b) {
		ঘূৰাই a == b;
	}
	[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ std::optional<T> &a, Hasher h) {
		যদি(a.has_মান())
			h.eat(*a);
		অন্যথা
			h.eat(0);
		ঘূৰাই h;
	}
};

অন্তঃস্থ unsigned পূর্ণাঙ্ক hashtable_size(unsigned পূর্ণাঙ্ক min_size)
{
	// Primes as generated by https://oeis.org/A175953
	স্থির std::vector<unsigned পূর্ণাঙ্ক> zero_and_some_primes = {
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

	বাবে (auto p : zero_and_some_primes)
		যদি (p >= min_size) ঘূৰাই p;

	যদি (পরিমাণ(unsigned পূর্ণাঙ্ক) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	বাবে (auto p : zero_and_some_primes)
		যদি (100129 * p > min_size) ঘূৰাই 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, পূর্ণাঙ্ক offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
সার্ৱজনিক:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	শূন্য eat(ধ্রুৱ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	শূন্য eat(ধ্রুৱ Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
		বাবে (auto b : buckets)
			h.eat(b);
		ঘূৰাই h;
	}
গুপ্ত:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	সংরচনা entry_t
	{
		std::pair<K, T> udata;
		পূর্ণাঙ্ক next;

		entry_t() { }
		entry_t(ধ্রুৱ std::pair<K, T> &udata, পূর্ণাঙ্ক next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, পূর্ণাঙ্ক next) : udata(std::move(udata)), next(next) { }
		bool operator<(ধ্রুৱ entry_t &other) ধ্রুৱ { ঘূৰাই udata.first < other.udata.first; }
	};

	std::vector<পূর্ণাঙ্ক> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	স্থির অন্তঃস্থ শূন্য do_assert(bool) { }
#অন্যথা
	স্থির অন্তঃস্থ শূন্য do_assert(bool cond) {
		যদি (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = 0;
		যদি (!hashtable.empty())
			hash = ops.hash(চাবি).yield() % (unsigned পূর্ণাঙ্ক)(hashtable.size());
		ঘূৰাই hash;
	}

	শূন্য do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		বাবে (পূর্ণাঙ্ক i = 0; i < পূর্ণাঙ্ক(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < পূর্ণাঙ্ক(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	পূর্ণাঙ্ক do_erase(পূর্ণাঙ্ক index, Hasher::hash_t hash)
	{
		do_assert(index < পূর্ণাঙ্ক(entries.size()));
		যদি (hashtable.empty() || index < 0)
			ঘূৰাই 0;

		পূর্ণাঙ্ক k = hashtable[hash];
		do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));

		যদি (k == index) {
			hashtable[hash] = entries[index].next;
		} অন্যথা {
			যেতিয়ালৈ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		পূর্ণাঙ্ক back_idx = entries.size()-1;

		যদি (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));

			যদি (k == back_idx) {
				hashtable[back_hash] = index;
			} অন্যথা {
				যেতিয়ালৈ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.বেৰ_কৰো_back();

		যদি (entries.empty())
			hashtable.clear();

		ঘূৰাই 1;
	}

	পূর্ণাঙ্ক do_lookup(ধ্রুৱ K &চাবি, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty())
			ঘূৰাই -1;

		যদি (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(চাবি);
		}

		ঘূৰাই do_lookup_internal(চাবি, hash);
	}

	পূর্ণাঙ্ক do_lookup_internal(ধ্রুৱ K &চাবি, Hasher::hash_t hash) ধ্রুৱ
	{
		পূর্ণাঙ্ক index = hashtable[hash];

		যেতিয়ালৈ (index >= 0 && !ops.cmp(entries[index].udata.first, চাবি)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < পূর্ণাঙ্ক(entries.size()));
		}

		ঘূৰাই index;
	}

	পূর্ণাঙ্ক do_lookup_no_rehash(ধ্রুৱ K &চাবি, Hasher::hash_t hash) ধ্রুৱ
	{
		যদি (hashtable.empty())
			ঘূৰাই -1;

		ঘূৰাই do_lookup_internal(চাবি, hash);
	}

	পূর্ণাঙ্ক do_ঢোকাও(ধ্রুৱ K &চাবি, ধ্রুৱ Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(চাবি, T()), -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(std::pair<K, T>(চাবি, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ঘূৰাই entries.size() - 1;
	}

	পূর্ণাঙ্ক do_ঢোকাও(ধ্রুৱ std::pair<K, T> &মান, ধ্রুৱ Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(মান, -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(মান, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ঘূৰাই entries.size() - 1;
	}

	পূর্ণাঙ্ক do_ঢোকাও(std::pair<K, T> &&rvalue, ধ্রুৱ Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} অন্যথা {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ঘূৰাই entries.size() - 1;
	}

সার্ৱজনিক:
	class const_iterator
	{
		friend class dict;
	রক্ষিত:
		ধ্রুৱ dict *ptr;
		পূর্ণাঙ্ক index;
		const_iterator(ধ্রুৱ dict *ptr, পূর্ণাঙ্ক index) : ptr(ptr), index(index) { }
	সার্ৱজনিক:
		প্রকারনির্দেশ std::bidirectional_iterator_tag iterator_category;
		প্রকারনির্দেশ std::pair<K, T> মান_type;
		প্রকারনির্দেশ ptrdiff_t difference_type;
		প্রকারনির্দেশ ধ্রুৱ std::pair<K, T>* pointer;
		প্রকারনির্দেশ ধ্রুৱ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ঘূৰাই *this; }
		const_iterator operator++(পূর্ণাঙ্ক) { const_iterator tmp = *this; index--; ঘূৰাই tmp; }
		const_iterator operator--() { index++; ঘূৰাই *this; }
		const_iterator operator--(পূর্ণাঙ্ক) { const_iterator tmp = *this; index++; ঘূৰাই tmp; }
		const_iterator operator+=(পূর্ণাঙ্ক amt) { index -= amt; ঘূৰাই *this; }
		bool operator<(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index > other.index; }
		bool operator==(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index == other.index; }
		bool operator!=(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index != other.index; }
		ধ্রুৱ std::pair<K, T> &operator*() ধ্রুৱ { ঘূৰাই ptr->entries[index].udata; }
		ধ্রুৱ std::pair<K, T> *operator->() ধ্রুৱ { ঘূৰাই &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	রক্ষিত:
		dict *ptr;
		পূর্ণাঙ্ক index;
		iterator(dict *ptr, পূর্ণাঙ্ক index) : ptr(ptr), index(index) { }
	সার্ৱজনিক:
		প্রকারনির্দেশ std::forward_iterator_tag iterator_category;
		প্রকারনির্দেশ std::pair<K, T> মান_type;
		প্রকারনির্দেশ ptrdiff_t difference_type;
		প্রকারনির্দেশ std::pair<K, T>* pointer;
		প্রকারনির্দেশ std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; ঘূৰাই *this; }
		iterator operator+=(পূর্ণাঙ্ক amt) { index -= amt; ঘূৰাই *this; }
		bool operator<(ধ্রুৱ iterator &other) ধ্রুৱ { ঘূৰাই index > other.index; }
		bool operator==(ধ্রুৱ iterator &other) ধ্রুৱ { ঘূৰাই index == other.index; }
		bool operator!=(ধ্রুৱ iterator &other) ধ্রুৱ { ঘূৰাই index != other.index; }
		std::pair<K, T> &operator*() { ঘূৰাই ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { ঘূৰাই &ptr->entries[index].udata; }
		ধ্রুৱ std::pair<K, T> &operator*() ধ্রুৱ { ঘূৰাই ptr->entries[index].udata; }
		ধ্রুৱ std::pair<K, T> *operator->() ধ্রুৱ { ঘূৰাই &ptr->entries[index].udata; }
		operator const_iterator() ধ্রুৱ { ঘূৰাই const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ধ্রুৱ {
		ঘূৰাই std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ধ্রুৱ {
		ঘূৰাই std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ধ্রুৱ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ধ্রুৱ dict &other) {
		entries = other.entries;
		do_rehash();
		ঘূৰাই *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		ঘূৰাই *this;
	}

	dict(ধ্রুৱ std::initializer_list<std::pair<K, T>> &list)
	{
		বাবে (auto &it : list)
			ঢোকাও(it);
	}

	template<class প্ৰবেশ_Iterator>
	dict(প্ৰবেশ_Iterator first, প্ৰবেশ_Iterator last)
	{
		ঢোকাও(first, last);
	}

	template<class প্ৰবেশ_Iterator>
	শূন্য ঢোকাও(প্ৰবেশ_Iterator first, প্ৰবেশ_Iterator last)
	{
		বাবে (; first != last; ++first)
			ঢোকাও(*first);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(চাবি, hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুৱ std::pair<K, T> &মান)
	{
		Hasher::hash_t hash = do_hash(মান.first);
		পূর্ণাঙ্ক i = do_lookup(মান.first, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(মান, hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		পূর্ণাঙ্ক i = do_lookup(rvalue.first, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::forward<std::pair<K, T>>(rvalue), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ধ্রুৱ &চাবি, T ধ্রুৱ &মান)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(চাবি, মান), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ধ্রুৱ &চাবি, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(চাবি, std::forward<T>(rvalue)), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ধ্রুৱ &মান)
	{
		Hasher::hash_t hash = do_hash(rkey);
		পূর্ণাঙ্ক i = do_lookup(rkey, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(std::forward<K>(rkey), মান), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		পূর্ণাঙ্ক i = do_lookup(rkey, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	পূর্ণাঙ্ক erase(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক index = do_lookup(চাবি, hash);
		ঘূৰাই do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		ঘূৰাই ++it;
	}

	পূর্ণাঙ্ক গুনতি(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		ঘূৰাই i < 0 ? 0 : 1;
	}

	পূর্ণাঙ্ক গুনতি(ধ্রুৱ K &চাবি, const_iterator it) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		ঘূৰাই i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই end();
		ঘূৰাই iterator(this, i);
	}

	const_iterator find(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই end();
		ঘূৰাই const_iterator(this, i);
	}

	T& at(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("dict::at()");
		ঘূৰাই entries[i].udata.second;
	}

	ধ্রুৱ T& at(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("dict::at()");
		ঘূৰাই entries[i].udata.second;
	}

	ধ্রুৱ T& at(ধ্রুৱ K &চাবি, ধ্রুৱ T &defval) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই defval;
		ঘূৰাই entries[i].udata.second;
	}

	T& operator[](ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i < 0)
			i = do_ঢোকাও(std::pair<K, T>(চাবি, T()), hash);
		ঘূৰাই entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	শূন্য sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ধ্রুৱ entry_t &a, ধ্রুৱ entry_t &b){ ঘূৰাই comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	শূন্য swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ধ্রুৱ dict &other) ধ্রুৱ {
		যদি (size() != other.size())
			ঘূৰাই false;
		বাবে (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			যদি (oit == other.end() || !(oit->second == it.udata.second))
				ঘূৰাই false;
		}
		ঘূৰাই true;
	}

	bool operator!=(ধ্রুৱ dict &other) ধ্রুৱ {
		ঘূৰাই !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
		commutative_hash comm;
		বাবে (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		ঘূৰাই comm.hash_into(h);
	}

	শূন্য reserve(size_t n) { entries.reserve(n); }
	size_t size() ধ্রুৱ { ঘূৰাই entries.size(); }
	bool empty() ধ্রুৱ { ঘূৰাই entries.empty(); }
	শূন্য clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ঘূৰাই iterator(this, পূর্ণাঙ্ক(entries.size())-1); }
	iterator element(পূর্ণাঙ্ক n) { ঘূৰাই iterator(this, পূর্ণাঙ্ক(entries.size())-1-n); }
	iterator end() { ঘূৰাই iterator(nullptr, -1); }

	const_iterator begin() ধ্রুৱ { ঘূৰাই const_iterator(this, পূর্ণাঙ্ক(entries.size())-1); }
	const_iterator element(পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই const_iterator(this, পূর্ণাঙ্ক(entries.size())-1-n); }
	const_iterator end() ধ্রুৱ { ঘূৰাই const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, পূর্ণাঙ্ক, typename> friend class idict;

রক্ষিত:
	সংরচনা entry_t
	{
		K udata;
		পূর্ণাঙ্ক next;

		entry_t() { }
		entry_t(ধ্রুৱ K &udata, পূর্ণাঙ্ক next) : udata(udata), next(next) { }
		entry_t(K &&udata, পূর্ণাঙ্ক next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<পূর্ণাঙ্ক> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	স্থির অন্তঃস্থ শূন্য do_assert(bool) { }
#অন্যথা
	স্থির অন্তঃস্থ শূন্য do_assert(bool cond) {
		যদি (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = 0;
		যদি (!hashtable.empty())
			hash = ops.hash(চাবি).yield() % (unsigned পূর্ণাঙ্ক)(hashtable.size());
		ঘূৰাই hash;
	}

	শূন্য do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		বাবে (পূর্ণাঙ্ক i = 0; i < পূর্ণাঙ্ক(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < পূর্ণাঙ্ক(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	পূর্ণাঙ্ক do_erase(পূর্ণাঙ্ক index, Hasher::hash_t hash)
	{
		do_assert(index < পূর্ণাঙ্ক(entries.size()));
		যদি (hashtable.empty() || index < 0)
			ঘূৰাই 0;

		পূর্ণাঙ্ক k = hashtable[hash];
		যদি (k == index) {
			hashtable[hash] = entries[index].next;
		} অন্যথা {
			যেতিয়ালৈ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		পূর্ণাঙ্ক back_idx = entries.size()-1;

		যদি (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			যদি (k == back_idx) {
				hashtable[back_hash] = index;
			} অন্যথা {
				যেতিয়ালৈ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < পূর্ণাঙ্ক(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.বেৰ_কৰো_back();

		যদি (entries.empty())
			hashtable.clear();

		ঘূৰাই 1;
	}

	পূর্ণাঙ্ক do_lookup(ধ্রুৱ K &চাবি, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty())
			ঘূৰাই -1;

		যদি (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(চাবি);
		}

		ঘূৰাই do_lookup_internal(চাবি, hash);
	}

	পূর্ণাঙ্ক do_lookup_internal(ধ্রুৱ K &চাবি, Hasher::hash_t hash) ধ্রুৱ
	{
		পূর্ণাঙ্ক index = hashtable[hash];

		যেতিয়ালৈ (index >= 0 && !ops.cmp(entries[index].udata, চাবি)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < পূর্ণাঙ্ক(entries.size()));
		}

		ঘূৰাই index;
	}

	পূর্ণাঙ্ক do_lookup_no_rehash(ধ্রুৱ K &চাবি, Hasher::hash_t hash) ধ্রুৱ
	{
		যদি (hashtable.empty())
			ঘূৰাই -1;

		ঘূৰাই do_lookup_internal(চাবি, hash);
	}

	পূর্ণাঙ্ক do_ঢোকাও(ধ্রুৱ K &মান, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(মান, -1);
			do_rehash();
			hash = do_hash(মান);
		} অন্যথা {
			entries.emplace_back(মান, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ঘূৰাই entries.size() - 1;
	}

	পূর্ণাঙ্ক do_ঢোকাও(K &&rvalue, Hasher::hash_t &hash)
	{
		যদি (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} অন্যথা {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ঘূৰাই entries.size() - 1;
	}

সার্ৱজনিক:
	class const_iterator
	{
		friend class pool;
	রক্ষিত:
		ধ্রুৱ pool *ptr;
		পূর্ণাঙ্ক index;
		const_iterator(ধ্রুৱ pool *ptr, পূর্ণাঙ্ক index) : ptr(ptr), index(index) { }
	সার্ৱজনিক:
		প্রকারনির্দেশ std::forward_iterator_tag iterator_category;
		প্রকারনির্দেশ K মান_type;
		প্রকারনির্দেশ ptrdiff_t difference_type;
		প্রকারনির্দেশ K* pointer;
		প্রকারনির্দেশ K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ঘূৰাই *this; }
		bool operator==(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index == other.index; }
		bool operator!=(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index != other.index; }
		ধ্রুৱ K &operator*() ধ্রুৱ { ঘূৰাই ptr->entries[index].udata; }
		ধ্রুৱ K *operator->() ধ্রুৱ { ঘূৰাই &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	রক্ষিত:
		pool *ptr;
		পূর্ণাঙ্ক index;
		iterator(pool *ptr, পূর্ণাঙ্ক index) : ptr(ptr), index(index) { }
	সার্ৱজনিক:
		প্রকারনির্দেশ std::forward_iterator_tag iterator_category;
		প্রকারনির্দেশ K মান_type;
		প্রকারনির্দেশ ptrdiff_t difference_type;
		প্রকারনির্দেশ K* pointer;
		প্রকারনির্দেশ K& reference;
		iterator() { }
		iterator operator++() { index--; ঘূৰাই *this; }
		bool operator==(ধ্রুৱ iterator &other) ধ্রুৱ { ঘূৰাই index == other.index; }
		bool operator!=(ধ্রুৱ iterator &other) ধ্রুৱ { ঘূৰাই index != other.index; }
		K &operator*() { ঘূৰাই ptr->entries[index].udata; }
		K *operator->() { ঘূৰাই &ptr->entries[index].udata; }
		ধ্রুৱ K &operator*() ধ্রুৱ { ঘূৰাই ptr->entries[index].udata; }
		ধ্রুৱ K *operator->() ধ্রুৱ { ঘূৰাই &ptr->entries[index].udata; }
		operator const_iterator() ধ্রুৱ { ঘূৰাই const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(ধ্রুৱ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ধ্রুৱ pool &other) {
		entries = other.entries;
		do_rehash();
		ঘূৰাই *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		ঘূৰাই *this;
	}

	pool(ধ্রুৱ std::initializer_list<K> &list)
	{
		বাবে (auto &it : list)
			ঢোকাও(it);
	}

	template<class প্ৰবেশ_Iterator>
	pool(প্ৰবেশ_Iterator first, প্ৰবেশ_Iterator last)
	{
		ঢোকাও(first, last);
	}

	template<class প্ৰবেশ_Iterator>
	শূন্য ঢোকাও(প্ৰবেশ_Iterator first, প্ৰবেশ_Iterator last)
	{
		বাবে (; first != last; ++first)
			ঢোকাও(*first);
	}

	std::pair<iterator, bool> ঢোকাও(ধ্রুৱ K &মান)
	{
		Hasher::hash_t hash = do_hash(মান);
		পূর্ণাঙ্ক i = do_lookup(মান, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(মান, hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ঢোকাও(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		পূর্ণাঙ্ক i = do_lookup(rvalue, hash);
		যদি (i >= 0)
			ঘূৰাই std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ঢোকাও(std::forward<K>(rvalue), hash);
		ঘূৰাই std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		ঘূৰাই ঢোকাও(K(std::forward<Args>(args)...));
	}

	পূর্ণাঙ্ক erase(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক index = do_lookup(চাবি, hash);
		ঘূৰাই do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		ঘূৰাই ++it;
	}

	পূর্ণাঙ্ক গুনতি(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		ঘূৰাই i < 0 ? 0 : 1;
	}

	পূর্ণাঙ্ক গুনতি(ধ্রুৱ K &চাবি, const_iterator it) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		ঘূৰাই i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই end();
		ঘূৰাই iterator(this, i);
	}

	const_iterator find(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই end();
		ঘূৰাই const_iterator(this, i);
	}

	bool operator[](ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = do_hash(চাবি);
		পূর্ণাঙ্ক i = do_lookup(চাবি, hash);
		ঘূৰাই i >= 0;
	}

	template<typename Compare = std::less<K>>
	শূন্য sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ধ্রুৱ entry_t &a, ধ্রুৱ entry_t &b){ ঘূৰাই comp(b.udata, a.udata); });
		do_rehash();
	}

	K বেৰ_কৰো()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ঘূৰাই ret;
	}

	শূন্য swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ধ্রুৱ pool &other) ধ্রুৱ {
		যদি (size() != other.size())
			ঘূৰাই false;
		বাবে (auto &it : entries)
			যদি (!other.গুনতি(it.udata))
				ঘূৰাই false;
		ঘূৰাই true;
	}

	bool operator!=(ধ্রুৱ pool &other) ধ্রুৱ {
		ঘূৰাই !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
		commutative_hash comm;
		বাবে (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ঘূৰাই comm.hash_into(h);
	}

	শূন্য reserve(size_t n) { entries.reserve(n); }
	size_t size() ধ্রুৱ { ঘূৰাই entries.size(); }
	bool empty() ধ্রুৱ { ঘূৰাই entries.empty(); }
	শূন্য clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ঘূৰাই iterator(this, পূর্ণাঙ্ক(entries.size())-1); }
	iterator element(পূর্ণাঙ্ক n) { ঘূৰাই iterator(this, পূর্ণাঙ্ক(entries.size())-1-n); }
	iterator end() { ঘূৰাই iterator(nullptr, -1); }

	const_iterator begin() ধ্রুৱ { ঘূৰাই const_iterator(this, পূর্ণাঙ্ক(entries.size())-1); }
	const_iterator element(পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই const_iterator(this, পূর্ণাঙ্ক(entries.size())-1-n); }
	const_iterator end() ধ্রুৱ { ঘূৰাই const_iterator(nullptr, -1); }
};

template<typename K, পূর্ণাঙ্ক offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

সার্ৱজনিক:
	class const_iterator
	{
		friend class idict;
	রক্ষিত:
		ধ্রুৱ idict &container;
		পূর্ণাঙ্ক index;
		const_iterator(ধ্রুৱ idict &container, পূর্ণাঙ্ক index) : container(container), index(index) { }
	সার্ৱজনিক:
		প্রকারনির্দেশ std::forward_iterator_tag iterator_category;
		প্রকারনির্দেশ K মান_type;
		প্রকারনির্দেশ ptrdiff_t difference_type;
		প্রকারনির্দেশ K* pointer;
		প্রকারনির্দেশ K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; ঘূৰাই *this; }
		bool operator==(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index == other.index; }
		bool operator!=(ধ্রুৱ const_iterator &other) ধ্রুৱ { ঘূৰাই index != other.index; }
		ধ্রুৱ K &operator*() ধ্রুৱ { ঘূৰাই container[index]; }
		ধ্রুৱ K *operator->() ধ্রুৱ { ঘূৰাই &container[index]; }
	};

	constexpr idict()
	{
	}

	পূর্ণাঙ্ক operator()(ধ্রুৱ K &চাবি)
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণাঙ্ক i = database.do_lookup(চাবি, hash);
		যদি (i < 0)
			i = database.do_ঢোকাও(চাবি, hash);
		ঘূৰাই i + offset;
	}

	পূর্ণাঙ্ক at(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণাঙ্ক i = database.do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			throw std::out_of_range("idict::at()");
		ঘূৰাই i + offset;
	}

	পূর্ণাঙ্ক at(ধ্রুৱ K &চাবি, পূর্ণাঙ্ক defval) ধ্রুৱ
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণাঙ্ক i = database.do_lookup_no_rehash(চাবি, hash);
		যদি (i < 0)
			ঘূৰাই defval;
		ঘূৰাই i + offset;
	}

	পূর্ণাঙ্ক গুনতি(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		পূর্ণাঙ্ক i = database.do_lookup_no_rehash(চাবি, hash);
		ঘূৰাই i < 0 ? 0 : 1;
	}

	পূর্ণাঙ্ক lookup(ধ্রুৱ K &চাবি) ধ্রুৱ
	{
		Hasher::hash_t hash = database.do_hash(চাবি);
		ঘূৰাই database.do_lookup_no_rehash(চাবি, hash);
	}

	শূন্য expect(ধ্রুৱ K &চাবি, পূর্ণাঙ্ক i)
	{
		পূর্ণাঙ্ক j = (*this)(চাবি);
		যদি (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ধ্রুৱ K &operator[](পূর্ণাঙ্ক index) ধ্রুৱ
	{
		ঘূৰাই database.entries.at(index - offset).udata;
	}

	শূন্য swap(idict &other)
	{
		database.swap(other.database);
	}

	শূন্য reserve(size_t n) { database.reserve(n); }
	size_t size() ধ্রুৱ { ঘূৰাই database.size(); }
	bool empty() ধ্রুৱ { ঘূৰাই database.empty(); }
	শূন্য clear() { database.clear(); }

	const_iterator begin() ধ্রুৱ { ঘূৰাই const_iterator(*this, offset); }
	const_iterator element(পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই const_iterator(*this, n); }
	const_iterator end() ধ্রুৱ { ঘূৰাই const_iterator(*this, offset + size()); }
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
	সার্ৱজনিক:
		explicit AtomicParent(পূর্ণাঙ্ক p) : parent(p) {}
		AtomicParent(ধ্রুৱ AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ধ্রুৱ AtomicParent &other) { set(other.get()); ঘূৰাই *this; }
		পূর্ণাঙ্ক get() ধ্রুৱ { ঘূৰাই parent.load(std::memory_order_relaxed); }
		শূন্য set(পূর্ণাঙ্ক p) { parent.store(p, std::memory_order_relaxed); }
	গুপ্ত:
		std::atomic<পূর্ণাঙ্ক> parent;
	};
	std::vector<AtomicParent> parents;

সার্ৱজনিক:
	প্রকারনির্দেশ typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	পূর্ণাঙ্ক operator()(ধ্রুৱ K &চাবি)
	{
		পূর্ণাঙ্ক i = database(চাবি);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		যদি (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		ঘূৰাই i;
	}

	// Finds an element at given index
	ধ্রুৱ K &operator[](পূর্ণাঙ্ক index) ধ্রুৱ
	{
		ঘূৰাই database[index];
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
	পূর্ণাঙ্ক ifind(পূর্ণাঙ্ক i) ধ্রুৱ
	{
		পূর্ণাঙ্ক p = i, k = i;

		যেতিয়ালৈ (true) {
			পূর্ণাঙ্ক pp = parents[p].get();
			যদি (pp < 0)
				ভঙ্গ;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		যেতিয়ালৈ (k != p) {
			পূর্ণাঙ্ক next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		ঘূৰাই p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	শূন্য imerge(পূর্ণাঙ্ক i, পূর্ণাঙ্ক j)
	{
		i = ifind(i);
		j = ifind(j);

		যদি (i != j)
			parents[i].set(j);
	}

	শূন্য ipromote(পূর্ণাঙ্ক i)
	{
		পূর্ণাঙ্ক k = i;

		যেতিয়ালৈ (k != -1) {
			পূর্ণাঙ্ক next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	পূর্ণাঙ্ক lookup(ধ্রুৱ K &a)
	{
		ঘূৰাই ifind((*this)(a));
	}

	ধ্রুৱ K &find(ধ্রুৱ K &a) ধ্রুৱ
	{
		পূর্ণাঙ্ক i = database.at(a, -1);
		যদি (i < 0)
			ঘূৰাই a;
		ঘূৰাই (*this)[ifind(i)];
	}

	শূন্য merge(ধ্রুৱ K &a, ধ্রুৱ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	শূন্য promote(ধ্রুৱ K &a)
	{
		পূর্ণাঙ্ক i = database.at(a, -1);
		যদি (i >= 0)
			ipromote(i);
	}

	শূন্য swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	শূন্য reserve(size_t n) { database.reserve(n); }
	size_t size() ধ্রুৱ { ঘূৰাই database.size(); }
	bool empty() ধ্রুৱ { ঘূৰাই database.empty(); }
	শূন্য clear() { database.clear(); parents.clear(); }

	const_iterator begin() ধ্রুৱ { ঘূৰাই database.begin(); }
	const_iterator element(পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই database.element(n); }
	const_iterator end() ধ্রুৱ { ঘূৰাই database.end(); }
};

} /* namespace hashlib */

#endif
