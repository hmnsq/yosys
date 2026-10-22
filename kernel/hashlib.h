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

#include <array>
#include <atomic>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <type_traits>
#include <stdint.h>

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

#အကယ်၍ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#မဟုတ်ပါက
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

const ကိန်းပြည့် hashtable_size_trigger = 2;
const ကိန်းပြည့် hashtable_size_factor = 3;

namespace legacy {
	anta_hstha uint32_t djb2_add(uint32_t a, uint32_t b) {
		ပြန်ပေး ((a << 5) + a) + b;
	}
};

template<typename T>
sa_nracanaa hash_ops;

anta_hstha unsigned ကိန်းပြည့် mkhash_xorshift(unsigned ကိန်းပြည့် a) {
	အကယ်၍ (parimaa_na(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} မဟုတ်ပါက အကယ်၍ (parimaa_na(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} မဟုတ်ပါက
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	ပြန်ပေး a;
}

class HasherDJB32 {
public:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

private:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	static uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		ပြန်ပေး hash;
	}
public:
	void hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		ပြန်ပေး;
	}
	void hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		ပြန်ပေး;
	}
	[[nodiscard]]
	hash_t yield() const {
		ပြန်ပေး (hash_t)state;
	}

	template<typename T>
	void eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	void eat(const T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	void commutative_eat(hash_t t) {
		state ^= t;
	}

	void force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_TOP_LOOP_FST [[nodiscard]] static anta_hstha Hasher hash
#define HASH_TOP_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	ပြန်ပေး h; \
}

template<typename T>
sa_nracanaa hash_ops {
	static anta_hstha bool cmp(const T &a, const T &b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const T &a, Hasher h) {
		အကယ်၍ constexpr (std::is_integral_v<T>) {
			static_assert(parimaa_na(T) <= parimaa_na(uint64_t));
			အကယ်၍ (parimaa_na(T) == parimaa_na(uint64_t))
				h.hash64(a);
			မဟုတ်ပါက
				h.hash32(a);
			ပြန်ပေး h;
		} မဟုတ်ပါက အကယ်၍ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			ပြန်ပေး hash_ops<u_type>::hash_into((u_type) a, h);
		} မဟုတ်ပါက အကယ်၍ constexpr (std::is_pointer_v<T>) {
			ပြန်ပေး hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} မဟုတ်ပါက အကယ်၍ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			ကိန်းပြည့် size = a.size();
			ကိန်းပြည့် i = 0;
			နေစဉ် (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			ပြန်ပေး h;
		} မဟုတ်ပါက {
			ပြန်ပေး a.hash_into(h);
		}
	}
	HASH_TOP_LOOP_FST (const T &a) HASH_TOP_LOOP_SND
};

template<typename P, typename Q> sa_nracanaa hash_ops<std::pair<P, Q>> {
	static anta_hstha bool cmp(const std::pair<P, Q> &a, const std::pair<P, Q> &b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		ပြန်ပေး h;
	}
	HASH_TOP_LOOP_FST (const std::pair<P, Q> &a) HASH_TOP_LOOP_SND
	[[nodiscard]] static anta_hstha Hasher hash(const P &p, const Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		ပြန်ပေး h;
	}
};

template<typename... T> sa_nracanaa hash_ops<std::tuple<T...>> {
	static anta_hstha bool cmp(const std::tuple<T...> &a, const std::tuple<T...> &b) {
		ပြန်ပေး a == b;
	}
	template<size_t I = 0>
	static anta_hstha typename std::enable_if<I == parimaa_na...(T), Hasher>::type hash_into(const std::tuple<T...> &, Hasher h) {
		ပြန်ပေး h;
	}
	template<size_t I = 0>
	static anta_hstha typename std::enable_if<I != parimaa_na...(T), Hasher>::type hash_into(const std::tuple<T...> &a, Hasher h) {
		typedef hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		ပြန်ပေး h;
	}
	HASH_TOP_LOOP_FST (const std::tuple<T...> &a) HASH_TOP_LOOP_SND
};

template<typename T> sa_nracanaa hash_ops<std::vector<T>> {
	static anta_hstha bool cmp(const std::vector<T> &a, const std::vector<T> &b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		အတွက် (auto k : a)
			h.eat(k);
		ပြန်ပေး h;
	}
	HASH_TOP_LOOP_FST (const std::vector<T> &a) HASH_TOP_LOOP_SND
};

template<typename T, size_t N> sa_nracanaa hash_ops<std::array<T, N>> {
    static anta_hstha bool cmp(const std::array<T, N> &a, const std::array<T, N> &b) {
        ပြန်ပေး a == b;
    }
    [[nodiscard]] static anta_hstha Hasher hash_into(const std::array<T, N> &a, Hasher h) {
        အတွက် (const auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        ပြန်ပေး h;
    }
	HASH_TOP_LOOP_FST (const std::array<T, N> &a) HASH_TOP_LOOP_SND
};

sa_nracanaa hash_cstr_ops {
	static anta_hstha bool cmp(const var_na *a, const var_na *b) {
		ပြန်ပေး strcmp(a, b) == 0;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const var_na *a, Hasher h) {
		နေစဉ် (*a)
			h.hash32(*(a++));
		ပြန်ပေး h;
	}
	HASH_TOP_LOOP_FST (const var_na *a) HASH_TOP_LOOP_SND
};

template <> sa_nracanaa hash_ops<var_na*> : hash_cstr_ops {};

sa_nracanaa hash_ptr_ops {
	static anta_hstha bool cmp(const void *a, const void *b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const void *a, Hasher h) {
		ပြန်ပေး hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_TOP_LOOP_FST (const void *a) HASH_TOP_LOOP_SND
};

sa_nracanaa hash_obj_ops {
	static anta_hstha bool cmp(const void *a, const void *b) {
		ပြန်ပေး a == b;
	}
	template<typename T>
	[[nodiscard]] static anta_hstha Hasher hash_into(const T *a, Hasher h) {
		အကယ်၍ (a)
			h = a->hash_into(h);
		မဟုတ်ပါက
			h.eat(0);
		ပြန်ပေး h;
	}
	template<typename T>
	HASH_TOP_LOOP_FST (const T *a) HASH_TOP_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(const T& obj) {
	ပြန်ပေး hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
anta_hstha unsigned ကိန်းပြည့် mkhash(const T &v) {
	ပြန်ပေး (unsigned ကိန်းပြည့်) run_hash<T>(v);
}

template<> sa_nracanaa hash_ops<std::monostate> {
	static anta_hstha bool cmp(std::monostate a, std::monostate b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(std::monostate, Hasher h) {
		ပြန်ပေး h;
	}
};

template<typename... T> sa_nracanaa hash_ops<std::variant<T...>> {
	static anta_hstha bool cmp(const std::variant<T...> &a, const std::variant<T...> &b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const std::variant<T...> &a, Hasher h) {
		std::visit([& h](const auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		ပြန်ပေး h;
	}
};

template<typename T> sa_nracanaa hash_ops<std::optional<T>> {
	static anta_hstha bool cmp(const std::optional<T> &a, const std::optional<T> &b) {
		ပြန်ပေး a == b;
	}
	[[nodiscard]] static anta_hstha Hasher hash_into(const std::optional<T> &a, Hasher h) {
		အကယ်၍(a.has_value())
			h.eat(*a);
		မဟုတ်ပါက
			h.eat(0);
		ပြန်ပေး h;
	}
};

anta_hstha unsigned ကိန်းပြည့် hashtable_size(unsigned ကိန်းပြည့် min_size)
{
	// Primes as generated by https://oeis.org/A175953
	static std::vector<unsigned ကိန်းပြည့်> zero_and_some_primes = {
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

	အတွက် (auto p : zero_and_some_primes)
		အကယ်၍ (p >= min_size) ပြန်ပေး p;

	အကယ်၍ (parimaa_na(unsigned ကိန်းပြည့်) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	အတွက် (auto p : zero_and_some_primes)
		အကယ်၍ (100129 * p > min_size) ပြန်ပေး 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, ကိန်းပြည့် offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
public:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	void eat(const T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	void eat(const Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) const {
		အတွက် (auto b : buckets)
			h.eat(b);
		ပြန်ပေး h;
	}
private:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	sa_nracanaa entry_t
	{
		std::pair<K, T> udata;
		ကိန်းပြည့် next;

		entry_t() { }
		entry_t(const std::pair<K, T> &udata, ကိန်းပြည့် next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, ကိန်းပြည့် next) : udata(std::move(udata)), next(next) { }
		bool operator<(const entry_t &other) const { ပြန်ပေး udata.first < other.udata.first; }
	};

	std::vector<ကိန်းပြည့်> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	static anta_hstha void do_assert(bool) { }
#မဟုတ်ပါက
	static anta_hstha void do_assert(bool cond) {
		အကယ်၍ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(const K &key) const
	{
		Hasher::hash_t hash = 0;
		အကယ်၍ (!hashtable.empty())
			hash = ops.hash(key).yield() % (unsigned ကိန်းပြည့်)(hashtable.size());
		ပြန်ပေး hash;
	}

	void do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		အတွက် (ကိန်းပြည့် i = 0; i < ကိန်းပြည့်(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ကိန်းပြည့်(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ကိန်းပြည့် do_erase(ကိန်းပြည့် index, Hasher::hash_t hash)
	{
		do_assert(index < ကိန်းပြည့်(entries.size()));
		အကယ်၍ (hashtable.empty() || index < 0)
			ပြန်ပေး 0;

		ကိန်းပြည့် k = hashtable[hash];
		do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));

		အကယ်၍ (k == index) {
			hashtable[hash] = entries[index].next;
		} မဟုတ်ပါက {
			နေစဉ် (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ကိန်းပြည့် back_idx = entries.size()-1;

		အကယ်၍ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));

			အကယ်၍ (k == back_idx) {
				hashtable[back_hash] = index;
			} မဟုတ်ပါက {
				နေစဉ် (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.pop_back();

		အကယ်၍ (entries.empty())
			hashtable.clear();

		ပြန်ပေး 1;
	}

	ကိန်းပြည့် do_lookup(const K &key, Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty())
			ပြန်ပေး -1;

		အကယ်၍ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(key);
		}

		ပြန်ပေး do_lookup_internal(key, hash);
	}

	ကိန်းပြည့် do_lookup_internal(const K &key, Hasher::hash_t hash) const
	{
		ကိန်းပြည့် index = hashtable[hash];

		နေစဉ် (index >= 0 && !ops.cmp(entries[index].udata.first, key)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ကိန်းပြည့်(entries.size()));
		}

		ပြန်ပေး index;
	}

	ကိန်းပြည့် do_lookup_no_rehash(const K &key, Hasher::hash_t hash) const
	{
		အကယ်၍ (hashtable.empty())
			ပြန်ပေး -1;

		ပြန်ပေး do_lookup_internal(key, hash);
	}

	ကိန်းပြည့် do_insert(const K &key, const Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(key, T()), -1);
			do_rehash();
		} မဟုတ်ပါက {
			entries.emplace_back(std::pair<K, T>(key, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ပြန်ပေး entries.size() - 1;
	}

	ကိန်းပြည့် do_insert(const std::pair<K, T> &value, const Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty()) {
			entries.emplace_back(value, -1);
			do_rehash();
		} မဟုတ်ပါက {
			entries.emplace_back(value, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ပြန်ပေး entries.size() - 1;
	}

	ကိန်းပြည့် do_insert(std::pair<K, T> &&rvalue, const Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} မဟုတ်ပါက {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ပြန်ပေး entries.size() - 1;
	}

public:
	class const_iterator
	{
		friend class dict;
	rak_sita:
		const dict *ptr;
		ကိန်းပြည့် index;
		const_iterator(const dict *ptr, ကိန်းပြည့် index) : ptr(ptr), index(index) { }
	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef std::pair<K, T> value_type;
		typedef ptrdiff_t difference_type;
		typedef const std::pair<K, T>* pointer;
		typedef const std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ပြန်ပေး *this; }
		const_iterator operator++(ကိန်းပြည့်) { const_iterator tmp = *this; index--; ပြန်ပေး tmp; }
		const_iterator operator--() { index++; ပြန်ပေး *this; }
		const_iterator operator--(ကိန်းပြည့်) { const_iterator tmp = *this; index++; ပြန်ပေး tmp; }
		const_iterator operator+=(ကိန်းပြည့် amt) { index -= amt; ပြန်ပေး *this; }
		bool operator<(const const_iterator &other) const { ပြန်ပေး index > other.index; }
		bool operator==(const const_iterator &other) const { ပြန်ပေး index == other.index; }
		bool operator!=(const const_iterator &other) const { ပြန်ပေး index != other.index; }
		const std::pair<K, T> &operator*() const { ပြန်ပေး ptr->entries[index].udata; }
		const std::pair<K, T> *operator->() const { ပြန်ပေး &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	rak_sita:
		dict *ptr;
		ကိန်းပြည့် index;
		iterator(dict *ptr, ကိန်းပြည့် index) : ptr(ptr), index(index) { }
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef std::pair<K, T> value_type;
		typedef ptrdiff_t difference_type;
		typedef std::pair<K, T>* pointer;
		typedef std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; ပြန်ပေး *this; }
		iterator operator+=(ကိန်းပြည့် amt) { index -= amt; ပြန်ပေး *this; }
		bool operator<(const iterator &other) const { ပြန်ပေး index > other.index; }
		bool operator==(const iterator &other) const { ပြန်ပေး index == other.index; }
		bool operator!=(const iterator &other) const { ပြန်ပေး index != other.index; }
		std::pair<K, T> &operator*() { ပြန်ပေး ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { ပြန်ပေး &ptr->entries[index].udata; }
		const std::pair<K, T> &operator*() const { ပြန်ပေး ptr->entries[index].udata; }
		const std::pair<K, T> *operator->() const { ပြန်ပေး &ptr->entries[index].udata; }
		operator const_iterator() const { ပြန်ပေး const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() const {
		ပြန်ပေး std::make_reverse_iterator(end());
	}
	reverse_iterator rend() const {
		ပြန်ပေး std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(const dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(const dict &other) {
		entries = other.entries;
		do_rehash();
		ပြန်ပေး *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		ပြန်ပေး *this;
	}

	dict(const std::initializer_list<std::pair<K, T>> &list)
	{
		အတွက် (auto &it : list)
			insert(it);
	}

	template<class InputIterator>
	dict(InputIterator first, InputIterator last)
	{
		insert(first, last);
	}

	template<class InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		အတွက် (; first != last; ++first)
			insert(*first);
	}

	std::pair<iterator, bool> insert(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(key, hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(const std::pair<K, T> &value)
	{
		Hasher::hash_t hash = do_hash(value.first);
		ကိန်းပြည့် i = do_lookup(value.first, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(value, hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		ကိန်းပြည့် i = do_lookup(rvalue.first, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::forward<std::pair<K, T>>(rvalue), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K const &key, T const &value)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(key, value), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K const &key, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(key, std::forward<T>(rvalue)), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T const &value)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ကိန်းပြည့် i = do_lookup(rkey, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(std::forward<K>(rkey), value), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		ကိန်းပြည့် i = do_lookup(rkey, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	ကိန်းပြည့် erase(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် index = do_lookup(key, hash);
		ပြန်ပေး do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		ပြန်ပေး ++it;
	}

	ကိန်းပြည့် count(const K &key) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		ပြန်ပေး i < 0 ? 0 : 1;
	}

	ကိန်းပြည့် count(const K &key, const_iterator it) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		ပြန်ပေး i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး end();
		ပြန်ပေး iterator(this, i);
	}

	const_iterator find(const K &key) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး end();
		ပြန်ပေး const_iterator(this, i);
	}

	T& at(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i < 0)
			throw std::out_of_range("dict::at()");
		ပြန်ပေး entries[i].udata.second;
	}

	const T& at(const K &key) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			throw std::out_of_range("dict::at()");
		ပြန်ပေး entries[i].udata.second;
	}

	const T& at(const K &key, const T &defval) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး defval;
		ပြန်ပေး entries[i].udata.second;
	}

	T& operator[](const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i < 0)
			i = do_insert(std::pair<K, T>(key, T()), hash);
		ပြန်ပေး entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	void sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](const entry_t &a, const entry_t &b){ ပြန်ပေး comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	void swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(const dict &other) const {
		အကယ်၍ (size() != other.size())
			ပြန်ပေး false;
		အတွက် (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			အကယ်၍ (oit == other.end() || !(oit->second == it.udata.second))
				ပြန်ပေး false;
		}
		ပြန်ပေး true;
	}

	bool operator!=(const dict &other) const {
		ပြန်ပေး !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const {
		commutative_hash comm;
		အတွက် (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		ပြန်ပေး comm.hash_into(h);
	}

	void reserve(size_t n) { entries.reserve(n); }
	size_t size() const { ပြန်ပေး entries.size(); }
	bool empty() const { ပြန်ပေး entries.empty(); }
	void clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ပြန်ပေး iterator(this, ကိန်းပြည့်(entries.size())-1); }
	iterator element(ကိန်းပြည့် n) { ပြန်ပေး iterator(this, ကိန်းပြည့်(entries.size())-1-n); }
	iterator end() { ပြန်ပေး iterator(nullptr, -1); }

	const_iterator begin() const { ပြန်ပေး const_iterator(this, ကိန်းပြည့်(entries.size())-1); }
	const_iterator element(ကိန်းပြည့် n) const { ပြန်ပေး const_iterator(this, ကိန်းပြည့်(entries.size())-1-n); }
	const_iterator end() const { ပြန်ပေး const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, ကိန်းပြည့်, typename> friend class idict;

rak_sita:
	sa_nracanaa entry_t
	{
		K udata;
		ကိန်းပြည့် next;

		entry_t() { }
		entry_t(const K &udata, ကိန်းပြည့် next) : udata(udata), next(next) { }
		entry_t(K &&udata, ကိန်းပြည့် next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<ကိန်းပြည့်> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	static anta_hstha void do_assert(bool) { }
#မဟုတ်ပါက
	static anta_hstha void do_assert(bool cond) {
		အကယ်၍ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(const K &key) const
	{
		Hasher::hash_t hash = 0;
		အကယ်၍ (!hashtable.empty())
			hash = ops.hash(key).yield() % (unsigned ကိန်းပြည့်)(hashtable.size());
		ပြန်ပေး hash;
	}

	void do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		အတွက် (ကိန်းပြည့် i = 0; i < ကိန်းပြည့်(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < ကိန်းပြည့်(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	ကိန်းပြည့် do_erase(ကိန်းပြည့် index, Hasher::hash_t hash)
	{
		do_assert(index < ကိန်းပြည့်(entries.size()));
		အကယ်၍ (hashtable.empty() || index < 0)
			ပြန်ပေး 0;

		ကိန်းပြည့် k = hashtable[hash];
		အကယ်၍ (k == index) {
			hashtable[hash] = entries[index].next;
		} မဟုတ်ပါက {
			နေစဉ် (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		ကိန်းပြည့် back_idx = entries.size()-1;

		အကယ်၍ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			အကယ်၍ (k == back_idx) {
				hashtable[back_hash] = index;
			} မဟုတ်ပါက {
				နေစဉ် (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < ကိန်းပြည့်(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.pop_back();

		အကယ်၍ (entries.empty())
			hashtable.clear();

		ပြန်ပေး 1;
	}

	ကိန်းပြည့် do_lookup(const K &key, Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty())
			ပြန်ပေး -1;

		အကယ်၍ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(key);
		}

		ပြန်ပေး do_lookup_internal(key, hash);
	}

	ကိန်းပြည့် do_lookup_internal(const K &key, Hasher::hash_t hash) const
	{
		ကိန်းပြည့် index = hashtable[hash];

		နေစဉ် (index >= 0 && !ops.cmp(entries[index].udata, key)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < ကိန်းပြည့်(entries.size()));
		}

		ပြန်ပေး index;
	}

	ကိန်းပြည့် do_lookup_no_rehash(const K &key, Hasher::hash_t hash) const
	{
		အကယ်၍ (hashtable.empty())
			ပြန်ပေး -1;

		ပြန်ပေး do_lookup_internal(key, hash);
	}

	ကိန်းပြည့် do_insert(const K &value, Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty()) {
			entries.emplace_back(value, -1);
			do_rehash();
			hash = do_hash(value);
		} မဟုတ်ပါက {
			entries.emplace_back(value, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ပြန်ပေး entries.size() - 1;
	}

	ကိန်းပြည့် do_insert(K &&rvalue, Hasher::hash_t &hash)
	{
		အကယ်၍ (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} မဟုတ်ပါက {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		ပြန်ပေး entries.size() - 1;
	}

public:
	class const_iterator
	{
		friend class pool;
	rak_sita:
		const pool *ptr;
		ကိန်းပြည့် index;
		const_iterator(const pool *ptr, ကိန်းပြည့် index) : ptr(ptr), index(index) { }
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef K value_type;
		typedef ptrdiff_t difference_type;
		typedef K* pointer;
		typedef K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; ပြန်ပေး *this; }
		bool operator==(const const_iterator &other) const { ပြန်ပေး index == other.index; }
		bool operator!=(const const_iterator &other) const { ပြန်ပေး index != other.index; }
		const K &operator*() const { ပြန်ပေး ptr->entries[index].udata; }
		const K *operator->() const { ပြန်ပေး &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	rak_sita:
		pool *ptr;
		ကိန်းပြည့် index;
		iterator(pool *ptr, ကိန်းပြည့် index) : ptr(ptr), index(index) { }
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef K value_type;
		typedef ptrdiff_t difference_type;
		typedef K* pointer;
		typedef K& reference;
		iterator() { }
		iterator operator++() { index--; ပြန်ပေး *this; }
		bool operator==(const iterator &other) const { ပြန်ပေး index == other.index; }
		bool operator!=(const iterator &other) const { ပြန်ပေး index != other.index; }
		K &operator*() { ပြန်ပေး ptr->entries[index].udata; }
		K *operator->() { ပြန်ပေး &ptr->entries[index].udata; }
		const K &operator*() const { ပြန်ပေး ptr->entries[index].udata; }
		const K *operator->() const { ပြန်ပေး &ptr->entries[index].udata; }
		operator const_iterator() const { ပြန်ပေး const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(const pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(const pool &other) {
		entries = other.entries;
		do_rehash();
		ပြန်ပေး *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		ပြန်ပေး *this;
	}

	pool(const std::initializer_list<K> &list)
	{
		အတွက် (auto &it : list)
			insert(it);
	}

	template<class InputIterator>
	pool(InputIterator first, InputIterator last)
	{
		insert(first, last);
	}

	template<class InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		အတွက် (; first != last; ++first)
			insert(*first);
	}

	std::pair<iterator, bool> insert(const K &value)
	{
		Hasher::hash_t hash = do_hash(value);
		ကိန်းပြည့် i = do_lookup(value, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(value, hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		ကိန်းပြည့် i = do_lookup(rvalue, hash);
		အကယ်၍ (i >= 0)
			ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::forward<K>(rvalue), hash);
		ပြန်ပေး std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		ပြန်ပေး insert(K(std::forward<Args>(args)...));
	}

	ကိန်းပြည့် erase(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် index = do_lookup(key, hash);
		ပြန်ပေး do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		ပြန်ပေး ++it;
	}

	ကိန်းပြည့် count(const K &key) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		ပြန်ပေး i < 0 ? 0 : 1;
	}

	ကိန်းပြည့် count(const K &key, const_iterator it) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		ပြန်ပေး i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး end();
		ပြန်ပေး iterator(this, i);
	}

	const_iterator find(const K &key) const
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး end();
		ပြန်ပေး const_iterator(this, i);
	}

	bool operator[](const K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		ကိန်းပြည့် i = do_lookup(key, hash);
		ပြန်ပေး i >= 0;
	}

	template<typename Compare = std::less<K>>
	void sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](const entry_t &a, const entry_t &b){ ပြန်ပေး comp(b.udata, a.udata); });
		do_rehash();
	}

	K pop()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		ပြန်ပေး ret;
	}

	void swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(const pool &other) const {
		အကယ်၍ (size() != other.size())
			ပြန်ပေး false;
		အတွက် (auto &it : entries)
			အကယ်၍ (!other.count(it.udata))
				ပြန်ပေး false;
		ပြန်ပေး true;
	}

	bool operator!=(const pool &other) const {
		ပြန်ပေး !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const {
		commutative_hash comm;
		အတွက် (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		ပြန်ပေး comm.hash_into(h);
	}

	void reserve(size_t n) { entries.reserve(n); }
	size_t size() const { ပြန်ပေး entries.size(); }
	bool empty() const { ပြန်ပေး entries.empty(); }
	void clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { ပြန်ပေး iterator(this, ကိန်းပြည့်(entries.size())-1); }
	iterator element(ကိန်းပြည့် n) { ပြန်ပေး iterator(this, ကိန်းပြည့်(entries.size())-1-n); }
	iterator end() { ပြန်ပေး iterator(nullptr, -1); }

	const_iterator begin() const { ပြန်ပေး const_iterator(this, ကိန်းပြည့်(entries.size())-1); }
	const_iterator element(ကိန်းပြည့် n) const { ပြန်ပေး const_iterator(this, ကိန်းပြည့်(entries.size())-1-n); }
	const_iterator end() const { ပြန်ပေး const_iterator(nullptr, -1); }
};

template<typename K, ကိန်းပြည့် offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

public:
	class const_iterator
	{
		friend class idict;
	rak_sita:
		const idict &container;
		ကိန်းပြည့် index;
		const_iterator(const idict &container, ကိန်းပြည့် index) : container(container), index(index) { }
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef K value_type;
		typedef ptrdiff_t difference_type;
		typedef K* pointer;
		typedef K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; ပြန်ပေး *this; }
		bool operator==(const const_iterator &other) const { ပြန်ပေး index == other.index; }
		bool operator!=(const const_iterator &other) const { ပြန်ပေး index != other.index; }
		const K &operator*() const { ပြန်ပေး container[index]; }
		const K *operator->() const { ပြန်ပေး &container[index]; }
	};

	constexpr idict()
	{
	}

	ကိန်းပြည့် operator()(const K &key)
	{
		Hasher::hash_t hash = database.do_hash(key);
		ကိန်းပြည့် i = database.do_lookup(key, hash);
		အကယ်၍ (i < 0)
			i = database.do_insert(key, hash);
		ပြန်ပေး i + offset;
	}

	ကိန်းပြည့် at(const K &key) const
	{
		Hasher::hash_t hash = database.do_hash(key);
		ကိန်းပြည့် i = database.do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			throw std::out_of_range("idict::at()");
		ပြန်ပေး i + offset;
	}

	ကိန်းပြည့် at(const K &key, ကိန်းပြည့် defval) const
	{
		Hasher::hash_t hash = database.do_hash(key);
		ကိန်းပြည့် i = database.do_lookup_no_rehash(key, hash);
		အကယ်၍ (i < 0)
			ပြန်ပေး defval;
		ပြန်ပေး i + offset;
	}

	ကိန်းပြည့် count(const K &key) const
	{
		Hasher::hash_t hash = database.do_hash(key);
		ကိန်းပြည့် i = database.do_lookup_no_rehash(key, hash);
		ပြန်ပေး i < 0 ? 0 : 1;
	}

	ကိန်းပြည့် lookup(const K &key) const
	{
		Hasher::hash_t hash = database.do_hash(key);
		ပြန်ပေး database.do_lookup_no_rehash(key, hash);
	}

	void expect(const K &key, ကိန်းပြည့် i)
	{
		ကိန်းပြည့် j = (*this)(key);
		အကယ်၍ (i != j)
			throw std::out_of_range("idict::expect()");
	}

	const K &operator[](ကိန်းပြည့် index) const
	{
		ပြန်ပေး database.entries.at(index - offset).udata;
	}

	void swap(idict &other)
	{
		database.swap(other.database);
	}

	void reserve(size_t n) { database.reserve(n); }
	size_t size() const { ပြန်ပေး database.size(); }
	bool empty() const { ပြန်ပေး database.empty(); }
	void clear() { database.clear(); }

	const_iterator begin() const { ပြန်ပေး const_iterator(*this, offset); }
	const_iterator element(ကိန်းပြည့် n) const { ပြန်ပေး const_iterator(*this, n); }
	const_iterator end() const { ပြန်ပေး const_iterator(*this, offset + size()); }
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
	public:
		explicit AtomicParent(ကိန်းပြည့် p) : parent(p) {}
		AtomicParent(const AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(const AtomicParent &other) { set(other.get()); ပြန်ပေး *this; }
		ကိန်းပြည့် get() const { ပြန်ပေး parent.load(std::memory_order_relaxed); }
		void set(ကိန်းပြည့် p) { parent.store(p, std::memory_order_relaxed); }
	private:
		std::atomic<ကိန်းပြည့်> parent;
	};
	std::vector<AtomicParent> parents;

public:
	typedef typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	ကိန်းပြည့် operator()(const K &key)
	{
		ကိန်းပြည့် i = database(key);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		အကယ်၍ (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		ပြန်ပေး i;
	}

	// Finds an element at given index
	const K &operator[](ကိန်းပြည့် index) const
	{
		ပြန်ပေး database[index];
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
	ကိန်းပြည့် ifind(ကိန်းပြည့် i) const
	{
		ကိန်းပြည့် p = i, k = i;

		နေစဉ် (true) {
			ကိန်းပြည့် pp = parents[p].get();
			အကယ်၍ (pp < 0)
				bha_nga;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		နေစဉ် (k != p) {
			ကိန်းပြည့် next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		ပြန်ပေး p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	void imerge(ကိန်းပြည့် i, ကိန်းပြည့် j)
	{
		i = ifind(i);
		j = ifind(j);

		အကယ်၍ (i != j)
			parents[i].set(j);
	}

	void ipromote(ကိန်းပြည့် i)
	{
		ကိန်းပြည့် k = i;

		နေစဉ် (k != -1) {
			ကိန်းပြည့် next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	ကိန်းပြည့် lookup(const K &a)
	{
		ပြန်ပေး ifind((*this)(a));
	}

	const K &find(const K &a) const
	{
		ကိန်းပြည့် i = database.at(a, -1);
		အကယ်၍ (i < 0)
			ပြန်ပေး a;
		ပြန်ပေး (*this)[ifind(i)];
	}

	void merge(const K &a, const K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	void promote(const K &a)
	{
		ကိန်းပြည့် i = database.at(a, -1);
		အကယ်၍ (i >= 0)
			ipromote(i);
	}

	void swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	void reserve(size_t n) { database.reserve(n); }
	size_t size() const { ပြန်ပေး database.size(); }
	bool empty() const { ပြန်ပေး database.empty(); }
	void clear() { database.clear(); parents.clear(); }

	const_iterator begin() const { ပြန်ပေး database.begin(); }
	const_iterator element(ကိန်းပြည့် n) const { ပြန်ပေး database.element(n); }
	const_iterator end() const { ပြန်ပေး database.end(); }
};

} /* namespace hashlib */

#endif
