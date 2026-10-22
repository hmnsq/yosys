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

#ضمن <array>
#ضمن <atomic>
#ضمن <stdexcept>
#ضمن <algorithm>
#ضمن <optional>
#ضمن <string>
#ضمن <variant>
#ضمن <vector>
#ضمن <type_traits>
#ضمن <stdint.h>

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

#אם defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#אחרת
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ثابت صحيح hashtable_size_trigger = 2;
ثابت صحيح hashtable_size_factor = 3;

namespace legacy {
	مضمن uint32_t djb2_add(uint32_t a, uint32_t b) {
		החזר ((a << 5) + a) + b;
	}
};

template<typename T>
بنية hash_ops;

مضمن unsigned صحيح mkhash_xorshift(unsigned صحيح a) {
	אם (حجم(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} אחרת אם (حجم(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} אחרת
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	החזר a;
}

class HasherDJB32 {
عام:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

خاص:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ساكن uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		החזר hash;
	}
عام:
	فراغ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		החזר;
	}
	فراغ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		החזר;
	}
	[[nodiscard]]
	hash_t yield() ثابت {
		החזר (hash_t)state;
	}

	template<typename T>
	فراغ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	فراغ eat(ثابت T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	فراغ commutative_eat(hash_t t) {
		state ^= t;
	}

	فراغ force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_TOP_LOOP_FST [[nodiscard]] ساكن مضمن Hasher hash
#define HASH_TOP_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	החזר h; \
}

template<typename T>
بنية hash_ops {
	ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
		אם constexpr (std::is_integral_v<T>) {
			static_assert(حجم(T) <= حجم(uint64_t));
			אם (حجم(T) == حجم(uint64_t))
				h.hash64(a);
			אחרת
				h.hash32(a);
			החזר h;
		} אחרת אם constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			החזר hash_ops<u_type>::hash_into((u_type) a, h);
		} אחרת אם constexpr (std::is_pointer_v<T>) {
			החזר hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} אחרת אם constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحيح size = a.size();
			صحيح i = 0;
			כלעוד (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			החזר h;
		} אחרת {
			החזר a.hash_into(h);
		}
	}
	HASH_TOP_LOOP_FST (ثابت T &a) HASH_TOP_LOOP_SND
};

template<typename P, typename Q> بنية hash_ops<std::pair<P, Q>> {
	ساكن مضمن bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		החזר h;
	}
	HASH_TOP_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_TOP_LOOP_SND
	[[nodiscard]] ساكن مضمن Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		החזר h;
	}
};

template<typename... T> بنية hash_ops<std::tuple<T...>> {
	ساكن مضمن bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		החזר a == b;
	}
	template<size_t I = 0>
	ساكن مضمن typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		החזר h;
	}
	template<size_t I = 0>
	ساكن مضمن typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعريفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		החזר h;
	}
	HASH_TOP_LOOP_FST (ثابت std::tuple<T...> &a) HASH_TOP_LOOP_SND
};

template<typename T> بنية hash_ops<std::vector<T>> {
	ساكن مضمن bool cmp(ثابت std::vector<T> &a, ثابت std::vector<T> &b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		עבור (auto k : a)
			h.eat(k);
		החזר h;
	}
	HASH_TOP_LOOP_FST (ثابت std::vector<T> &a) HASH_TOP_LOOP_SND
};

template<typename T, size_t N> بنية hash_ops<std::array<T, N>> {
    ساكن مضمن bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        החזר a == b;
    }
    [[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        עבור (ثابت auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        החזר h;
    }
	HASH_TOP_LOOP_FST (ثابت std::array<T, N> &a) HASH_TOP_LOOP_SND
};

بنية hash_cstr_ops {
	ساكن مضمن bool cmp(ثابت حرف *a, ثابت حرف *b) {
		החזר strcmp(a, b) == 0;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت حرف *a, Hasher h) {
		כלעוד (*a)
			h.hash32(*(a++));
		החזר h;
	}
	HASH_TOP_LOOP_FST (ثابت حرف *a) HASH_TOP_LOOP_SND
};

template <> بنية hash_ops<حرف*> : hash_cstr_ops {};

بنية hash_ptr_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت فراغ *a, Hasher h) {
		החזר hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_TOP_LOOP_FST (ثابت فراغ *a) HASH_TOP_LOOP_SND
};

بنية hash_obj_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		החזר a == b;
	}
	template<typename T>
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T *a, Hasher h) {
		אם (a)
			h = a->hash_into(h);
		אחרת
			h.eat(0);
		החזר h;
	}
	template<typename T>
	HASH_TOP_LOOP_FST (ثابت T *a) HASH_TOP_LOOP_SND
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
	החזר hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
مضمن unsigned صحيح mkhash(ثابت T &v) {
	החזר (unsigned صحيح) run_hash<T>(v);
}

template<> بنية hash_ops<std::monostate> {
	ساكن مضمن bool cmp(std::monostate a, std::monostate b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(std::monostate, Hasher h) {
		החזר h;
	}
};

template<typename... T> بنية hash_ops<std::variant<T...>> {
	ساكن مضمن bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::visit([& h](ثابت auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		החזר h;
	}
};

template<typename T> بنية hash_ops<std::optional<T>> {
	ساكن مضمن bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		החזר a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		אם(a.has_value())
			h.eat(*a);
		אחרת
			h.eat(0);
		החזר h;
	}
};

مضمن unsigned صحيح hashtable_size(unsigned صحيح min_size)
{
	// Primes as generated by https://oeis.org/A175953
	ساكن std::vector<unsigned صحيح> zero_and_some_primes = {
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

	עבור (auto p : zero_and_some_primes)
		אם (p >= min_size) החזר p;

	אם (حجم(unsigned صحيح) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	עבור (auto p : zero_and_some_primes)
		אם (100129 * p > min_size) החזר 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, صحيح offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عام:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	فراغ eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	فراغ eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		עבור (auto b : buckets)
			h.eat(b);
		החזר h;
	}
خاص:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	بنية entry_t
	{
		std::pair<K, T> udata;
		صحيح next;

		entry_t() { }
		entry_t(ثابت std::pair<K, T> &udata, صحيح next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, صحيح next) : udata(std::move(udata)), next(next) { }
		bool operator<(ثابت entry_t &other) ثابت { החזר udata.first < other.udata.first; }
	};

	std::vector<صحيح> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#אחרת
	ساكن مضمن فراغ do_assert(bool cond) {
		אם (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = 0;
		אם (!hashtable.empty())
			hash = ops.hash(key).yield() % (unsigned صحيح)(hashtable.size());
		החזר hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		עבור (صحيح i = 0; i < صحيح(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < صحيح(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح index, Hasher::hash_t hash)
	{
		do_assert(index < صحيح(entries.size()));
		אם (hashtable.empty() || index < 0)
			החזר 0;

		صحيح k = hashtable[hash];
		do_assert(0 <= k && k < صحيح(entries.size()));

		אם (k == index) {
			hashtable[hash] = entries[index].next;
		} אחרת {
			כלעוד (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < صحيح(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		صحيح back_idx = entries.size()-1;

		אם (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < صحيح(entries.size()));

			אם (k == back_idx) {
				hashtable[back_hash] = index;
			} אחרת {
				כלעוד (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < صحيح(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.pop_back();

		אם (entries.empty())
			hashtable.clear();

		החזר 1;
	}

	صحيح do_lookup(ثابت K &key, Hasher::hash_t &hash)
	{
		אם (hashtable.empty())
			החזר -1;

		אם (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(key);
		}

		החזר do_lookup_internal(key, hash);
	}

	صحيح do_lookup_internal(ثابت K &key, Hasher::hash_t hash) ثابت
	{
		صحيح index = hashtable[hash];

		כלעוד (index >= 0 && !ops.cmp(entries[index].udata.first, key)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < صحيح(entries.size()));
		}

		החזר index;
	}

	صحيح do_lookup_no_rehash(ثابت K &key, Hasher::hash_t hash) ثابت
	{
		אם (hashtable.empty())
			החזר -1;

		החזר do_lookup_internal(key, hash);
	}

	صحيح do_insert(ثابت K &key, ثابت Hasher::hash_t &hash)
	{
		אם (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(key, T()), -1);
			do_rehash();
		} אחרת {
			entries.emplace_back(std::pair<K, T>(key, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		החזר entries.size() - 1;
	}

	صحيح do_insert(ثابت std::pair<K, T> &value, ثابت Hasher::hash_t &hash)
	{
		אם (hashtable.empty()) {
			entries.emplace_back(value, -1);
			do_rehash();
		} אחרת {
			entries.emplace_back(value, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		החזר entries.size() - 1;
	}

	صحيح do_insert(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		אם (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} אחרת {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		החזר entries.size() - 1;
	}

عام:
	class const_iterator
	{
		friend class dict;
	محمي:
		ثابت dict *ptr;
		صحيح index;
		const_iterator(ثابت dict *ptr, صحيح index) : ptr(ptr), index(index) { }
	عام:
		تعريفنوع std::bidirectional_iterator_tag iterator_category;
		تعريفنوع std::pair<K, T> value_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع ثابت std::pair<K, T>* pointer;
		تعريفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; החזר *this; }
		const_iterator operator++(صحيح) { const_iterator tmp = *this; index--; החזר tmp; }
		const_iterator operator--() { index++; החזר *this; }
		const_iterator operator--(صحيح) { const_iterator tmp = *this; index++; החזר tmp; }
		const_iterator operator+=(صحيح amt) { index -= amt; החזר *this; }
		bool operator<(ثابت const_iterator &other) ثابت { החזר index > other.index; }
		bool operator==(ثابت const_iterator &other) ثابت { החזר index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { החזר index != other.index; }
		ثابت std::pair<K, T> &operator*() ثابت { החזר ptr->entries[index].udata; }
		ثابت std::pair<K, T> *operator->() ثابت { החזר &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	محمي:
		dict *ptr;
		صحيح index;
		iterator(dict *ptr, صحيح index) : ptr(ptr), index(index) { }
	عام:
		تعريفنوع std::forward_iterator_tag iterator_category;
		تعريفنوع std::pair<K, T> value_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع std::pair<K, T>* pointer;
		تعريفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; החזר *this; }
		iterator operator+=(صحيح amt) { index -= amt; החזר *this; }
		bool operator<(ثابت iterator &other) ثابت { החזר index > other.index; }
		bool operator==(ثابت iterator &other) ثابت { החזר index == other.index; }
		bool operator!=(ثابت iterator &other) ثابت { החזר index != other.index; }
		std::pair<K, T> &operator*() { החזר ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { החזר &ptr->entries[index].udata; }
		ثابت std::pair<K, T> &operator*() ثابت { החזר ptr->entries[index].udata; }
		ثابت std::pair<K, T> *operator->() ثابت { החזר &ptr->entries[index].udata; }
		operator const_iterator() ثابت { החזר const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ثابت {
		החזר std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ثابت {
		החזר std::make_reverse_iterator(begin());
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
		החזר *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		החזר *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		עבור (auto &it : list)
			insert(it);
	}

	template<class InputIterator>
	dict(InputIterator first, InputIterator last)
	{
		insert(first, last);
	}

	template<class InputIterator>
	فراغ insert(InputIterator first, InputIterator last)
	{
		עבור (; first != last; ++first)
			insert(*first);
	}

	std::pair<iterator, bool> insert(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(key, hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(ثابت std::pair<K, T> &value)
	{
		Hasher::hash_t hash = do_hash(value.first);
		صحيح i = do_lookup(value.first, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(value, hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		صحيح i = do_lookup(rvalue.first, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::forward<std::pair<K, T>>(rvalue), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &key, T ثابت &value)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(key, value), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &key, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(key, std::forward<T>(rvalue)), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &value)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(std::forward<K>(rkey), value), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحيح erase(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح index = do_lookup(key, hash);
		החזר do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		החזר ++it;
	}

	صحيح count(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		החזר i < 0 ? 0 : 1;
	}

	صحيح count(ثابت K &key, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		החזר i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i < 0)
			החזר end();
		החזר iterator(this, i);
	}

	const_iterator find(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		אם (i < 0)
			החזר end();
		החזר const_iterator(this, i);
	}

	T& at(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i < 0)
			throw std::out_of_range("dict::at()");
		החזר entries[i].udata.second;
	}

	ثابت T& at(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		אם (i < 0)
			throw std::out_of_range("dict::at()");
		החזר entries[i].udata.second;
	}

	ثابت T& at(ثابت K &key, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		אם (i < 0)
			החזר defval;
		החזר entries[i].udata.second;
	}

	T& operator[](ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i < 0)
			i = do_insert(std::pair<K, T>(key, T()), hash);
		החזר entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	فراغ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ثابت entry_t &a, ثابت entry_t &b){ החזר comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	فراغ swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ثابت dict &other) ثابت {
		אם (size() != other.size())
			החזר false;
		עבור (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			אם (oit == other.end() || !(oit->second == it.udata.second))
				החזר false;
		}
		החזר true;
	}

	bool operator!=(ثابت dict &other) ثابت {
		החזר !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		עבור (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		החזר comm.hash_into(h);
	}

	فراغ reserve(size_t n) { entries.reserve(n); }
	size_t size() ثابت { החזר entries.size(); }
	bool empty() ثابت { החזר entries.empty(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { החזר iterator(this, صحيح(entries.size())-1); }
	iterator element(صحيح n) { החזר iterator(this, صحيح(entries.size())-1-n); }
	iterator end() { החזר iterator(nullptr, -1); }

	const_iterator begin() ثابت { החזר const_iterator(this, صحيح(entries.size())-1); }
	const_iterator element(صحيح n) ثابت { החזר const_iterator(this, صحيح(entries.size())-1-n); }
	const_iterator end() ثابت { החזר const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, صحيح, typename> friend class idict;

محمي:
	بنية entry_t
	{
		K udata;
		صحيح next;

		entry_t() { }
		entry_t(ثابت K &udata, صحيح next) : udata(udata), next(next) { }
		entry_t(K &&udata, صحيح next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<صحيح> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#אחרת
	ساكن مضمن فراغ do_assert(bool cond) {
		אם (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = 0;
		אם (!hashtable.empty())
			hash = ops.hash(key).yield() % (unsigned صحيح)(hashtable.size());
		החזר hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		עבור (صحيح i = 0; i < صحيح(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < صحيح(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح index, Hasher::hash_t hash)
	{
		do_assert(index < صحيح(entries.size()));
		אם (hashtable.empty() || index < 0)
			החזר 0;

		صحيح k = hashtable[hash];
		אם (k == index) {
			hashtable[hash] = entries[index].next;
		} אחרת {
			כלעוד (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < صحيح(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		صحيح back_idx = entries.size()-1;

		אם (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			אם (k == back_idx) {
				hashtable[back_hash] = index;
			} אחרת {
				כלעוד (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < صحيح(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.pop_back();

		אם (entries.empty())
			hashtable.clear();

		החזר 1;
	}

	صحيح do_lookup(ثابت K &key, Hasher::hash_t &hash)
	{
		אם (hashtable.empty())
			החזר -1;

		אם (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(key);
		}

		החזר do_lookup_internal(key, hash);
	}

	صحيح do_lookup_internal(ثابت K &key, Hasher::hash_t hash) ثابت
	{
		صحيح index = hashtable[hash];

		כלעוד (index >= 0 && !ops.cmp(entries[index].udata, key)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < صحيح(entries.size()));
		}

		החזר index;
	}

	صحيح do_lookup_no_rehash(ثابت K &key, Hasher::hash_t hash) ثابت
	{
		אם (hashtable.empty())
			החזר -1;

		החזר do_lookup_internal(key, hash);
	}

	صحيح do_insert(ثابت K &value, Hasher::hash_t &hash)
	{
		אם (hashtable.empty()) {
			entries.emplace_back(value, -1);
			do_rehash();
			hash = do_hash(value);
		} אחרת {
			entries.emplace_back(value, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		החזר entries.size() - 1;
	}

	صحيح do_insert(K &&rvalue, Hasher::hash_t &hash)
	{
		אם (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} אחרת {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		החזר entries.size() - 1;
	}

عام:
	class const_iterator
	{
		friend class pool;
	محمي:
		ثابت pool *ptr;
		صحيح index;
		const_iterator(ثابت pool *ptr, صحيح index) : ptr(ptr), index(index) { }
	عام:
		تعريفنوع std::forward_iterator_tag iterator_category;
		تعريفنوع K value_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* pointer;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; החזר *this; }
		bool operator==(ثابت const_iterator &other) ثابت { החזר index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { החזר index != other.index; }
		ثابت K &operator*() ثابت { החזר ptr->entries[index].udata; }
		ثابت K *operator->() ثابت { החזר &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	محمي:
		pool *ptr;
		صحيح index;
		iterator(pool *ptr, صحيح index) : ptr(ptr), index(index) { }
	عام:
		تعريفنوع std::forward_iterator_tag iterator_category;
		تعريفنوع K value_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* pointer;
		تعريفنوع K& reference;
		iterator() { }
		iterator operator++() { index--; החזר *this; }
		bool operator==(ثابت iterator &other) ثابت { החזר index == other.index; }
		bool operator!=(ثابت iterator &other) ثابت { החזר index != other.index; }
		K &operator*() { החזר ptr->entries[index].udata; }
		K *operator->() { החזר &ptr->entries[index].udata; }
		ثابت K &operator*() ثابت { החזר ptr->entries[index].udata; }
		ثابت K *operator->() ثابت { החזר &ptr->entries[index].udata; }
		operator const_iterator() ثابت { החזר const_iterator(ptr, index); }
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
		החזר *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		החזר *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		עבור (auto &it : list)
			insert(it);
	}

	template<class InputIterator>
	pool(InputIterator first, InputIterator last)
	{
		insert(first, last);
	}

	template<class InputIterator>
	فراغ insert(InputIterator first, InputIterator last)
	{
		עבור (; first != last; ++first)
			insert(*first);
	}

	std::pair<iterator, bool> insert(ثابت K &value)
	{
		Hasher::hash_t hash = do_hash(value);
		صحيح i = do_lookup(value, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(value, hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> insert(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحيح i = do_lookup(rvalue, hash);
		אם (i >= 0)
			החזר std::pair<iterator, bool>(iterator(this, i), false);
		i = do_insert(std::forward<K>(rvalue), hash);
		החזר std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		החזר insert(K(std::forward<Args>(args)...));
	}

	صحيح erase(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح index = do_lookup(key, hash);
		החזר do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		החזר ++it;
	}

	صحيح count(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		החזר i < 0 ? 0 : 1;
	}

	صحيح count(ثابت K &key, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		החזר i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		אם (i < 0)
			החזר end();
		החזר iterator(this, i);
	}

	const_iterator find(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup_no_rehash(key, hash);
		אם (i < 0)
			החזר end();
		החזר const_iterator(this, i);
	}

	bool operator[](ثابت K &key)
	{
		Hasher::hash_t hash = do_hash(key);
		صحيح i = do_lookup(key, hash);
		החזר i >= 0;
	}

	template<typename Compare = std::less<K>>
	فراغ sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ثابت entry_t &a, ثابت entry_t &b){ החזר comp(b.udata, a.udata); });
		do_rehash();
	}

	K pop()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		החזר ret;
	}

	فراغ swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ثابت pool &other) ثابت {
		אם (size() != other.size())
			החזר false;
		עבור (auto &it : entries)
			אם (!other.count(it.udata))
				החזר false;
		החזר true;
	}

	bool operator!=(ثابت pool &other) ثابت {
		החזר !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		עבור (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		החזר comm.hash_into(h);
	}

	فراغ reserve(size_t n) { entries.reserve(n); }
	size_t size() ثابت { החזר entries.size(); }
	bool empty() ثابت { החזר entries.empty(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { החזר iterator(this, صحيح(entries.size())-1); }
	iterator element(صحيح n) { החזר iterator(this, صحيح(entries.size())-1-n); }
	iterator end() { החזר iterator(nullptr, -1); }

	const_iterator begin() ثابت { החזר const_iterator(this, صحيح(entries.size())-1); }
	const_iterator element(صحيح n) ثابت { החזר const_iterator(this, صحيح(entries.size())-1-n); }
	const_iterator end() ثابت { החזר const_iterator(nullptr, -1); }
};

template<typename K, صحيح offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

عام:
	class const_iterator
	{
		friend class idict;
	محمي:
		ثابت idict &container;
		صحيح index;
		const_iterator(ثابت idict &container, صحيح index) : container(container), index(index) { }
	عام:
		تعريفنوع std::forward_iterator_tag iterator_category;
		تعريفنوع K value_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* pointer;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; החזר *this; }
		bool operator==(ثابت const_iterator &other) ثابت { החזר index == other.index; }
		bool operator!=(ثابت const_iterator &other) ثابت { החזר index != other.index; }
		ثابت K &operator*() ثابت { החזר container[index]; }
		ثابت K *operator->() ثابت { החזר &container[index]; }
	};

	constexpr idict()
	{
	}

	صحيح operator()(ثابت K &key)
	{
		Hasher::hash_t hash = database.do_hash(key);
		صحيح i = database.do_lookup(key, hash);
		אם (i < 0)
			i = database.do_insert(key, hash);
		החזר i + offset;
	}

	صحيح at(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = database.do_hash(key);
		صحيح i = database.do_lookup_no_rehash(key, hash);
		אם (i < 0)
			throw std::out_of_range("idict::at()");
		החזר i + offset;
	}

	صحيح at(ثابت K &key, صحيح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(key);
		صحيح i = database.do_lookup_no_rehash(key, hash);
		אם (i < 0)
			החזר defval;
		החזר i + offset;
	}

	صحيح count(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = database.do_hash(key);
		صحيح i = database.do_lookup_no_rehash(key, hash);
		החזר i < 0 ? 0 : 1;
	}

	صحيح lookup(ثابت K &key) ثابت
	{
		Hasher::hash_t hash = database.do_hash(key);
		החזר database.do_lookup_no_rehash(key, hash);
	}

	فراغ expect(ثابت K &key, صحيح i)
	{
		صحيح j = (*this)(key);
		אם (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ثابت K &operator[](صحيح index) ثابت
	{
		החזר database.entries.at(index - offset).udata;
	}

	فراغ swap(idict &other)
	{
		database.swap(other.database);
	}

	فراغ reserve(size_t n) { database.reserve(n); }
	size_t size() ثابت { החזר database.size(); }
	bool empty() ثابت { החזר database.empty(); }
	فراغ clear() { database.clear(); }

	const_iterator begin() ثابت { החזר const_iterator(*this, offset); }
	const_iterator element(صحيح n) ثابت { החזר const_iterator(*this, n); }
	const_iterator end() ثابت { החזר const_iterator(*this, offset + size()); }
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
	عام:
		explicit AtomicParent(صحيح p) : parent(p) {}
		AtomicParent(ثابت AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ثابت AtomicParent &other) { set(other.get()); החזר *this; }
		صحيح get() ثابت { החזר parent.load(std::memory_order_relaxed); }
		فراغ set(صحيح p) { parent.store(p, std::memory_order_relaxed); }
	خاص:
		std::atomic<صحيح> parent;
	};
	std::vector<AtomicParent> parents;

عام:
	تعريفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحيح operator()(ثابت K &key)
	{
		صحيح i = database(key);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		אם (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		החזר i;
	}

	// Finds an element at given index
	ثابت K &operator[](صحيح index) ثابت
	{
		החזר database[index];
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
	صحيح ifind(صحيح i) ثابت
	{
		صحيح p = i, k = i;

		כלעוד (true) {
			صحيح pp = parents[p].get();
			אם (pp < 0)
				اكسر;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		כלעוד (k != p) {
			صحيح next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		החזר p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	فراغ imerge(صحيح i, صحيح j)
	{
		i = ifind(i);
		j = ifind(j);

		אם (i != j)
			parents[i].set(j);
	}

	فراغ ipromote(صحيح i)
	{
		صحيح k = i;

		כלעוד (k != -1) {
			صحيح next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	صحيح lookup(ثابت K &a)
	{
		החזר ifind((*this)(a));
	}

	ثابت K &find(ثابت K &a) ثابت
	{
		صحيح i = database.at(a, -1);
		אם (i < 0)
			החזר a;
		החזר (*this)[ifind(i)];
	}

	فراغ merge(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	فراغ promote(ثابت K &a)
	{
		صحيح i = database.at(a, -1);
		אם (i >= 0)
			ipromote(i);
	}

	فراغ swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	فراغ reserve(size_t n) { database.reserve(n); }
	size_t size() ثابت { החזר database.size(); }
	bool empty() ثابت { החזר database.empty(); }
	فراغ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { החזר database.begin(); }
	const_iterator element(صحيح n) ثابت { החזר database.element(n); }
	const_iterator end() ثابت { החזר database.end(); }
};

} /* namespace hashlib */

#endif
