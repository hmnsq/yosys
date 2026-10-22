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

#شامل <array>
#شامل <atomic>
#شامل <stdexcept>
#شامل <algorithm>
#شامل <optional>
#شامل <string>
#شامل <variant>
#شامل <vector>
#شامل <type_traits>
#شامل <stdint.h>

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

#اگر defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#وگرنه
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ثابت صحیح hashtable_size_trigger = 2;
ثابت صحیح hashtable_size_factor = 3;

namespace legacy {
	درونخطی uint32_t djb2_add(uint32_t a, uint32_t b) {
		بازگشت ((a << 5) + a) + b;
	}
};

template<typename T>
ساختار hash_ops;

درونخطی unsigned صحیح mkhash_xorshift(unsigned صحیح a) {
	اگر (اندازه(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} وگرنه اگر (اندازه(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} وگرنه
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	بازگشت a;
}

class HasherDJB32 {
عمومی:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

خصوصی:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ایستا uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		بازگشت hash;
	}
عمومی:
	تهی hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		بازگشت;
	}
	تهی hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		بازگشت;
	}
	[[nodiscard]]
	hash_t yield() ثابت {
		بازگشت (hash_t)state;
	}

	template<typename T>
	تهی eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	تهی eat(ثابت T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	تهی commutative_eat(hash_t t) {
		state ^= t;
	}

	تهی force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_بالاترین_LOOP_FST [[nodiscard]] ایستا درونخطی Hasher hash
#define HASH_بالاترین_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	بازگشت h; \
}

template<typename T>
ساختار hash_ops {
	ایستا درونخطی bool cmp(ثابت T &a, ثابت T &b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت T &a, Hasher h) {
		اگر constexpr (std::is_integral_v<T>) {
			static_assert(اندازه(T) <= اندازه(uint64_t));
			اگر (اندازه(T) == اندازه(uint64_t))
				h.hash64(a);
			وگرنه
				h.hash32(a);
			بازگشت h;
		} وگرنه اگر constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			بازگشت hash_ops<u_type>::hash_into((u_type) a, h);
		} وگرنه اگر constexpr (std::is_pointer_v<T>) {
			بازگشت hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} وگرنه اگر constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحیح size = a.size();
			صحیح i = 0;
			وقتی (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			بازگشت h;
		} وگرنه {
			بازگشت a.hash_into(h);
		}
	}
	HASH_بالاترین_LOOP_FST (ثابت T &a) HASH_بالاترین_LOOP_SND
};

template<typename P, typename Q> ساختار hash_ops<std::pair<P, Q>> {
	ایستا درونخطی bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		بازگشت h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_بالاترین_LOOP_SND
	[[nodiscard]] ایستا درونخطی Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		بازگشت h;
	}
};

template<typename... T> ساختار hash_ops<std::tuple<T...>> {
	ایستا درونخطی bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		بازگشت a == b;
	}
	template<size_t I = 0>
	ایستا درونخطی typename std::enable_if<I == اندازه...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		بازگشت h;
	}
	template<size_t I = 0>
	ایستا درونخطی typename std::enable_if<I != اندازه...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعریفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		بازگشت h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::tuple<T...> &a) HASH_بالاترین_LOOP_SND
};

template<typename T> ساختار hash_ops<std::vector<T>> {
	ایستا درونخطی bool cmp(ثابت std::vector<T> &a, ثابت std::vector<T> &b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		برای (auto k : a)
			h.eat(k);
		بازگشت h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::vector<T> &a) HASH_بالاترین_LOOP_SND
};

template<typename T, size_t N> ساختار hash_ops<std::array<T, N>> {
    ایستا درونخطی bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        بازگشت a == b;
    }
    [[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        برای (ثابت auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        بازگشت h;
    }
	HASH_بالاترین_LOOP_FST (ثابت std::array<T, N> &a) HASH_بالاترین_LOOP_SND
};

ساختار hash_cstr_ops {
	ایستا درونخطی bool cmp(ثابت نویسه *a, ثابت نویسه *b) {
		بازگشت strcmp(a, b) == 0;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت نویسه *a, Hasher h) {
		وقتی (*a)
			h.hash32(*(a++));
		بازگشت h;
	}
	HASH_بالاترین_LOOP_FST (ثابت نویسه *a) HASH_بالاترین_LOOP_SND
};

template <> ساختار hash_ops<نویسه*> : hash_cstr_ops {};

ساختار hash_ptr_ops {
	ایستا درونخطی bool cmp(ثابت تهی *a, ثابت تهی *b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت تهی *a, Hasher h) {
		بازگشت hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_بالاترین_LOOP_FST (ثابت تهی *a) HASH_بالاترین_LOOP_SND
};

ساختار hash_obj_ops {
	ایستا درونخطی bool cmp(ثابت تهی *a, ثابت تهی *b) {
		بازگشت a == b;
	}
	template<typename T>
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت T *a, Hasher h) {
		اگر (a)
			h = a->hash_into(h);
		وگرنه
			h.eat(0);
		بازگشت h;
	}
	template<typename T>
	HASH_بالاترین_LOOP_FST (ثابت T *a) HASH_بالاترین_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ثابت T& obj) {
	بازگشت hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
درونخطی unsigned صحیح mkhash(ثابت T &v) {
	بازگشت (unsigned صحیح) run_hash<T>(v);
}

template<> ساختار hash_ops<std::monostate> {
	ایستا درونخطی bool cmp(std::monostate a, std::monostate b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(std::monostate, Hasher h) {
		بازگشت h;
	}
};

template<typename... T> ساختار hash_ops<std::variant<T...>> {
	ایستا درونخطی bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::visit([& h](ثابت auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		بازگشت h;
	}
};

template<typename T> ساختار hash_ops<std::optional<T>> {
	ایستا درونخطی bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		بازگشت a == b;
	}
	[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		اگر(a.has_مقدار())
			h.eat(*a);
		وگرنه
			h.eat(0);
		بازگشت h;
	}
};

درونخطی unsigned صحیح hashtable_size(unsigned صحیح min_size)
{
	// Primes as generated by https://oeis.org/A175953
	ایستا std::vector<unsigned صحیح> zero_and_some_primes = {
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

	برای (auto p : zero_and_some_primes)
		اگر (p >= min_size) بازگشت p;

	اگر (اندازه(unsigned صحیح) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	برای (auto p : zero_and_some_primes)
		اگر (100129 * p > min_size) بازگشت 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, صحیح offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عمومی:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	تهی eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	تهی eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		برای (auto b : buckets)
			h.eat(b);
		بازگشت h;
	}
خصوصی:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	ساختار entry_t
	{
		std::pair<K, T> udata;
		صحیح next;

		entry_t() { }
		entry_t(ثابت std::pair<K, T> &udata, صحیح next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, صحیح next) : udata(std::move(udata)), next(next) { }
		bool operator<(ثابت entry_t &other) ثابت { بازگشت udata.first < other.udata.first; }
	};

	std::vector<صحیح> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ایستا درونخطی تهی do_assert(bool) { }
#وگرنه
	ایستا درونخطی تهی do_assert(bool cond) {
		اگر (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		اگر (!hashtable.empty())
			hash = ops.hash(کلید).yield() % (unsigned صحیح)(hashtable.size());
		بازگشت hash;
	}

	تهی do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		برای (صحیح i = 0; i < صحیح(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < صحیح(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحیح do_erase(صحیح index, Hasher::hash_t hash)
	{
		do_assert(index < صحیح(entries.size()));
		اگر (hashtable.empty() || index < 0)
			بازگشت 0;

		صحیح k = hashtable[hash];
		do_assert(0 <= k && k < صحیح(entries.size()));

		اگر (k == index) {
			hashtable[hash] = entries[index].next;
		} وگرنه {
			وقتی (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < صحیح(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		صحیح back_idx = entries.size()-1;

		اگر (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < صحیح(entries.size()));

			اگر (k == back_idx) {
				hashtable[back_hash] = index;
			} وگرنه {
				وقتی (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < صحیح(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.برداشت_back();

		اگر (entries.empty())
			hashtable.clear();

		بازگشت 1;
	}

	صحیح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		اگر (hashtable.empty())
			بازگشت -1;

		اگر (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		بازگشت do_lookup_internal(کلید, hash);
	}

	صحیح do_lookup_internal(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحیح index = hashtable[hash];

		وقتی (index >= 0 && !ops.cmp(entries[index].udata.first, کلید)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < صحیح(entries.size()));
		}

		بازگشت index;
	}

	صحیح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		اگر (hashtable.empty())
			بازگشت -1;

		بازگشت do_lookup_internal(کلید, hash);
	}

	صحیح do_درج(ثابت K &کلید, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(کلید, T()), -1);
			do_rehash();
		} وگرنه {
			entries.emplace_back(std::pair<K, T>(کلید, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		بازگشت entries.size() - 1;
	}

	صحیح do_درج(ثابت std::pair<K, T> &مقدار, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.empty()) {
			entries.emplace_back(مقدار, -1);
			do_rehash();
		} وگرنه {
			entries.emplace_back(مقدار, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		بازگشت entries.size() - 1;
	}

	صحیح do_درج(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} وگرنه {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		بازگشت entries.size() - 1;
	}

عمومی:
	class const_iterator
	{
		friend class dict;
	محافظتشده:
		ثابت dict *ptr;
		صحیح index;
		const_iterator(ثابت dict *ptr, صحیح index) : ptr(ptr), index(index) { }
	عمومی:
		تعریفنوع std::bidirectional_iterator_tag iterator_category;
		تعریفنوع std::pair<K, T> مقدار_type;
		تعریفنوع ptrdiff_t difference_type;
		تعریفنوع ثابت std::pair<K, T>* pointer;
		تعریفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; بازگشت *this; }
		const_iterator operator++(صحیح) { const_iterator tmp = *this; index--; بازگشت tmp; }
		const_iterator operator--() { index++; بازگشت *this; }
		const_iterator operator--(صحیح) { const_iterator tmp = *this; index++; بازگشت tmp; }
		const_iterator operator+=(صحیح amt) { index -= amt; بازگشت *this; }
		bool operator<(ثابت const_iterator &other) ثابت { بازگشت index > other.index; }
		bool operator==(ثابت const_iterator &other) ثابت { بازگشت index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { بازگشت index != other.index; }
		ثابت std::pair<K, T> &operator*() ثابت { بازگشت ptr->entries[index].udata; }
		ثابت std::pair<K, T> *operator->() ثابت { بازگشت &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	محافظتشده:
		dict *ptr;
		صحیح index;
		iterator(dict *ptr, صحیح index) : ptr(ptr), index(index) { }
	عمومی:
		تعریفنوع std::forward_iterator_tag iterator_category;
		تعریفنوع std::pair<K, T> مقدار_type;
		تعریفنوع ptrdiff_t difference_type;
		تعریفنوع std::pair<K, T>* pointer;
		تعریفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; بازگشت *this; }
		iterator operator+=(صحیح amt) { index -= amt; بازگشت *this; }
		bool operator<(ثابت iterator &other) ثابت { بازگشت index > other.index; }
		bool operator==(ثابت iterator &other) ثابت { بازگشت index == other.index; }
		bool operator!=(ثابت iterator &other) ثابت { بازگشت index != other.index; }
		std::pair<K, T> &operator*() { بازگشت ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { بازگشت &ptr->entries[index].udata; }
		ثابت std::pair<K, T> &operator*() ثابت { بازگشت ptr->entries[index].udata; }
		ثابت std::pair<K, T> *operator->() ثابت { بازگشت &ptr->entries[index].udata; }
		operator const_iterator() ثابت { بازگشت const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ثابت {
		بازگشت std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ثابت {
		بازگشت std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ثابت dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		بازگشت *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		بازگشت *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		برای (auto &it : list)
			درج(it);
	}

	template<class درون_داد_Iterator>
	dict(درون_داد_Iterator first, درون_داد_Iterator last)
	{
		درج(first, last);
	}

	template<class درون_داد_Iterator>
	تهی درج(درون_داد_Iterator first, درون_داد_Iterator last)
	{
		برای (; first != last; ++first)
			درج(*first);
	}

	std::pair<iterator, bool> درج(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(کلید, hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(ثابت std::pair<K, T> &مقدار)
	{
		Hasher::hash_t hash = do_hash(مقدار.first);
		صحیح i = do_lookup(مقدار.first, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(مقدار, hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		صحیح i = do_lookup(rvalue.first, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::forward<std::pair<K, T>>(rvalue), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T ثابت &مقدار)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(کلید, مقدار), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(کلید, std::forward<T>(rvalue)), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &مقدار)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحیح i = do_lookup(rkey, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(std::forward<K>(rkey), مقدار), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحیح i = do_lookup(rkey, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحیح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح index = do_lookup(کلید, hash);
		بازگشت do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		بازگشت ++it;
	}

	صحیح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		بازگشت i < 0 ? 0 : 1;
	}

	صحیح شمار(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		بازگشت i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			بازگشت end();
		بازگشت iterator(this, i);
	}

	const_iterator find(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			بازگشت end();
		بازگشت const_iterator(this, i);
	}

	T& at(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			throw std::out_of_range("dict::at()");
		بازگشت entries[i].udata.second;
	}

	ثابت T& at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			throw std::out_of_range("dict::at()");
		بازگشت entries[i].udata.second;
	}

	ثابت T& at(ثابت K &کلید, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			بازگشت defval;
		بازگشت entries[i].udata.second;
	}

	T& operator[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			i = do_درج(std::pair<K, T>(کلید, T()), hash);
		بازگشت entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	تهی sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ثابت entry_t &a, ثابت entry_t &b){ بازگشت comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	تهی swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ثابت dict &other) ثابت {
		اگر (size() != other.size())
			بازگشت false;
		برای (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			اگر (oit == other.end() || !(oit->second == it.udata.second))
				بازگشت false;
		}
		بازگشت true;
	}

	bool operator!=(ثابت dict &other) ثابت {
		بازگشت !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		برای (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		بازگشت comm.hash_into(h);
	}

	تهی reserve(size_t n) { entries.reserve(n); }
	size_t size() ثابت { بازگشت entries.size(); }
	bool empty() ثابت { بازگشت entries.empty(); }
	تهی clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { بازگشت iterator(this, صحیح(entries.size())-1); }
	iterator element(صحیح n) { بازگشت iterator(this, صحیح(entries.size())-1-n); }
	iterator end() { بازگشت iterator(nullptr, -1); }

	const_iterator begin() ثابت { بازگشت const_iterator(this, صحیح(entries.size())-1); }
	const_iterator element(صحیح n) ثابت { بازگشت const_iterator(this, صحیح(entries.size())-1-n); }
	const_iterator end() ثابت { بازگشت const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, صحیح, typename> friend class idict;

محافظتشده:
	ساختار entry_t
	{
		K udata;
		صحیح next;

		entry_t() { }
		entry_t(ثابت K &udata, صحیح next) : udata(udata), next(next) { }
		entry_t(K &&udata, صحیح next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<صحیح> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ایستا درونخطی تهی do_assert(bool) { }
#وگرنه
	ایستا درونخطی تهی do_assert(bool cond) {
		اگر (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		اگر (!hashtable.empty())
			hash = ops.hash(کلید).yield() % (unsigned صحیح)(hashtable.size());
		بازگشت hash;
	}

	تهی do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		برای (صحیح i = 0; i < صحیح(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < صحیح(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحیح do_erase(صحیح index, Hasher::hash_t hash)
	{
		do_assert(index < صحیح(entries.size()));
		اگر (hashtable.empty() || index < 0)
			بازگشت 0;

		صحیح k = hashtable[hash];
		اگر (k == index) {
			hashtable[hash] = entries[index].next;
		} وگرنه {
			وقتی (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < صحیح(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		صحیح back_idx = entries.size()-1;

		اگر (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			اگر (k == back_idx) {
				hashtable[back_hash] = index;
			} وگرنه {
				وقتی (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < صحیح(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.برداشت_back();

		اگر (entries.empty())
			hashtable.clear();

		بازگشت 1;
	}

	صحیح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		اگر (hashtable.empty())
			بازگشت -1;

		اگر (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		بازگشت do_lookup_internal(کلید, hash);
	}

	صحیح do_lookup_internal(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحیح index = hashtable[hash];

		وقتی (index >= 0 && !ops.cmp(entries[index].udata, کلید)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < صحیح(entries.size()));
		}

		بازگشت index;
	}

	صحیح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		اگر (hashtable.empty())
			بازگشت -1;

		بازگشت do_lookup_internal(کلید, hash);
	}

	صحیح do_درج(ثابت K &مقدار, Hasher::hash_t &hash)
	{
		اگر (hashtable.empty()) {
			entries.emplace_back(مقدار, -1);
			do_rehash();
			hash = do_hash(مقدار);
		} وگرنه {
			entries.emplace_back(مقدار, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		بازگشت entries.size() - 1;
	}

	صحیح do_درج(K &&rvalue, Hasher::hash_t &hash)
	{
		اگر (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} وگرنه {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		بازگشت entries.size() - 1;
	}

عمومی:
	class const_iterator
	{
		friend class pool;
	محافظتشده:
		ثابت pool *ptr;
		صحیح index;
		const_iterator(ثابت pool *ptr, صحیح index) : ptr(ptr), index(index) { }
	عمومی:
		تعریفنوع std::forward_iterator_tag iterator_category;
		تعریفنوع K مقدار_type;
		تعریفنوع ptrdiff_t difference_type;
		تعریفنوع K* pointer;
		تعریفنوع K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; بازگشت *this; }
		bool operator==(ثابت const_iterator &other) ثابت { بازگشت index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { بازگشت index != other.index; }
		ثابت K &operator*() ثابت { بازگشت ptr->entries[index].udata; }
		ثابت K *operator->() ثابت { بازگشت &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	محافظتشده:
		pool *ptr;
		صحیح index;
		iterator(pool *ptr, صحیح index) : ptr(ptr), index(index) { }
	عمومی:
		تعریفنوع std::forward_iterator_tag iterator_category;
		تعریفنوع K مقدار_type;
		تعریفنوع ptrdiff_t difference_type;
		تعریفنوع K* pointer;
		تعریفنوع K& reference;
		iterator() { }
		iterator operator++() { index--; بازگشت *this; }
		bool operator==(ثابت iterator &other) ثابت { بازگشت index == other.index; }
		bool operator!=(ثابت iterator &other) ثابت { بازگشت index != other.index; }
		K &operator*() { بازگشت ptr->entries[index].udata; }
		K *operator->() { بازگشت &ptr->entries[index].udata; }
		ثابت K &operator*() ثابت { بازگشت ptr->entries[index].udata; }
		ثابت K *operator->() ثابت { بازگشت &ptr->entries[index].udata; }
		operator const_iterator() ثابت { بازگشت const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(ثابت pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		بازگشت *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		بازگشت *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		برای (auto &it : list)
			درج(it);
	}

	template<class درون_داد_Iterator>
	pool(درون_داد_Iterator first, درون_داد_Iterator last)
	{
		درج(first, last);
	}

	template<class درون_داد_Iterator>
	تهی درج(درون_داد_Iterator first, درون_داد_Iterator last)
	{
		برای (; first != last; ++first)
			درج(*first);
	}

	std::pair<iterator, bool> درج(ثابت K &مقدار)
	{
		Hasher::hash_t hash = do_hash(مقدار);
		صحیح i = do_lookup(مقدار, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(مقدار, hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحیح i = do_lookup(rvalue, hash);
		اگر (i >= 0)
			بازگشت std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::forward<K>(rvalue), hash);
		بازگشت std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		بازگشت درج(K(std::forward<Args>(args)...));
	}

	صحیح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح index = do_lookup(کلید, hash);
		بازگشت do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		بازگشت ++it;
	}

	صحیح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		بازگشت i < 0 ? 0 : 1;
	}

	صحیح شمار(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		بازگشت i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			بازگشت end();
		بازگشت iterator(this, i);
	}

	const_iterator find(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			بازگشت end();
		بازگشت const_iterator(this, i);
	}

	bool operator[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		بازگشت i >= 0;
	}

	template<typename Compare = std::less<K>>
	تهی sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ثابت entry_t &a, ثابت entry_t &b){ بازگشت comp(b.udata, a.udata); });
		do_rehash();
	}

	K برداشت()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		بازگشت ret;
	}

	تهی swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ثابت pool &other) ثابت {
		اگر (size() != other.size())
			بازگشت false;
		برای (auto &it : entries)
			اگر (!other.شمار(it.udata))
				بازگشت false;
		بازگشت true;
	}

	bool operator!=(ثابت pool &other) ثابت {
		بازگشت !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		برای (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		بازگشت comm.hash_into(h);
	}

	تهی reserve(size_t n) { entries.reserve(n); }
	size_t size() ثابت { بازگشت entries.size(); }
	bool empty() ثابت { بازگشت entries.empty(); }
	تهی clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { بازگشت iterator(this, صحیح(entries.size())-1); }
	iterator element(صحیح n) { بازگشت iterator(this, صحیح(entries.size())-1-n); }
	iterator end() { بازگشت iterator(nullptr, -1); }

	const_iterator begin() ثابت { بازگشت const_iterator(this, صحیح(entries.size())-1); }
	const_iterator element(صحیح n) ثابت { بازگشت const_iterator(this, صحیح(entries.size())-1-n); }
	const_iterator end() ثابت { بازگشت const_iterator(nullptr, -1); }
};

template<typename K, صحیح offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

عمومی:
	class const_iterator
	{
		friend class idict;
	محافظتشده:
		ثابت idict &container;
		صحیح index;
		const_iterator(ثابت idict &container, صحیح index) : container(container), index(index) { }
	عمومی:
		تعریفنوع std::forward_iterator_tag iterator_category;
		تعریفنوع K مقدار_type;
		تعریفنوع ptrdiff_t difference_type;
		تعریفنوع K* pointer;
		تعریفنوع K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; بازگشت *this; }
		bool operator==(ثابت const_iterator &other) ثابت { بازگشت index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { بازگشت index != other.index; }
		ثابت K &operator*() ثابت { بازگشت container[index]; }
		ثابت K *operator->() ثابت { بازگشت &container[index]; }
	};

	constexpr idict()
	{
	}

	صحیح operator()(ثابت K &کلید)
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup(کلید, hash);
		اگر (i < 0)
			i = database.do_درج(کلید, hash);
		بازگشت i + offset;
	}

	صحیح at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			throw std::out_of_range("idict::at()");
		بازگشت i + offset;
	}

	صحیح at(ثابت K &کلید, صحیح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			بازگشت defval;
		بازگشت i + offset;
	}

	صحیح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		بازگشت i < 0 ? 0 : 1;
	}

	صحیح lookup(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		بازگشت database.do_lookup_no_rehash(کلید, hash);
	}

	تهی expect(ثابت K &کلید, صحیح i)
	{
		صحیح j = (*this)(کلید);
		اگر (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ثابت K &operator[](صحیح index) ثابت
	{
		بازگشت database.entries.at(index - offset).udata;
	}

	تهی swap(idict &other)
	{
		database.swap(other.database);
	}

	تهی reserve(size_t n) { database.reserve(n); }
	size_t size() ثابت { بازگشت database.size(); }
	bool empty() ثابت { بازگشت database.empty(); }
	تهی clear() { database.clear(); }

	const_iterator begin() ثابت { بازگشت const_iterator(*this, offset); }
	const_iterator element(صحیح n) ثابت { بازگشت const_iterator(*this, n); }
	const_iterator end() ثابت { بازگشت const_iterator(*this, offset + size()); }
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
	عمومی:
		explicit AtomicParent(صحیح p) : parent(p) {}
		AtomicParent(ثابت AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ثابت AtomicParent &other) { set(other.get()); بازگشت *this; }
		صحیح get() ثابت { بازگشت parent.load(std::memory_order_relaxed); }
		تهی set(صحیح p) { parent.store(p, std::memory_order_relaxed); }
	خصوصی:
		std::atomic<صحیح> parent;
	};
	std::vector<AtomicParent> parents;

عمومی:
	تعریفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحیح operator()(ثابت K &کلید)
	{
		صحیح i = database(کلید);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		اگر (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		بازگشت i;
	}

	// Finds an element at given index
	ثابت K &operator[](صحیح index) ثابت
	{
		بازگشت database[index];
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
	صحیح ifind(صحیح i) ثابت
	{
		صحیح p = i, k = i;

		وقتی (true) {
			صحیح pp = parents[p].get();
			اگر (pp < 0)
				بشکن;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		وقتی (k != p) {
			صحیح next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		بازگشت p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	تهی imerge(صحیح i, صحیح j)
	{
		i = ifind(i);
		j = ifind(j);

		اگر (i != j)
			parents[i].set(j);
	}

	تهی ipromote(صحیح i)
	{
		صحیح k = i;

		وقتی (k != -1) {
			صحیح next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	صحیح lookup(ثابت K &a)
	{
		بازگشت ifind((*this)(a));
	}

	ثابت K &find(ثابت K &a) ثابت
	{
		صحیح i = database.at(a, -1);
		اگر (i < 0)
			بازگشت a;
		بازگشت (*this)[ifind(i)];
	}

	تهی merge(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	تهی promote(ثابت K &a)
	{
		صحیح i = database.at(a, -1);
		اگر (i >= 0)
			ipromote(i);
	}

	تهی swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	تهی reserve(size_t n) { database.reserve(n); }
	size_t size() ثابت { بازگشت database.size(); }
	bool empty() ثابت { بازگشت database.empty(); }
	تهی clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { بازگشت database.begin(); }
	const_iterator element(صحیح n) ثابت { بازگشت database.element(n); }
	const_iterator end() ثابت { بازگشت database.end(); }
};

} /* namespace hashlib */

#endif
