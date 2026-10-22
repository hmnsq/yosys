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

#அந்தர்பாவ <array>
#அந்தர்பாவ <atomic>
#அந்தர்பாவ <stdexcept>
#அந்தர்பாவ <algorithm>
#அந்தர்பாவ <optional>
#அந்தர்பாவ <string>
#அந்தர்பாவ <variant>
#அந்தர்பாவ <vector>
#அந்தர்பாவ <type_traits>
#அந்தர்பாவ <stdint.h>

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

#எனில் defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#இல்லையெனில்
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

மாறிலி முழு hashtable_size_trigger = 2;
மாறிலி முழு hashtable_size_factor = 3;

namespace legacy {
	உள்வரி uint32_t djb2_add(uint32_t a, uint32_t b) {
		திரும்பு ((a << 5) + a) + b;
	}
};

template<typename T>
கட்டமைப்பு hash_ops;

உள்வரி unsigned முழு mkhash_xorshift(unsigned முழு a) {
	எனில் (அளவு(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} இல்லையெனில் எனில் (அளவு(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} இல்லையெனில்
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	திரும்பு a;
}

class HasherDJB32 {
பொது:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

தனிப்பட்ட:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	நிலையான uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		திரும்பு hash;
	}
பொது:
	வெறுமை hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		திரும்பு;
	}
	வெறுமை hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		திரும்பு;
	}
	[[nodiscard]]
	hash_t yield() மாறிலி {
		திரும்பு (hash_t)state;
	}

	template<typename T>
	வெறுமை eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	வெறுமை eat(மாறிலி T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	வெறுமை commutative_eat(hash_t t) {
		state ^= t;
	}

	வெறுமை force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_உச்சி_LOOP_FST [[nodiscard]] நிலையான உள்வரி Hasher hash
#define HASH_உச்சி_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	திரும்பு h; \
}

template<typename T>
கட்டமைப்பு hash_ops {
	நிலையான உள்வரி bool cmp(மாறிலி T &a, மாறிலி T &b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி T &a, Hasher h) {
		எனில் constexpr (std::is_integral_v<T>) {
			static_assert(அளவு(T) <= அளவு(uint64_t));
			எனில் (அளவு(T) == அளவு(uint64_t))
				h.hash64(a);
			இல்லையெனில்
				h.hash32(a);
			திரும்பு h;
		} இல்லையெனில் எனில் constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			திரும்பு hash_ops<u_type>::hash_into((u_type) a, h);
		} இல்லையெனில் எனில் constexpr (std::is_pointer_v<T>) {
			திரும்பு hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} இல்லையெனில் எனில் constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			முழு size = a.size();
			முழு i = 0;
			போது (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			திரும்பு h;
		} இல்லையெனில் {
			திரும்பு a.hash_into(h);
		}
	}
	HASH_உச்சி_LOOP_FST (மாறிலி T &a) HASH_உச்சி_LOOP_SND
};

template<typename P, typename Q> கட்டமைப்பு hash_ops<std::pair<P, Q>> {
	நிலையான உள்வரி bool cmp(மாறிலி std::pair<P, Q> &a, மாறிலி std::pair<P, Q> &b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		திரும்பு h;
	}
	HASH_உச்சி_LOOP_FST (மாறிலி std::pair<P, Q> &a) HASH_உச்சி_LOOP_SND
	[[nodiscard]] நிலையான உள்வரி Hasher hash(மாறிலி P &p, மாறிலி Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		திரும்பு h;
	}
};

template<typename... T> கட்டமைப்பு hash_ops<std::tuple<T...>> {
	நிலையான உள்வரி bool cmp(மாறிலி std::tuple<T...> &a, மாறிலி std::tuple<T...> &b) {
		திரும்பு a == b;
	}
	template<size_t I = 0>
	நிலையான உள்வரி typename std::enable_if<I == அளவு...(T), Hasher>::type hash_into(மாறிலி std::tuple<T...> &, Hasher h) {
		திரும்பு h;
	}
	template<size_t I = 0>
	நிலையான உள்வரி typename std::enable_if<I != அளவு...(T), Hasher>::type hash_into(மாறிலி std::tuple<T...> &a, Hasher h) {
		வகைவரை hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		திரும்பு h;
	}
	HASH_உச்சி_LOOP_FST (மாறிலி std::tuple<T...> &a) HASH_உச்சி_LOOP_SND
};

template<typename T> கட்டமைப்பு hash_ops<std::vector<T>> {
	நிலையான உள்வரி bool cmp(மாறிலி std::vector<T> &a, மாறிலி std::vector<T> &b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		க்கு (auto k : a)
			h.eat(k);
		திரும்பு h;
	}
	HASH_உச்சி_LOOP_FST (மாறிலி std::vector<T> &a) HASH_உச்சி_LOOP_SND
};

template<typename T, size_t N> கட்டமைப்பு hash_ops<std::array<T, N>> {
    நிலையான உள்வரி bool cmp(மாறிலி std::array<T, N> &a, மாறிலி std::array<T, N> &b) {
        திரும்பு a == b;
    }
    [[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி std::array<T, N> &a, Hasher h) {
        க்கு (மாறிலி auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        திரும்பு h;
    }
	HASH_உச்சி_LOOP_FST (மாறிலி std::array<T, N> &a) HASH_உச்சி_LOOP_SND
};

கட்டமைப்பு hash_cstr_ops {
	நிலையான உள்வரி bool cmp(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
		திரும்பு strcmp(a, b) == 0;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி எழுத்து *a, Hasher h) {
		போது (*a)
			h.hash32(*(a++));
		திரும்பு h;
	}
	HASH_உச்சி_LOOP_FST (மாறிலி எழுத்து *a) HASH_உச்சி_LOOP_SND
};

template <> கட்டமைப்பு hash_ops<எழுத்து*> : hash_cstr_ops {};

கட்டமைப்பு hash_ptr_ops {
	நிலையான உள்வரி bool cmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி வெறுமை *a, Hasher h) {
		திரும்பு hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_உச்சி_LOOP_FST (மாறிலி வெறுமை *a) HASH_உச்சி_LOOP_SND
};

கட்டமைப்பு hash_obj_ops {
	நிலையான உள்வரி bool cmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b) {
		திரும்பு a == b;
	}
	template<typename T>
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி T *a, Hasher h) {
		எனில் (a)
			h = a->hash_into(h);
		இல்லையெனில்
			h.eat(0);
		திரும்பு h;
	}
	template<typename T>
	HASH_உச்சி_LOOP_FST (மாறிலி T *a) HASH_உச்சி_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(மாறிலி T& obj) {
	திரும்பு hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
உள்வரி unsigned முழு mkhash(மாறிலி T &v) {
	திரும்பு (unsigned முழு) run_hash<T>(v);
}

template<> கட்டமைப்பு hash_ops<std::monostate> {
	நிலையான உள்வரி bool cmp(std::monostate a, std::monostate b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(std::monostate, Hasher h) {
		திரும்பு h;
	}
};

template<typename... T> கட்டமைப்பு hash_ops<std::variant<T...>> {
	நிலையான உள்வரி bool cmp(மாறிலி std::variant<T...> &a, மாறிலி std::variant<T...> &b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி std::variant<T...> &a, Hasher h) {
		std::visit([& h](மாறிலி auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		திரும்பு h;
	}
};

template<typename T> கட்டமைப்பு hash_ops<std::optional<T>> {
	நிலையான உள்வரி bool cmp(மாறிலி std::optional<T> &a, மாறிலி std::optional<T> &b) {
		திரும்பு a == b;
	}
	[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி std::optional<T> &a, Hasher h) {
		எனில்(a.has_மதிப்பு())
			h.eat(*a);
		இல்லையெனில்
			h.eat(0);
		திரும்பு h;
	}
};

உள்வரி unsigned முழு hashtable_size(unsigned முழு min_size)
{
	// Primes as generated by https://oeis.org/A175953
	நிலையான std::vector<unsigned முழு> zero_and_some_primes = {
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

	க்கு (auto p : zero_and_some_primes)
		எனில் (p >= min_size) திரும்பு p;

	எனில் (அளவு(unsigned முழு) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	க்கு (auto p : zero_and_some_primes)
		எனில் (100129 * p > min_size) திரும்பு 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, முழு offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
பொது:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	வெறுமை eat(மாறிலி T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	வெறுமை eat(மாறிலி Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		க்கு (auto b : buckets)
			h.eat(b);
		திரும்பு h;
	}
தனிப்பட்ட:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	கட்டமைப்பு entry_t
	{
		std::pair<K, T> udata;
		முழு next;

		entry_t() { }
		entry_t(மாறிலி std::pair<K, T> &udata, முழு next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, முழு next) : udata(std::move(udata)), next(next) { }
		bool operator<(மாறிலி entry_t &other) மாறிலி { திரும்பு udata.first < other.udata.first; }
	};

	std::vector<முழு> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	நிலையான உள்வரி வெறுமை do_assert(bool) { }
#இல்லையெனில்
	நிலையான உள்வரி வெறுமை do_assert(bool cond) {
		எனில் (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = 0;
		எனில் (!hashtable.empty())
			hash = ops.hash(சாவி).yield() % (unsigned முழு)(hashtable.size());
		திரும்பு hash;
	}

	வெறுமை do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		க்கு (முழு i = 0; i < முழு(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < முழு(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	முழு do_erase(முழு index, Hasher::hash_t hash)
	{
		do_assert(index < முழு(entries.size()));
		எனில் (hashtable.empty() || index < 0)
			திரும்பு 0;

		முழு k = hashtable[hash];
		do_assert(0 <= k && k < முழு(entries.size()));

		எனில் (k == index) {
			hashtable[hash] = entries[index].next;
		} இல்லையெனில் {
			போது (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < முழு(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		முழு back_idx = entries.size()-1;

		எனில் (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < முழு(entries.size()));

			எனில் (k == back_idx) {
				hashtable[back_hash] = index;
			} இல்லையெனில் {
				போது (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < முழு(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.எடு_back();

		எனில் (entries.empty())
			hashtable.clear();

		திரும்பு 1;
	}

	முழு do_lookup(மாறிலி K &சாவி, Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty())
			திரும்பு -1;

		எனில் (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(சாவி);
		}

		திரும்பு do_lookup_internal(சாவி, hash);
	}

	முழு do_lookup_internal(மாறிலி K &சாவி, Hasher::hash_t hash) மாறிலி
	{
		முழு index = hashtable[hash];

		போது (index >= 0 && !ops.cmp(entries[index].udata.first, சாவி)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < முழு(entries.size()));
		}

		திரும்பு index;
	}

	முழு do_lookup_no_rehash(மாறிலி K &சாவி, Hasher::hash_t hash) மாறிலி
	{
		எனில் (hashtable.empty())
			திரும்பு -1;

		திரும்பு do_lookup_internal(சாவி, hash);
	}

	முழு do_செருகு(மாறிலி K &சாவி, மாறிலி Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(சாவி, T()), -1);
			do_rehash();
		} இல்லையெனில் {
			entries.emplace_back(std::pair<K, T>(சாவி, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		திரும்பு entries.size() - 1;
	}

	முழு do_செருகு(மாறிலி std::pair<K, T> &மதிப்பு, மாறிலி Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty()) {
			entries.emplace_back(மதிப்பு, -1);
			do_rehash();
		} இல்லையெனில் {
			entries.emplace_back(மதிப்பு, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		திரும்பு entries.size() - 1;
	}

	முழு do_செருகு(std::pair<K, T> &&rvalue, மாறிலி Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} இல்லையெனில் {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		திரும்பு entries.size() - 1;
	}

பொது:
	class const_iterator
	{
		friend class dict;
	பாதுகாக்கப்பட்ட:
		மாறிலி dict *ptr;
		முழு index;
		const_iterator(மாறிலி dict *ptr, முழு index) : ptr(ptr), index(index) { }
	பொது:
		வகைவரை std::bidirectional_iterator_tag iterator_category;
		வகைவரை std::pair<K, T> மதிப்பு_type;
		வகைவரை ptrdiff_t difference_type;
		வகைவரை மாறிலி std::pair<K, T>* pointer;
		வகைவரை மாறிலி std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; திரும்பு *this; }
		const_iterator operator++(முழு) { const_iterator tmp = *this; index--; திரும்பு tmp; }
		const_iterator operator--() { index++; திரும்பு *this; }
		const_iterator operator--(முழு) { const_iterator tmp = *this; index++; திரும்பு tmp; }
		const_iterator operator+=(முழு amt) { index -= amt; திரும்பு *this; }
		bool operator<(மாறிலி const_iterator &other) மாறிலி { திரும்பு index > other.index; }
		bool operator==(மாறிலி const_iterator &other) மாறிலி { திரும்பு index == other.index; }
		bool operator!=(மாறிலி const_iterator &other) மாறிலி { திரும்பு index != other.index; }
		மாறிலி std::pair<K, T> &operator*() மாறிலி { திரும்பு ptr->entries[index].udata; }
		மாறிலி std::pair<K, T> *operator->() மாறிலி { திரும்பு &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	பாதுகாக்கப்பட்ட:
		dict *ptr;
		முழு index;
		iterator(dict *ptr, முழு index) : ptr(ptr), index(index) { }
	பொது:
		வகைவரை std::forward_iterator_tag iterator_category;
		வகைவரை std::pair<K, T> மதிப்பு_type;
		வகைவரை ptrdiff_t difference_type;
		வகைவரை std::pair<K, T>* pointer;
		வகைவரை std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; திரும்பு *this; }
		iterator operator+=(முழு amt) { index -= amt; திரும்பு *this; }
		bool operator<(மாறிலி iterator &other) மாறிலி { திரும்பு index > other.index; }
		bool operator==(மாறிலி iterator &other) மாறிலி { திரும்பு index == other.index; }
		bool operator!=(மாறிலி iterator &other) மாறிலி { திரும்பு index != other.index; }
		std::pair<K, T> &operator*() { திரும்பு ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { திரும்பு &ptr->entries[index].udata; }
		மாறிலி std::pair<K, T> &operator*() மாறிலி { திரும்பு ptr->entries[index].udata; }
		மாறிலி std::pair<K, T> *operator->() மாறிலி { திரும்பு &ptr->entries[index].udata; }
		operator const_iterator() மாறிலி { திரும்பு const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() மாறிலி {
		திரும்பு std::make_reverse_iterator(end());
	}
	reverse_iterator rend() மாறிலி {
		திரும்பு std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(மாறிலி dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(மாறிலி dict &other) {
		entries = other.entries;
		do_rehash();
		திரும்பு *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		திரும்பு *this;
	}

	dict(மாறிலி std::initializer_list<std::pair<K, T>> &list)
	{
		க்கு (auto &it : list)
			செருகு(it);
	}

	template<class உள்ளீடு_Iterator>
	dict(உள்ளீடு_Iterator first, உள்ளீடு_Iterator last)
	{
		செருகு(first, last);
	}

	template<class உள்ளீடு_Iterator>
	வெறுமை செருகு(உள்ளீடு_Iterator first, உள்ளீடு_Iterator last)
	{
		க்கு (; first != last; ++first)
			செருகு(*first);
	}

	std::pair<iterator, bool> செருகு(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(சாவி, hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> செருகு(மாறிலி std::pair<K, T> &மதிப்பு)
	{
		Hasher::hash_t hash = do_hash(மதிப்பு.first);
		முழு i = do_lookup(மதிப்பு.first, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(மதிப்பு, hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> செருகு(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		முழு i = do_lookup(rvalue.first, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::forward<std::pair<K, T>>(rvalue), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K மாறிலி &சாவி, T மாறிலி &மதிப்பு)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::make_pair(சாவி, மதிப்பு), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K மாறிலி &சாவி, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::make_pair(சாவி, std::forward<T>(rvalue)), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T மாறிலி &மதிப்பு)
	{
		Hasher::hash_t hash = do_hash(rkey);
		முழு i = do_lookup(rkey, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::make_pair(std::forward<K>(rkey), மதிப்பு), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		முழு i = do_lookup(rkey, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	முழு erase(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு index = do_lookup(சாவி, hash);
		திரும்பு do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		திரும்பு ++it;
	}

	முழு மொத்தம்(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		திரும்பு i < 0 ? 0 : 1;
	}

	முழு மொத்தம்(மாறிலி K &சாவி, const_iterator it) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		திரும்பு i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i < 0)
			திரும்பு end();
		திரும்பு iterator(this, i);
	}

	const_iterator find(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			திரும்பு end();
		திரும்பு const_iterator(this, i);
	}

	T& at(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i < 0)
			throw std::out_of_range("dict::at()");
		திரும்பு entries[i].udata.second;
	}

	மாறிலி T& at(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			throw std::out_of_range("dict::at()");
		திரும்பு entries[i].udata.second;
	}

	மாறிலி T& at(மாறிலி K &சாவி, மாறிலி T &defval) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			திரும்பு defval;
		திரும்பு entries[i].udata.second;
	}

	T& operator[](மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i < 0)
			i = do_செருகு(std::pair<K, T>(சாவி, T()), hash);
		திரும்பு entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	வெறுமை sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](மாறிலி entry_t &a, மாறிலி entry_t &b){ திரும்பு comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	வெறுமை swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(மாறிலி dict &other) மாறிலி {
		எனில் (size() != other.size())
			திரும்பு false;
		க்கு (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			எனில் (oit == other.end() || !(oit->second == it.udata.second))
				திரும்பு false;
		}
		திரும்பு true;
	}

	bool operator!=(மாறிலி dict &other) மாறிலி {
		திரும்பு !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		commutative_hash comm;
		க்கு (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		திரும்பு comm.hash_into(h);
	}

	வெறுமை reserve(size_t n) { entries.reserve(n); }
	size_t size() மாறிலி { திரும்பு entries.size(); }
	bool empty() மாறிலி { திரும்பு entries.empty(); }
	வெறுமை clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { திரும்பு iterator(this, முழு(entries.size())-1); }
	iterator element(முழு n) { திரும்பு iterator(this, முழு(entries.size())-1-n); }
	iterator end() { திரும்பு iterator(nullptr, -1); }

	const_iterator begin() மாறிலி { திரும்பு const_iterator(this, முழு(entries.size())-1); }
	const_iterator element(முழு n) மாறிலி { திரும்பு const_iterator(this, முழு(entries.size())-1-n); }
	const_iterator end() மாறிலி { திரும்பு const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, முழு, typename> friend class idict;

பாதுகாக்கப்பட்ட:
	கட்டமைப்பு entry_t
	{
		K udata;
		முழு next;

		entry_t() { }
		entry_t(மாறிலி K &udata, முழு next) : udata(udata), next(next) { }
		entry_t(K &&udata, முழு next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<முழு> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	நிலையான உள்வரி வெறுமை do_assert(bool) { }
#இல்லையெனில்
	நிலையான உள்வரி வெறுமை do_assert(bool cond) {
		எனில் (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = 0;
		எனில் (!hashtable.empty())
			hash = ops.hash(சாவி).yield() % (unsigned முழு)(hashtable.size());
		திரும்பு hash;
	}

	வெறுமை do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		க்கு (முழு i = 0; i < முழு(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < முழு(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	முழு do_erase(முழு index, Hasher::hash_t hash)
	{
		do_assert(index < முழு(entries.size()));
		எனில் (hashtable.empty() || index < 0)
			திரும்பு 0;

		முழு k = hashtable[hash];
		எனில் (k == index) {
			hashtable[hash] = entries[index].next;
		} இல்லையெனில் {
			போது (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < முழு(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		முழு back_idx = entries.size()-1;

		எனில் (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			எனில் (k == back_idx) {
				hashtable[back_hash] = index;
			} இல்லையெனில் {
				போது (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < முழு(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.எடு_back();

		எனில் (entries.empty())
			hashtable.clear();

		திரும்பு 1;
	}

	முழு do_lookup(மாறிலி K &சாவி, Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty())
			திரும்பு -1;

		எனில் (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(சாவி);
		}

		திரும்பு do_lookup_internal(சாவி, hash);
	}

	முழு do_lookup_internal(மாறிலி K &சாவி, Hasher::hash_t hash) மாறிலி
	{
		முழு index = hashtable[hash];

		போது (index >= 0 && !ops.cmp(entries[index].udata, சாவி)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < முழு(entries.size()));
		}

		திரும்பு index;
	}

	முழு do_lookup_no_rehash(மாறிலி K &சாவி, Hasher::hash_t hash) மாறிலி
	{
		எனில் (hashtable.empty())
			திரும்பு -1;

		திரும்பு do_lookup_internal(சாவி, hash);
	}

	முழு do_செருகு(மாறிலி K &மதிப்பு, Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty()) {
			entries.emplace_back(மதிப்பு, -1);
			do_rehash();
			hash = do_hash(மதிப்பு);
		} இல்லையெனில் {
			entries.emplace_back(மதிப்பு, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		திரும்பு entries.size() - 1;
	}

	முழு do_செருகு(K &&rvalue, Hasher::hash_t &hash)
	{
		எனில் (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} இல்லையெனில் {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		திரும்பு entries.size() - 1;
	}

பொது:
	class const_iterator
	{
		friend class pool;
	பாதுகாக்கப்பட்ட:
		மாறிலி pool *ptr;
		முழு index;
		const_iterator(மாறிலி pool *ptr, முழு index) : ptr(ptr), index(index) { }
	பொது:
		வகைவரை std::forward_iterator_tag iterator_category;
		வகைவரை K மதிப்பு_type;
		வகைவரை ptrdiff_t difference_type;
		வகைவரை K* pointer;
		வகைவரை K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; திரும்பு *this; }
		bool operator==(மாறிலி const_iterator &other) மாறிலி { திரும்பு index == other.index; }
		bool operator!=(மாறிலி const_iterator &other) மாறிலி { திரும்பு index != other.index; }
		மாறிலி K &operator*() மாறிலி { திரும்பு ptr->entries[index].udata; }
		மாறிலி K *operator->() மாறிலி { திரும்பு &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	பாதுகாக்கப்பட்ட:
		pool *ptr;
		முழு index;
		iterator(pool *ptr, முழு index) : ptr(ptr), index(index) { }
	பொது:
		வகைவரை std::forward_iterator_tag iterator_category;
		வகைவரை K மதிப்பு_type;
		வகைவரை ptrdiff_t difference_type;
		வகைவரை K* pointer;
		வகைவரை K& reference;
		iterator() { }
		iterator operator++() { index--; திரும்பு *this; }
		bool operator==(மாறிலி iterator &other) மாறிலி { திரும்பு index == other.index; }
		bool operator!=(மாறிலி iterator &other) மாறிலி { திரும்பு index != other.index; }
		K &operator*() { திரும்பு ptr->entries[index].udata; }
		K *operator->() { திரும்பு &ptr->entries[index].udata; }
		மாறிலி K &operator*() மாறிலி { திரும்பு ptr->entries[index].udata; }
		மாறிலி K *operator->() மாறிலி { திரும்பு &ptr->entries[index].udata; }
		operator const_iterator() மாறிலி { திரும்பு const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(மாறிலி pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(மாறிலி pool &other) {
		entries = other.entries;
		do_rehash();
		திரும்பு *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		திரும்பு *this;
	}

	pool(மாறிலி std::initializer_list<K> &list)
	{
		க்கு (auto &it : list)
			செருகு(it);
	}

	template<class உள்ளீடு_Iterator>
	pool(உள்ளீடு_Iterator first, உள்ளீடு_Iterator last)
	{
		செருகு(first, last);
	}

	template<class உள்ளீடு_Iterator>
	வெறுமை செருகு(உள்ளீடு_Iterator first, உள்ளீடு_Iterator last)
	{
		க்கு (; first != last; ++first)
			செருகு(*first);
	}

	std::pair<iterator, bool> செருகு(மாறிலி K &மதிப்பு)
	{
		Hasher::hash_t hash = do_hash(மதிப்பு);
		முழு i = do_lookup(மதிப்பு, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(மதிப்பு, hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> செருகு(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		முழு i = do_lookup(rvalue, hash);
		எனில் (i >= 0)
			திரும்பு std::pair<iterator, bool>(iterator(this, i), false);
		i = do_செருகு(std::forward<K>(rvalue), hash);
		திரும்பு std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		திரும்பு செருகு(K(std::forward<Args>(args)...));
	}

	முழு erase(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு index = do_lookup(சாவி, hash);
		திரும்பு do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		திரும்பு ++it;
	}

	முழு மொத்தம்(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		திரும்பு i < 0 ? 0 : 1;
	}

	முழு மொத்தம்(மாறிலி K &சாவி, const_iterator it) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		திரும்பு i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		எனில் (i < 0)
			திரும்பு end();
		திரும்பு iterator(this, i);
	}

	const_iterator find(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			திரும்பு end();
		திரும்பு const_iterator(this, i);
	}

	bool operator[](மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = do_hash(சாவி);
		முழு i = do_lookup(சாவி, hash);
		திரும்பு i >= 0;
	}

	template<typename Compare = std::less<K>>
	வெறுமை sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](மாறிலி entry_t &a, மாறிலி entry_t &b){ திரும்பு comp(b.udata, a.udata); });
		do_rehash();
	}

	K எடு()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		திரும்பு ret;
	}

	வெறுமை swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(மாறிலி pool &other) மாறிலி {
		எனில் (size() != other.size())
			திரும்பு false;
		க்கு (auto &it : entries)
			எனில் (!other.மொத்தம்(it.udata))
				திரும்பு false;
		திரும்பு true;
	}

	bool operator!=(மாறிலி pool &other) மாறிலி {
		திரும்பு !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		commutative_hash comm;
		க்கு (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		திரும்பு comm.hash_into(h);
	}

	வெறுமை reserve(size_t n) { entries.reserve(n); }
	size_t size() மாறிலி { திரும்பு entries.size(); }
	bool empty() மாறிலி { திரும்பு entries.empty(); }
	வெறுமை clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { திரும்பு iterator(this, முழு(entries.size())-1); }
	iterator element(முழு n) { திரும்பு iterator(this, முழு(entries.size())-1-n); }
	iterator end() { திரும்பு iterator(nullptr, -1); }

	const_iterator begin() மாறிலி { திரும்பு const_iterator(this, முழு(entries.size())-1); }
	const_iterator element(முழு n) மாறிலி { திரும்பு const_iterator(this, முழு(entries.size())-1-n); }
	const_iterator end() மாறிலி { திரும்பு const_iterator(nullptr, -1); }
};

template<typename K, முழு offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

பொது:
	class const_iterator
	{
		friend class idict;
	பாதுகாக்கப்பட்ட:
		மாறிலி idict &container;
		முழு index;
		const_iterator(மாறிலி idict &container, முழு index) : container(container), index(index) { }
	பொது:
		வகைவரை std::forward_iterator_tag iterator_category;
		வகைவரை K மதிப்பு_type;
		வகைவரை ptrdiff_t difference_type;
		வகைவரை K* pointer;
		வகைவரை K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; திரும்பு *this; }
		bool operator==(மாறிலி const_iterator &other) மாறிலி { திரும்பு index == other.index; }
		bool operator!=(மாறிலி const_iterator &other) மாறிலி { திரும்பு index != other.index; }
		மாறிலி K &operator*() மாறிலி { திரும்பு container[index]; }
		மாறிலி K *operator->() மாறிலி { திரும்பு &container[index]; }
	};

	constexpr idict()
	{
	}

	முழு operator()(மாறிலி K &சாவி)
	{
		Hasher::hash_t hash = database.do_hash(சாவி);
		முழு i = database.do_lookup(சாவி, hash);
		எனில் (i < 0)
			i = database.do_செருகு(சாவி, hash);
		திரும்பு i + offset;
	}

	முழு at(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = database.do_hash(சாவி);
		முழு i = database.do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			throw std::out_of_range("idict::at()");
		திரும்பு i + offset;
	}

	முழு at(மாறிலி K &சாவி, முழு defval) மாறிலி
	{
		Hasher::hash_t hash = database.do_hash(சாவி);
		முழு i = database.do_lookup_no_rehash(சாவி, hash);
		எனில் (i < 0)
			திரும்பு defval;
		திரும்பு i + offset;
	}

	முழு மொத்தம்(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = database.do_hash(சாவி);
		முழு i = database.do_lookup_no_rehash(சாவி, hash);
		திரும்பு i < 0 ? 0 : 1;
	}

	முழு lookup(மாறிலி K &சாவி) மாறிலி
	{
		Hasher::hash_t hash = database.do_hash(சாவி);
		திரும்பு database.do_lookup_no_rehash(சாவி, hash);
	}

	வெறுமை expect(மாறிலி K &சாவி, முழு i)
	{
		முழு j = (*this)(சாவி);
		எனில் (i != j)
			throw std::out_of_range("idict::expect()");
	}

	மாறிலி K &operator[](முழு index) மாறிலி
	{
		திரும்பு database.entries.at(index - offset).udata;
	}

	வெறுமை swap(idict &other)
	{
		database.swap(other.database);
	}

	வெறுமை reserve(size_t n) { database.reserve(n); }
	size_t size() மாறிலி { திரும்பு database.size(); }
	bool empty() மாறிலி { திரும்பு database.empty(); }
	வெறுமை clear() { database.clear(); }

	const_iterator begin() மாறிலி { திரும்பு const_iterator(*this, offset); }
	const_iterator element(முழு n) மாறிலி { திரும்பு const_iterator(*this, n); }
	const_iterator end() மாறிலி { திரும்பு const_iterator(*this, offset + size()); }
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
	பொது:
		explicit AtomicParent(முழு p) : parent(p) {}
		AtomicParent(மாறிலி AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(மாறிலி AtomicParent &other) { set(other.get()); திரும்பு *this; }
		முழு get() மாறிலி { திரும்பு parent.load(std::memory_order_relaxed); }
		வெறுமை set(முழு p) { parent.store(p, std::memory_order_relaxed); }
	தனிப்பட்ட:
		std::atomic<முழு> parent;
	};
	std::vector<AtomicParent> parents;

பொது:
	வகைவரை typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	முழு operator()(மாறிலி K &சாவி)
	{
		முழு i = database(சாவி);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		எனில் (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		திரும்பு i;
	}

	// Finds an element at given index
	மாறிலி K &operator[](முழு index) மாறிலி
	{
		திரும்பு database[index];
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
	முழு ifind(முழு i) மாறிலி
	{
		முழு p = i, k = i;

		போது (true) {
			முழு pp = parents[p].get();
			எனில் (pp < 0)
				நிறுத்து;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		போது (k != p) {
			முழு next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		திரும்பு p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	வெறுமை imerge(முழு i, முழு j)
	{
		i = ifind(i);
		j = ifind(j);

		எனில் (i != j)
			parents[i].set(j);
	}

	வெறுமை ipromote(முழு i)
	{
		முழு k = i;

		போது (k != -1) {
			முழு next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	முழு lookup(மாறிலி K &a)
	{
		திரும்பு ifind((*this)(a));
	}

	மாறிலி K &find(மாறிலி K &a) மாறிலி
	{
		முழு i = database.at(a, -1);
		எனில் (i < 0)
			திரும்பு a;
		திரும்பு (*this)[ifind(i)];
	}

	வெறுமை merge(மாறிலி K &a, மாறிலி K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	வெறுமை promote(மாறிலி K &a)
	{
		முழு i = database.at(a, -1);
		எனில் (i >= 0)
			ipromote(i);
	}

	வெறுமை swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	வெறுமை reserve(size_t n) { database.reserve(n); }
	size_t size() மாறிலி { திரும்பு database.size(); }
	bool empty() மாறிலி { திரும்பு database.empty(); }
	வெறுமை clear() { database.clear(); parents.clear(); }

	const_iterator begin() மாறிலி { திரும்பு database.begin(); }
	const_iterator element(முழு n) மாறிலி { திரும்பு database.element(n); }
	const_iterator end() மாறிலி { திரும்பு database.end(); }
};

} /* namespace hashlib */

#endif
