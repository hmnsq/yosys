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

#અન્તર્ભાવ <array>
#અન્તર્ભાવ <atomic>
#અન્તર્ભાવ <stdexcept>
#અન્તર્ભાવ <algorithm>
#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <string>
#અન્તર્ભાવ <variant>
#અન્તર્ભાવ <vector>
#અન્તર્ભાવ <type_traits>
#અન્તર્ભાવ <stdint.h>

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

#જો defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#નહીંતર
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

અચલ પૂર્ણાંક hashtable_size_trigger = 2;
અચલ પૂર્ણાંક hashtable_size_factor = 3;

namespace legacy {
	અંતઃરેખ uint32_t djb2_add(uint32_t a, uint32_t b) {
		પાછા ((a << 5) + a) + b;
	}
};

template<typename T>
સંરચના hash_ops;

અંતઃરેખ unsigned પૂર્ણાંક mkhash_xorshift(unsigned પૂર્ણાંક a) {
	જો (કદ(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} નહીંતર જો (કદ(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} નહીંતર
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	પાછા a;
}

class HasherDJB32 {
જાહેર:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

ખાનગી:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	સ્થિર uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		પાછા hash;
	}
જાહેર:
	શૂન્ય hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		પાછા;
	}
	શૂન્ય hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		પાછા;
	}
	[[nodiscard]]
	hash_t yield() અચલ {
		પાછા (hash_t)state;
	}

	template<typename T>
	શૂન્ય eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	શૂન્ય eat(અચલ T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	શૂન્ય commutative_eat(hash_t t) {
		state ^= t;
	}

	શૂન્ય force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_ટોચ_LOOP_FST [[nodiscard]] સ્થિર અંતઃરેખ Hasher hash
#define HASH_ટોચ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	પાછા h; \
}

template<typename T>
સંરચના hash_ops {
	સ્થિર અંતઃરેખ bool cmp(અચલ T &a, અચલ T &b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ T &a, Hasher h) {
		જો constexpr (std::is_integral_v<T>) {
			static_assert(કદ(T) <= કદ(uint64_t));
			જો (કદ(T) == કદ(uint64_t))
				h.hash64(a);
			નહીંતર
				h.hash32(a);
			પાછા h;
		} નહીંતર જો constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			પાછા hash_ops<u_type>::hash_into((u_type) a, h);
		} નહીંતર જો constexpr (std::is_pointer_v<T>) {
			પાછા hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} નહીંતર જો constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			પૂર્ણાંક size = a.size();
			પૂર્ણાંક i = 0;
			જ્યારે (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			પાછા h;
		} નહીંતર {
			પાછા a.hash_into(h);
		}
	}
	HASH_ટોચ_LOOP_FST (અચલ T &a) HASH_ટોચ_LOOP_SND
};

template<typename P, typename Q> સંરચના hash_ops<std::pair<P, Q>> {
	સ્થિર અંતઃરેખ bool cmp(અચલ std::pair<P, Q> &a, અચલ std::pair<P, Q> &b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		પાછા h;
	}
	HASH_ટોચ_LOOP_FST (અચલ std::pair<P, Q> &a) HASH_ટોચ_LOOP_SND
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash(અચલ P &p, અચલ Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		પાછા h;
	}
};

template<typename... T> સંરચના hash_ops<std::tuple<T...>> {
	સ્થિર અંતઃરેખ bool cmp(અચલ std::tuple<T...> &a, અચલ std::tuple<T...> &b) {
		પાછા a == b;
	}
	template<size_t I = 0>
	સ્થિર અંતઃરેખ typename std::enable_if<I == કદ...(T), Hasher>::type hash_into(અચલ std::tuple<T...> &, Hasher h) {
		પાછા h;
	}
	template<size_t I = 0>
	સ્થિર અંતઃરેખ typename std::enable_if<I != કદ...(T), Hasher>::type hash_into(અચલ std::tuple<T...> &a, Hasher h) {
		પ્રકારવ્યાખ્યા hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		પાછા h;
	}
	HASH_ટોચ_LOOP_FST (અચલ std::tuple<T...> &a) HASH_ટોચ_LOOP_SND
};

template<typename T> સંરચના hash_ops<std::vector<T>> {
	સ્થિર અંતઃરેખ bool cmp(અચલ std::vector<T> &a, અચલ std::vector<T> &b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		માટે (auto k : a)
			h.eat(k);
		પાછા h;
	}
	HASH_ટોચ_LOOP_FST (અચલ std::vector<T> &a) HASH_ટોચ_LOOP_SND
};

template<typename T, size_t N> સંરચના hash_ops<std::array<T, N>> {
    સ્થિર અંતઃરેખ bool cmp(અચલ std::array<T, N> &a, અચલ std::array<T, N> &b) {
        પાછા a == b;
    }
    [[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ std::array<T, N> &a, Hasher h) {
        માટે (અચલ auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        પાછા h;
    }
	HASH_ટોચ_LOOP_FST (અચલ std::array<T, N> &a) HASH_ટોચ_LOOP_SND
};

સંરચના hash_cstr_ops {
	સ્થિર અંતઃરેખ bool cmp(અચલ અક્ષર *a, અચલ અક્ષર *b) {
		પાછા strcmp(a, b) == 0;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ અક્ષર *a, Hasher h) {
		જ્યારે (*a)
			h.hash32(*(a++));
		પાછા h;
	}
	HASH_ટોચ_LOOP_FST (અચલ અક્ષર *a) HASH_ટોચ_LOOP_SND
};

template <> સંરચના hash_ops<અક્ષર*> : hash_cstr_ops {};

સંરચના hash_ptr_ops {
	સ્થિર અંતઃરેખ bool cmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ શૂન્ય *a, Hasher h) {
		પાછા hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_ટોચ_LOOP_FST (અચલ શૂન્ય *a) HASH_ટોચ_LOOP_SND
};

સંરચના hash_obj_ops {
	સ્થિર અંતઃરેખ bool cmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b) {
		પાછા a == b;
	}
	template<typename T>
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ T *a, Hasher h) {
		જો (a)
			h = a->hash_into(h);
		નહીંતર
			h.eat(0);
		પાછા h;
	}
	template<typename T>
	HASH_ટોચ_LOOP_FST (અચલ T *a) HASH_ટોચ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(અચલ T& obj) {
	પાછા hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
અંતઃરેખ unsigned પૂર્ણાંક mkhash(અચલ T &v) {
	પાછા (unsigned પૂર્ણાંક) run_hash<T>(v);
}

template<> સંરચના hash_ops<std::monostate> {
	સ્થિર અંતઃરેખ bool cmp(std::monostate a, std::monostate b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(std::monostate, Hasher h) {
		પાછા h;
	}
};

template<typename... T> સંરચના hash_ops<std::variant<T...>> {
	સ્થિર અંતઃરેખ bool cmp(અચલ std::variant<T...> &a, અચલ std::variant<T...> &b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ std::variant<T...> &a, Hasher h) {
		std::visit([& h](અચલ auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		પાછા h;
	}
};

template<typename T> સંરચના hash_ops<std::optional<T>> {
	સ્થિર અંતઃરેખ bool cmp(અચલ std::optional<T> &a, અચલ std::optional<T> &b) {
		પાછા a == b;
	}
	[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ std::optional<T> &a, Hasher h) {
		જો(a.has_કિંમત())
			h.eat(*a);
		નહીંતર
			h.eat(0);
		પાછા h;
	}
};

અંતઃરેખ unsigned પૂર્ણાંક hashtable_size(unsigned પૂર્ણાંક min_size)
{
	// Primes as generated by https://oeis.org/A175953
	સ્થિર std::vector<unsigned પૂર્ણાંક> zero_and_some_primes = {
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

	માટે (auto p : zero_and_some_primes)
		જો (p >= min_size) પાછા p;

	જો (કદ(unsigned પૂર્ણાંક) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	માટે (auto p : zero_and_some_primes)
		જો (100129 * p > min_size) પાછા 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, પૂર્ણાંક offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
જાહેર:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	શૂન્ય eat(અચલ T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	શૂન્ય eat(અચલ Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		માટે (auto b : buckets)
			h.eat(b);
		પાછા h;
	}
ખાનગી:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	સંરચના entry_t
	{
		std::pair<K, T> udata;
		પૂર્ણાંક next;

		entry_t() { }
		entry_t(અચલ std::pair<K, T> &udata, પૂર્ણાંક next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, પૂર્ણાંક next) : udata(std::move(udata)), next(next) { }
		bool operator<(અચલ entry_t &other) અચલ { પાછા udata.first < other.udata.first; }
	};

	std::vector<પૂર્ણાંક> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	સ્થિર અંતઃરેખ શૂન્ય do_assert(bool) { }
#નહીંતર
	સ્થિર અંતઃરેખ શૂન્ય do_assert(bool cond) {
		જો (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = 0;
		જો (!hashtable.empty())
			hash = ops.hash(ચાવી).yield() % (unsigned પૂર્ણાંક)(hashtable.size());
		પાછા hash;
	}

	શૂન્ય do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		માટે (પૂર્ણાંક i = 0; i < પૂર્ણાંક(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < પૂર્ણાંક(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	પૂર્ણાંક do_erase(પૂર્ણાંક index, Hasher::hash_t hash)
	{
		do_assert(index < પૂર્ણાંક(entries.size()));
		જો (hashtable.empty() || index < 0)
			પાછા 0;

		પૂર્ણાંક k = hashtable[hash];
		do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));

		જો (k == index) {
			hashtable[hash] = entries[index].next;
		} નહીંતર {
			જ્યારે (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		પૂર્ણાંક back_idx = entries.size()-1;

		જો (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));

			જો (k == back_idx) {
				hashtable[back_hash] = index;
			} નહીંતર {
				જ્યારે (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.કાઢો_back();

		જો (entries.empty())
			hashtable.clear();

		પાછા 1;
	}

	પૂર્ણાંક do_lookup(અચલ K &ચાવી, Hasher::hash_t &hash)
	{
		જો (hashtable.empty())
			પાછા -1;

		જો (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ચાવી);
		}

		પાછા do_lookup_internal(ચાવી, hash);
	}

	પૂર્ણાંક do_lookup_internal(અચલ K &ચાવી, Hasher::hash_t hash) અચલ
	{
		પૂર્ણાંક index = hashtable[hash];

		જ્યારે (index >= 0 && !ops.cmp(entries[index].udata.first, ચાવી)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < પૂર્ણાંક(entries.size()));
		}

		પાછા index;
	}

	પૂર્ણાંક do_lookup_no_rehash(અચલ K &ચાવી, Hasher::hash_t hash) અચલ
	{
		જો (hashtable.empty())
			પાછા -1;

		પાછા do_lookup_internal(ચાવી, hash);
	}

	પૂર્ણાંક do_ઉમેરો(અચલ K &ચાવી, અચલ Hasher::hash_t &hash)
	{
		જો (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(ચાવી, T()), -1);
			do_rehash();
		} નહીંતર {
			entries.emplace_back(std::pair<K, T>(ચાવી, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		પાછા entries.size() - 1;
	}

	પૂર્ણાંક do_ઉમેરો(અચલ std::pair<K, T> &કિંમત, અચલ Hasher::hash_t &hash)
	{
		જો (hashtable.empty()) {
			entries.emplace_back(કિંમત, -1);
			do_rehash();
		} નહીંતર {
			entries.emplace_back(કિંમત, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		પાછા entries.size() - 1;
	}

	પૂર્ણાંક do_ઉમેરો(std::pair<K, T> &&rvalue, અચલ Hasher::hash_t &hash)
	{
		જો (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} નહીંતર {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		પાછા entries.size() - 1;
	}

જાહેર:
	class const_iterator
	{
		friend class dict;
	સુરક્ષિત:
		અચલ dict *ptr;
		પૂર્ણાંક index;
		const_iterator(અચલ dict *ptr, પૂર્ણાંક index) : ptr(ptr), index(index) { }
	જાહેર:
		પ્રકારવ્યાખ્યા std::bidirectional_iterator_tag iterator_category;
		પ્રકારવ્યાખ્યા std::pair<K, T> કિંમત_type;
		પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
		પ્રકારવ્યાખ્યા અચલ std::pair<K, T>* pointer;
		પ્રકારવ્યાખ્યા અચલ std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; પાછા *this; }
		const_iterator operator++(પૂર્ણાંક) { const_iterator tmp = *this; index--; પાછા tmp; }
		const_iterator operator--() { index++; પાછા *this; }
		const_iterator operator--(પૂર્ણાંક) { const_iterator tmp = *this; index++; પાછા tmp; }
		const_iterator operator+=(પૂર્ણાંક amt) { index -= amt; પાછા *this; }
		bool operator<(અચલ const_iterator &other) અચલ { પાછા index > other.index; }
		bool operator==(અચલ const_iterator &other) અચલ { પાછા index == other.index; }
		bool operator!=(અચલ const_iterator &other) અચલ { પાછા index != other.index; }
		અચલ std::pair<K, T> &operator*() અચલ { પાછા ptr->entries[index].udata; }
		અચલ std::pair<K, T> *operator->() અચલ { પાછા &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	સુરક્ષિત:
		dict *ptr;
		પૂર્ણાંક index;
		iterator(dict *ptr, પૂર્ણાંક index) : ptr(ptr), index(index) { }
	જાહેર:
		પ્રકારવ્યાખ્યા std::forward_iterator_tag iterator_category;
		પ્રકારવ્યાખ્યા std::pair<K, T> કિંમત_type;
		પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
		પ્રકારવ્યાખ્યા std::pair<K, T>* pointer;
		પ્રકારવ્યાખ્યા std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; પાછા *this; }
		iterator operator+=(પૂર્ણાંક amt) { index -= amt; પાછા *this; }
		bool operator<(અચલ iterator &other) અચલ { પાછા index > other.index; }
		bool operator==(અચલ iterator &other) અચલ { પાછા index == other.index; }
		bool operator!=(અચલ iterator &other) અચલ { પાછા index != other.index; }
		std::pair<K, T> &operator*() { પાછા ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { પાછા &ptr->entries[index].udata; }
		અચલ std::pair<K, T> &operator*() અચલ { પાછા ptr->entries[index].udata; }
		અચલ std::pair<K, T> *operator->() અચલ { પાછા &ptr->entries[index].udata; }
		operator const_iterator() અચલ { પાછા const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() અચલ {
		પાછા std::make_reverse_iterator(end());
	}
	reverse_iterator rend() અચલ {
		પાછા std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(અચલ dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(અચલ dict &other) {
		entries = other.entries;
		do_rehash();
		પાછા *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		પાછા *this;
	}

	dict(અચલ std::initializer_list<std::pair<K, T>> &list)
	{
		માટે (auto &it : list)
			ઉમેરો(it);
	}

	template<class આગત_Iterator>
	dict(આગત_Iterator first, આગત_Iterator last)
	{
		ઉમેરો(first, last);
	}

	template<class આગત_Iterator>
	શૂન્ય ઉમેરો(આગત_Iterator first, આગત_Iterator last)
	{
		માટે (; first != last; ++first)
			ઉમેરો(*first);
	}

	std::pair<iterator, bool> ઉમેરો(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(ચાવી, hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ઉમેરો(અચલ std::pair<K, T> &કિંમત)
	{
		Hasher::hash_t hash = do_hash(કિંમત.first);
		પૂર્ણાંક i = do_lookup(કિંમત.first, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(કિંમત, hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ઉમેરો(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		પૂર્ણાંક i = do_lookup(rvalue.first, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::forward<std::pair<K, T>>(rvalue), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K અચલ &ચાવી, T અચલ &કિંમત)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::make_pair(ચાવી, કિંમત), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K અચલ &ચાવી, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::make_pair(ચાવી, std::forward<T>(rvalue)), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T અચલ &કિંમત)
	{
		Hasher::hash_t hash = do_hash(rkey);
		પૂર્ણાંક i = do_lookup(rkey, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::make_pair(std::forward<K>(rkey), કિંમત), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		પૂર્ણાંક i = do_lookup(rkey, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	પૂર્ણાંક erase(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક index = do_lookup(ચાવી, hash);
		પાછા do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		પાછા ++it;
	}

	પૂર્ણાંક ગણતરી(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		પાછા i < 0 ? 0 : 1;
	}

	પૂર્ણાંક ગણતરી(અચલ K &ચાવી, const_iterator it) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		પાછા i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i < 0)
			પાછા end();
		પાછા iterator(this, i);
	}

	const_iterator find(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			પાછા end();
		પાછા const_iterator(this, i);
	}

	T& at(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i < 0)
			throw std::out_of_range("dict::at()");
		પાછા entries[i].udata.second;
	}

	અચલ T& at(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			throw std::out_of_range("dict::at()");
		પાછા entries[i].udata.second;
	}

	અચલ T& at(અચલ K &ચાવી, અચલ T &defval) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			પાછા defval;
		પાછા entries[i].udata.second;
	}

	T& operator[](અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i < 0)
			i = do_ઉમેરો(std::pair<K, T>(ચાવી, T()), hash);
		પાછા entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	શૂન્ય sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](અચલ entry_t &a, અચલ entry_t &b){ પાછા comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	શૂન્ય swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(અચલ dict &other) અચલ {
		જો (size() != other.size())
			પાછા false;
		માટે (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			જો (oit == other.end() || !(oit->second == it.udata.second))
				પાછા false;
		}
		પાછા true;
	}

	bool operator!=(અચલ dict &other) અચલ {
		પાછા !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		commutative_hash comm;
		માટે (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		પાછા comm.hash_into(h);
	}

	શૂન્ય reserve(size_t n) { entries.reserve(n); }
	size_t size() અચલ { પાછા entries.size(); }
	bool empty() અચલ { પાછા entries.empty(); }
	શૂન્ય clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { પાછા iterator(this, પૂર્ણાંક(entries.size())-1); }
	iterator element(પૂર્ણાંક n) { પાછા iterator(this, પૂર્ણાંક(entries.size())-1-n); }
	iterator end() { પાછા iterator(nullptr, -1); }

	const_iterator begin() અચલ { પાછા const_iterator(this, પૂર્ણાંક(entries.size())-1); }
	const_iterator element(પૂર્ણાંક n) અચલ { પાછા const_iterator(this, પૂર્ણાંક(entries.size())-1-n); }
	const_iterator end() અચલ { પાછા const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, પૂર્ણાંક, typename> friend class idict;

સુરક્ષિત:
	સંરચના entry_t
	{
		K udata;
		પૂર્ણાંક next;

		entry_t() { }
		entry_t(અચલ K &udata, પૂર્ણાંક next) : udata(udata), next(next) { }
		entry_t(K &&udata, પૂર્ણાંક next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<પૂર્ણાંક> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	સ્થિર અંતઃરેખ શૂન્ય do_assert(bool) { }
#નહીંતર
	સ્થિર અંતઃરેખ શૂન્ય do_assert(bool cond) {
		જો (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = 0;
		જો (!hashtable.empty())
			hash = ops.hash(ચાવી).yield() % (unsigned પૂર્ણાંક)(hashtable.size());
		પાછા hash;
	}

	શૂન્ય do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		માટે (પૂર્ણાંક i = 0; i < પૂર્ણાંક(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < પૂર્ણાંક(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	પૂર્ણાંક do_erase(પૂર્ણાંક index, Hasher::hash_t hash)
	{
		do_assert(index < પૂર્ણાંક(entries.size()));
		જો (hashtable.empty() || index < 0)
			પાછા 0;

		પૂર્ણાંક k = hashtable[hash];
		જો (k == index) {
			hashtable[hash] = entries[index].next;
		} નહીંતર {
			જ્યારે (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		પૂર્ણાંક back_idx = entries.size()-1;

		જો (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			જો (k == back_idx) {
				hashtable[back_hash] = index;
			} નહીંતર {
				જ્યારે (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < પૂર્ણાંક(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.કાઢો_back();

		જો (entries.empty())
			hashtable.clear();

		પાછા 1;
	}

	પૂર્ણાંક do_lookup(અચલ K &ચાવી, Hasher::hash_t &hash)
	{
		જો (hashtable.empty())
			પાછા -1;

		જો (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(ચાવી);
		}

		પાછા do_lookup_internal(ચાવી, hash);
	}

	પૂર્ણાંક do_lookup_internal(અચલ K &ચાવી, Hasher::hash_t hash) અચલ
	{
		પૂર્ણાંક index = hashtable[hash];

		જ્યારે (index >= 0 && !ops.cmp(entries[index].udata, ચાવી)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < પૂર્ણાંક(entries.size()));
		}

		પાછા index;
	}

	પૂર્ણાંક do_lookup_no_rehash(અચલ K &ચાવી, Hasher::hash_t hash) અચલ
	{
		જો (hashtable.empty())
			પાછા -1;

		પાછા do_lookup_internal(ચાવી, hash);
	}

	પૂર્ણાંક do_ઉમેરો(અચલ K &કિંમત, Hasher::hash_t &hash)
	{
		જો (hashtable.empty()) {
			entries.emplace_back(કિંમત, -1);
			do_rehash();
			hash = do_hash(કિંમત);
		} નહીંતર {
			entries.emplace_back(કિંમત, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		પાછા entries.size() - 1;
	}

	પૂર્ણાંક do_ઉમેરો(K &&rvalue, Hasher::hash_t &hash)
	{
		જો (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} નહીંતર {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		પાછા entries.size() - 1;
	}

જાહેર:
	class const_iterator
	{
		friend class pool;
	સુરક્ષિત:
		અચલ pool *ptr;
		પૂર્ણાંક index;
		const_iterator(અચલ pool *ptr, પૂર્ણાંક index) : ptr(ptr), index(index) { }
	જાહેર:
		પ્રકારવ્યાખ્યા std::forward_iterator_tag iterator_category;
		પ્રકારવ્યાખ્યા K કિંમત_type;
		પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
		પ્રકારવ્યાખ્યા K* pointer;
		પ્રકારવ્યાખ્યા K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; પાછા *this; }
		bool operator==(અચલ const_iterator &other) અચલ { પાછા index == other.index; }
		bool operator!=(અચલ const_iterator &other) અચલ { પાછા index != other.index; }
		અચલ K &operator*() અચલ { પાછા ptr->entries[index].udata; }
		અચલ K *operator->() અચલ { પાછા &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	સુરક્ષિત:
		pool *ptr;
		પૂર્ણાંક index;
		iterator(pool *ptr, પૂર્ણાંક index) : ptr(ptr), index(index) { }
	જાહેર:
		પ્રકારવ્યાખ્યા std::forward_iterator_tag iterator_category;
		પ્રકારવ્યાખ્યા K કિંમત_type;
		પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
		પ્રકારવ્યાખ્યા K* pointer;
		પ્રકારવ્યાખ્યા K& reference;
		iterator() { }
		iterator operator++() { index--; પાછા *this; }
		bool operator==(અચલ iterator &other) અચલ { પાછા index == other.index; }
		bool operator!=(અચલ iterator &other) અચલ { પાછા index != other.index; }
		K &operator*() { પાછા ptr->entries[index].udata; }
		K *operator->() { પાછા &ptr->entries[index].udata; }
		અચલ K &operator*() અચલ { પાછા ptr->entries[index].udata; }
		અચલ K *operator->() અચલ { પાછા &ptr->entries[index].udata; }
		operator const_iterator() અચલ { પાછા const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(અચલ pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(અચલ pool &other) {
		entries = other.entries;
		do_rehash();
		પાછા *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		પાછા *this;
	}

	pool(અચલ std::initializer_list<K> &list)
	{
		માટે (auto &it : list)
			ઉમેરો(it);
	}

	template<class આગત_Iterator>
	pool(આગત_Iterator first, આગત_Iterator last)
	{
		ઉમેરો(first, last);
	}

	template<class આગત_Iterator>
	શૂન્ય ઉમેરો(આગત_Iterator first, આગત_Iterator last)
	{
		માટે (; first != last; ++first)
			ઉમેરો(*first);
	}

	std::pair<iterator, bool> ઉમેરો(અચલ K &કિંમત)
	{
		Hasher::hash_t hash = do_hash(કિંમત);
		પૂર્ણાંક i = do_lookup(કિંમત, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(કિંમત, hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ઉમેરો(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		પૂર્ણાંક i = do_lookup(rvalue, hash);
		જો (i >= 0)
			પાછા std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ઉમેરો(std::forward<K>(rvalue), hash);
		પાછા std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		પાછા ઉમેરો(K(std::forward<Args>(args)...));
	}

	પૂર્ણાંક erase(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક index = do_lookup(ચાવી, hash);
		પાછા do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		પાછા ++it;
	}

	પૂર્ણાંક ગણતરી(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		પાછા i < 0 ? 0 : 1;
	}

	પૂર્ણાંક ગણતરી(અચલ K &ચાવી, const_iterator it) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		પાછા i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		જો (i < 0)
			પાછા end();
		પાછા iterator(this, i);
	}

	const_iterator find(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			પાછા end();
		પાછા const_iterator(this, i);
	}

	bool operator[](અચલ K &ચાવી)
	{
		Hasher::hash_t hash = do_hash(ચાવી);
		પૂર્ણાંક i = do_lookup(ચાવી, hash);
		પાછા i >= 0;
	}

	template<typename Compare = std::less<K>>
	શૂન્ય sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](અચલ entry_t &a, અચલ entry_t &b){ પાછા comp(b.udata, a.udata); });
		do_rehash();
	}

	K કાઢો()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		પાછા ret;
	}

	શૂન્ય swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(અચલ pool &other) અચલ {
		જો (size() != other.size())
			પાછા false;
		માટે (auto &it : entries)
			જો (!other.ગણતરી(it.udata))
				પાછા false;
		પાછા true;
	}

	bool operator!=(અચલ pool &other) અચલ {
		પાછા !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		commutative_hash comm;
		માટે (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		પાછા comm.hash_into(h);
	}

	શૂન્ય reserve(size_t n) { entries.reserve(n); }
	size_t size() અચલ { પાછા entries.size(); }
	bool empty() અચલ { પાછા entries.empty(); }
	શૂન્ય clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { પાછા iterator(this, પૂર્ણાંક(entries.size())-1); }
	iterator element(પૂર્ણાંક n) { પાછા iterator(this, પૂર્ણાંક(entries.size())-1-n); }
	iterator end() { પાછા iterator(nullptr, -1); }

	const_iterator begin() અચલ { પાછા const_iterator(this, પૂર્ણાંક(entries.size())-1); }
	const_iterator element(પૂર્ણાંક n) અચલ { પાછા const_iterator(this, પૂર્ણાંક(entries.size())-1-n); }
	const_iterator end() અચલ { પાછા const_iterator(nullptr, -1); }
};

template<typename K, પૂર્ણાંક offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

જાહેર:
	class const_iterator
	{
		friend class idict;
	સુરક્ષિત:
		અચલ idict &container;
		પૂર્ણાંક index;
		const_iterator(અચલ idict &container, પૂર્ણાંક index) : container(container), index(index) { }
	જાહેર:
		પ્રકારવ્યાખ્યા std::forward_iterator_tag iterator_category;
		પ્રકારવ્યાખ્યા K કિંમત_type;
		પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
		પ્રકારવ્યાખ્યા K* pointer;
		પ્રકારવ્યાખ્યા K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; પાછા *this; }
		bool operator==(અચલ const_iterator &other) અચલ { પાછા index == other.index; }
		bool operator!=(અચલ const_iterator &other) અચલ { પાછા index != other.index; }
		અચલ K &operator*() અચલ { પાછા container[index]; }
		અચલ K *operator->() અચલ { પાછા &container[index]; }
	};

	constexpr idict()
	{
	}

	પૂર્ણાંક operator()(અચલ K &ચાવી)
	{
		Hasher::hash_t hash = database.do_hash(ચાવી);
		પૂર્ણાંક i = database.do_lookup(ચાવી, hash);
		જો (i < 0)
			i = database.do_ઉમેરો(ચાવી, hash);
		પાછા i + offset;
	}

	પૂર્ણાંક at(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = database.do_hash(ચાવી);
		પૂર્ણાંક i = database.do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			throw std::out_of_range("idict::at()");
		પાછા i + offset;
	}

	પૂર્ણાંક at(અચલ K &ચાવી, પૂર્ણાંક defval) અચલ
	{
		Hasher::hash_t hash = database.do_hash(ચાવી);
		પૂર્ણાંક i = database.do_lookup_no_rehash(ચાવી, hash);
		જો (i < 0)
			પાછા defval;
		પાછા i + offset;
	}

	પૂર્ણાંક ગણતરી(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = database.do_hash(ચાવી);
		પૂર્ણાંક i = database.do_lookup_no_rehash(ચાવી, hash);
		પાછા i < 0 ? 0 : 1;
	}

	પૂર્ણાંક lookup(અચલ K &ચાવી) અચલ
	{
		Hasher::hash_t hash = database.do_hash(ચાવી);
		પાછા database.do_lookup_no_rehash(ચાવી, hash);
	}

	શૂન્ય expect(અચલ K &ચાવી, પૂર્ણાંક i)
	{
		પૂર્ણાંક j = (*this)(ચાવી);
		જો (i != j)
			throw std::out_of_range("idict::expect()");
	}

	અચલ K &operator[](પૂર્ણાંક index) અચલ
	{
		પાછા database.entries.at(index - offset).udata;
	}

	શૂન્ય swap(idict &other)
	{
		database.swap(other.database);
	}

	શૂન્ય reserve(size_t n) { database.reserve(n); }
	size_t size() અચલ { પાછા database.size(); }
	bool empty() અચલ { પાછા database.empty(); }
	શૂન્ય clear() { database.clear(); }

	const_iterator begin() અચલ { પાછા const_iterator(*this, offset); }
	const_iterator element(પૂર્ણાંક n) અચલ { પાછા const_iterator(*this, n); }
	const_iterator end() અચલ { પાછા const_iterator(*this, offset + size()); }
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
	જાહેર:
		explicit AtomicParent(પૂર્ણાંક p) : parent(p) {}
		AtomicParent(અચલ AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(અચલ AtomicParent &other) { set(other.get()); પાછા *this; }
		પૂર્ણાંક get() અચલ { પાછા parent.load(std::memory_order_relaxed); }
		શૂન્ય set(પૂર્ણાંક p) { parent.store(p, std::memory_order_relaxed); }
	ખાનગી:
		std::atomic<પૂર્ણાંક> parent;
	};
	std::vector<AtomicParent> parents;

જાહેર:
	પ્રકારવ્યાખ્યા typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	પૂર્ણાંક operator()(અચલ K &ચાવી)
	{
		પૂર્ણાંક i = database(ચાવી);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		જો (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		પાછા i;
	}

	// Finds an element at given index
	અચલ K &operator[](પૂર્ણાંક index) અચલ
	{
		પાછા database[index];
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
	પૂર્ણાંક ifind(પૂર્ણાંક i) અચલ
	{
		પૂર્ણાંક p = i, k = i;

		જ્યારે (true) {
			પૂર્ણાંક pp = parents[p].get();
			જો (pp < 0)
				તોડો;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		જ્યારે (k != p) {
			પૂર્ણાંક next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		પાછા p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	શૂન્ય imerge(પૂર્ણાંક i, પૂર્ણાંક j)
	{
		i = ifind(i);
		j = ifind(j);

		જો (i != j)
			parents[i].set(j);
	}

	શૂન્ય ipromote(પૂર્ણાંક i)
	{
		પૂર્ણાંક k = i;

		જ્યારે (k != -1) {
			પૂર્ણાંક next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	પૂર્ણાંક lookup(અચલ K &a)
	{
		પાછા ifind((*this)(a));
	}

	અચલ K &find(અચલ K &a) અચલ
	{
		પૂર્ણાંક i = database.at(a, -1);
		જો (i < 0)
			પાછા a;
		પાછા (*this)[ifind(i)];
	}

	શૂન્ય merge(અચલ K &a, અચલ K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	શૂન્ય promote(અચલ K &a)
	{
		પૂર્ણાંક i = database.at(a, -1);
		જો (i >= 0)
			ipromote(i);
	}

	શૂન્ય swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	શૂન્ય reserve(size_t n) { database.reserve(n); }
	size_t size() અચલ { પાછા database.size(); }
	bool empty() અચલ { પાછા database.empty(); }
	શૂન્ય clear() { database.clear(); parents.clear(); }

	const_iterator begin() અચલ { પાછા database.begin(); }
	const_iterator element(પૂર્ણાંક n) અચલ { પાછા database.element(n); }
	const_iterator end() અચલ { પાછા database.end(); }
};

} /* namespace hashlib */

#endif
