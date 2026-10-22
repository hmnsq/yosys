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

#अन्तर्भाव <array>
#अन्तर्भाव <atomic>
#अन्तर्भाव <stdexcept>
#अन्तर्भाव <algorithm>
#अन्तर्भाव <optional>
#अन्तर्भाव <string>
#अन्तर्भाव <variant>
#अन्तर्भाव <vector>
#अन्तर्भाव <type_traits>
#अन्तर्भाव <stdint.h>

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

#यदि defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#अन्यथा
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ध्रुव पूर्णाङ्क hashtable_size_trigger = 2;
ध्रुव पूर्णाङ्क hashtable_size_factor = 3;

namespace legacy {
	अन्तःस्थ uint32_t djb2_add(uint32_t a, uint32_t b) {
		प्रत्यागम ((a << 5) + a) + b;
	}
};

template<typename T>
संरचना hash_ops;

अन्तःस्थ unsigned पूर्णाङ्क mkhash_xorshift(unsigned पूर्णाङ्क a) {
	यदि (परिमाण(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} अन्यथा यदि (परिमाण(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} अन्यथा
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	प्रत्यागम a;
}

class HasherDJB32 {
सार्वजनिक:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

गुप्त:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	स्थिर uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		प्रत्यागम hash;
	}
सार्वजनिक:
	शून्य hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		प्रत्यागम;
	}
	शून्य hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		प्रत्यागम;
	}
	[[nodiscard]]
	hash_t yield() ध्रुव {
		प्रत्यागम (hash_t)state;
	}

	template<typename T>
	शून्य eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	शून्य eat(ध्रुव T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	शून्य commutative_eat(hash_t t) {
		state ^= t;
	}

	शून्य force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_शीर्ष_LOOP_FST [[nodiscard]] स्थिर अन्तःस्थ Hasher hash
#define HASH_शीर्ष_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	प्रत्यागम h; \
}

template<typename T>
संरचना hash_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव T &a, ध्रुव T &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T &a, Hasher h) {
		यदि constexpr (std::is_integral_v<T>) {
			static_assert(परिमाण(T) <= परिमाण(uint64_t));
			यदि (परिमाण(T) == परिमाण(uint64_t))
				h.hash64(a);
			अन्यथा
				h.hash32(a);
			प्रत्यागम h;
		} अन्यथा यदि constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			प्रत्यागम hash_ops<u_type>::hash_into((u_type) a, h);
		} अन्यथा यदि constexpr (std::is_pointer_v<T>) {
			प्रत्यागम hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} अन्यथा यदि constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			पूर्णाङ्क size = a.size();
			पूर्णाङ्क i = 0;
			यावत् (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			प्रत्यागम h;
		} अन्यथा {
			प्रत्यागम a.hash_into(h);
		}
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव T &a) HASH_शीर्ष_LOOP_SND
};

template<typename P, typename Q> संरचना hash_ops<std::pair<P, Q>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::pair<P, Q> &a, ध्रुव std::pair<P, Q> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		प्रत्यागम h;
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव std::pair<P, Q> &a) HASH_शीर्ष_LOOP_SND
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव P &p, ध्रुव Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		प्रत्यागम h;
	}
};

template<typename... T> संरचना hash_ops<std::tuple<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::tuple<T...> &a, ध्रुव std::tuple<T...> &b) {
		प्रत्यागम a == b;
	}
	template<size_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I == परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &, Hasher h) {
		प्रत्यागम h;
	}
	template<size_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I != परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &a, Hasher h) {
		प्रकारनिर्देश hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		प्रत्यागम h;
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव std::tuple<T...> &a) HASH_शीर्ष_LOOP_SND
};

template<typename T> संरचना hash_ops<std::vector<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::vector<T> &a, ध्रुव std::vector<T> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		प्रति (auto k : a)
			h.eat(k);
		प्रत्यागम h;
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव std::vector<T> &a) HASH_शीर्ष_LOOP_SND
};

template<typename T, size_t N> संरचना hash_ops<std::array<T, N>> {
    स्थिर अन्तःस्थ bool cmp(ध्रुव std::array<T, N> &a, ध्रुव std::array<T, N> &b) {
        प्रत्यागम a == b;
    }
    [[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::array<T, N> &a, Hasher h) {
        प्रति (ध्रुव auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        प्रत्यागम h;
    }
	HASH_शीर्ष_LOOP_FST (ध्रुव std::array<T, N> &a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_cstr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
		प्रत्यागम strcmp(a, b) == 0;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव वर्ण *a, Hasher h) {
		यावत् (*a)
			h.hash32(*(a++));
		प्रत्यागम h;
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव वर्ण *a) HASH_शीर्ष_LOOP_SND
};

template <> संरचना hash_ops<वर्ण*> : hash_cstr_ops {};

संरचना hash_ptr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव शून्य *a, Hasher h) {
		प्रत्यागम hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शीर्ष_LOOP_FST (ध्रुव शून्य *a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_obj_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		प्रत्यागम a == b;
	}
	template<typename T>
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T *a, Hasher h) {
		यदि (a)
			h = a->hash_into(h);
		अन्यथा
			h.eat(0);
		प्रत्यागम h;
	}
	template<typename T>
	HASH_शीर्ष_LOOP_FST (ध्रुव T *a) HASH_शीर्ष_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(ध्रुव T& obj) {
	प्रत्यागम hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
अन्तःस्थ unsigned पूर्णाङ्क mkhash(ध्रुव T &v) {
	प्रत्यागम (unsigned पूर्णाङ्क) run_hash<T>(v);
}

template<> संरचना hash_ops<std::monostate> {
	स्थिर अन्तःस्थ bool cmp(std::monostate a, std::monostate b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(std::monostate, Hasher h) {
		प्रत्यागम h;
	}
};

template<typename... T> संरचना hash_ops<std::variant<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::variant<T...> &a, ध्रुव std::variant<T...> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::variant<T...> &a, Hasher h) {
		std::visit([& h](ध्रुव auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		प्रत्यागम h;
	}
};

template<typename T> संरचना hash_ops<std::optional<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::optional<T> &a, ध्रुव std::optional<T> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::optional<T> &a, Hasher h) {
		यदि(a.has_मूल्य())
			h.eat(*a);
		अन्यथा
			h.eat(0);
		प्रत्यागम h;
	}
};

अन्तःस्थ unsigned पूर्णाङ्क hashtable_size(unsigned पूर्णाङ्क min_size)
{
	// Primes as generated by https://oeis.org/A175953
	स्थिर std::vector<unsigned पूर्णाङ्क> zero_and_some_primes = {
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

	प्रति (auto p : zero_and_some_primes)
		यदि (p >= min_size) प्रत्यागम p;

	यदि (परिमाण(unsigned पूर्णाङ्क) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	प्रति (auto p : zero_and_some_primes)
		यदि (100129 * p > min_size) प्रत्यागम 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, पूर्णाङ्क offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
सार्वजनिक:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	शून्य eat(ध्रुव T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	शून्य eat(ध्रुव Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		प्रति (auto b : buckets)
			h.eat(b);
		प्रत्यागम h;
	}
गुप्त:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	संरचना entry_t
	{
		std::pair<K, T> udata;
		पूर्णाङ्क next;

		entry_t() { }
		entry_t(ध्रुव std::pair<K, T> &udata, पूर्णाङ्क next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, पूर्णाङ्क next) : udata(std::move(udata)), next(next) { }
		bool operator<(ध्रुव entry_t &other) ध्रुव { प्रत्यागम udata.first < other.udata.first; }
	};

	std::vector<पूर्णाङ्क> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.empty())
			hash = ops.hash(कुंजी).yield() % (unsigned पूर्णाङ्क)(hashtable.size());
		प्रत्यागम hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णाङ्क(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णाङ्क(entries.size()));
		यदि (hashtable.empty() || index < 0)
			प्रत्यागम 0;

		पूर्णाङ्क k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));

		यदि (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			यावत् (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णाङ्क back_idx = entries.size()-1;

		यदि (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));

			यदि (k == back_idx) {
				hashtable[back_hash] = index;
			} अन्यथा {
				यावत् (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.निकालो_back();

		यदि (entries.empty())
			hashtable.clear();

		प्रत्यागम 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुंजी, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty())
			प्रत्यागम -1;

		यदि (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(कुंजी);
		}

		प्रत्यागम do_lookup_internal(कुंजी, hash);
	}

	पूर्णाङ्क do_lookup_internal(ध्रुव K &कुंजी, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क index = hashtable[hash];

		यावत् (index >= 0 && !ops.cmp(entries[index].udata.first, कुंजी)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		प्रत्यागम index;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुंजी, Hasher::hash_t hash) ध्रुव
	{
		यदि (hashtable.empty())
			प्रत्यागम -1;

		प्रत्यागम do_lookup_internal(कुंजी, hash);
	}

	पूर्णाङ्क do_जोड़ो(ध्रुव K &कुंजी, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(कुंजी, T()), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::pair<K, T>(कुंजी, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		प्रत्यागम entries.size() - 1;
	}

	पूर्णाङ्क do_जोड़ो(ध्रुव std::pair<K, T> &मूल्य, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		प्रत्यागम entries.size() - 1;
	}

	पूर्णाङ्क do_जोड़ो(std::pair<K, T> &&rvalue, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		प्रत्यागम entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		friend class dict;
	रक्षित:
		ध्रुव dict *ptr;
		पूर्णाङ्क index;
		const_iterator(ध्रुव dict *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::bidirectional_iterator_tag iterator_category;
		प्रकारनिर्देश std::pair<K, T> मूल्य_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश ध्रुव std::pair<K, T>* pointer;
		प्रकारनिर्देश ध्रुव std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; प्रत्यागम *this; }
		const_iterator operator++(पूर्णाङ्क) { const_iterator tmp = *this; index--; प्रत्यागम tmp; }
		const_iterator operator--() { index++; प्रत्यागम *this; }
		const_iterator operator--(पूर्णाङ्क) { const_iterator tmp = *this; index++; प्रत्यागम tmp; }
		const_iterator operator+=(पूर्णाङ्क amt) { index -= amt; प्रत्यागम *this; }
		bool operator<(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index > other.index; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index != other.index; }
		ध्रुव std::pair<K, T> &operator*() ध्रुव { प्रत्यागम ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> *operator->() ध्रुव { प्रत्यागम &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	रक्षित:
		dict *ptr;
		पूर्णाङ्क index;
		iterator(dict *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश std::pair<K, T> मूल्य_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश std::pair<K, T>* pointer;
		प्रकारनिर्देश std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; प्रत्यागम *this; }
		iterator operator+=(पूर्णाङ्क amt) { index -= amt; प्रत्यागम *this; }
		bool operator<(ध्रुव iterator &other) ध्रुव { प्रत्यागम index > other.index; }
		bool operator==(ध्रुव iterator &other) ध्रुव { प्रत्यागम index == other.index; }
		bool operator!=(ध्रुव iterator &other) ध्रुव { प्रत्यागम index != other.index; }
		std::pair<K, T> &operator*() { प्रत्यागम ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { प्रत्यागम &ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> &operator*() ध्रुव { प्रत्यागम ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> *operator->() ध्रुव { प्रत्यागम &ptr->entries[index].udata; }
		operator const_iterator() ध्रुव { प्रत्यागम const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ध्रुव {
		प्रत्यागम std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ध्रुव {
		प्रत्यागम std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(ध्रुव dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(ध्रुव dict &other) {
		entries = other.entries;
		do_rehash();
		प्रत्यागम *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		प्रत्यागम *this;
	}

	dict(ध्रुव std::initializer_list<std::pair<K, T>> &list)
	{
		प्रति (auto &it : list)
			जोड़ो(it);
	}

	template<class निवेश_Iterator>
	dict(निवेश_Iterator first, निवेश_Iterator last)
	{
		जोड़ो(first, last);
	}

	template<class निवेश_Iterator>
	शून्य जोड़ो(निवेश_Iterator first, निवेश_Iterator last)
	{
		प्रति (; first != last; ++first)
			जोड़ो(*first);
	}

	std::pair<iterator, bool> जोड़ो(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(कुंजी, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(ध्रुव std::pair<K, T> &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य.first);
		पूर्णाङ्क i = do_lookup(मूल्य.first, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(मूल्य, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णाङ्क i = do_lookup(rvalue.first, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::forward<std::pair<K, T>>(rvalue), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुंजी, T ध्रुव &मूल्य)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(कुंजी, मूल्य), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुंजी, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(कुंजी, std::forward<T>(rvalue)), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ध्रुव &मूल्य)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(std::forward<K>(rkey), मूल्य), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णाङ्क erase(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क index = do_lookup(कुंजी, hash);
		प्रत्यागम do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		प्रत्यागम ++it;
	}

	पूर्णाङ्क गिनती(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गिनती(ध्रुव K &कुंजी, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		प्रत्यागम i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम end();
		प्रत्यागम iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम end();
		प्रत्यागम const_iterator(this, i);
	}

	T& at(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i < 0)
			throw std::out_of_range("dict::at()");
		प्रत्यागम entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			throw std::out_of_range("dict::at()");
		प्रत्यागम entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुंजी, ध्रुव T &defval) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम defval;
		प्रत्यागम entries[i].udata.second;
	}

	T& operator[](ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i < 0)
			i = do_जोड़ो(std::pair<K, T>(कुंजी, T()), hash);
		प्रत्यागम entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ध्रुव entry_t &a, ध्रुव entry_t &b){ प्रत्यागम comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	शून्य swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ध्रुव dict &other) ध्रुव {
		यदि (size() != other.size())
			प्रत्यागम false;
		प्रति (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			यदि (oit == other.end() || !(oit->second == it.udata.second))
				प्रत्यागम false;
		}
		प्रत्यागम true;
	}

	bool operator!=(ध्रुव dict &other) ध्रुव {
		प्रत्यागम !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		प्रति (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		प्रत्यागम comm.hash_into(h);
	}

	शून्य reserve(size_t n) { entries.reserve(n); }
	size_t size() ध्रुव { प्रत्यागम entries.size(); }
	bool empty() ध्रुव { प्रत्यागम entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { प्रत्यागम iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { प्रत्यागम iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { प्रत्यागम iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() ध्रुव { प्रत्यागम const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, पूर्णाङ्क, typename> friend class idict;

रक्षित:
	संरचना entry_t
	{
		K udata;
		पूर्णाङ्क next;

		entry_t() { }
		entry_t(ध्रुव K &udata, पूर्णाङ्क next) : udata(udata), next(next) { }
		entry_t(K &&udata, पूर्णाङ्क next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<पूर्णाङ्क> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.empty())
			hash = ops.hash(कुंजी).yield() % (unsigned पूर्णाङ्क)(hashtable.size());
		प्रत्यागम hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णाङ्क(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णाङ्क(entries.size()));
		यदि (hashtable.empty() || index < 0)
			प्रत्यागम 0;

		पूर्णाङ्क k = hashtable[hash];
		यदि (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			यावत् (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णाङ्क back_idx = entries.size()-1;

		यदि (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			यदि (k == back_idx) {
				hashtable[back_hash] = index;
			} अन्यथा {
				यावत् (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.निकालो_back();

		यदि (entries.empty())
			hashtable.clear();

		प्रत्यागम 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुंजी, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty())
			प्रत्यागम -1;

		यदि (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(कुंजी);
		}

		प्रत्यागम do_lookup_internal(कुंजी, hash);
	}

	पूर्णाङ्क do_lookup_internal(ध्रुव K &कुंजी, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क index = hashtable[hash];

		यावत् (index >= 0 && !ops.cmp(entries[index].udata, कुंजी)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		प्रत्यागम index;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुंजी, Hasher::hash_t hash) ध्रुव
	{
		यदि (hashtable.empty())
			प्रत्यागम -1;

		प्रत्यागम do_lookup_internal(कुंजी, hash);
	}

	पूर्णाङ्क do_जोड़ो(ध्रुव K &मूल्य, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
			hash = do_hash(मूल्य);
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		प्रत्यागम entries.size() - 1;
	}

	पूर्णाङ्क do_जोड़ो(K &&rvalue, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} अन्यथा {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		प्रत्यागम entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		friend class pool;
	रक्षित:
		ध्रुव pool *ptr;
		पूर्णाङ्क index;
		const_iterator(ध्रुव pool *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मूल्य_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; प्रत्यागम *this; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index != other.index; }
		ध्रुव K &operator*() ध्रुव { प्रत्यागम ptr->entries[index].udata; }
		ध्रुव K *operator->() ध्रुव { प्रत्यागम &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	रक्षित:
		pool *ptr;
		पूर्णाङ्क index;
		iterator(pool *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मूल्य_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		iterator() { }
		iterator operator++() { index--; प्रत्यागम *this; }
		bool operator==(ध्रुव iterator &other) ध्रुव { प्रत्यागम index == other.index; }
		bool operator!=(ध्रुव iterator &other) ध्रुव { प्रत्यागम index != other.index; }
		K &operator*() { प्रत्यागम ptr->entries[index].udata; }
		K *operator->() { प्रत्यागम &ptr->entries[index].udata; }
		ध्रुव K &operator*() ध्रुव { प्रत्यागम ptr->entries[index].udata; }
		ध्रुव K *operator->() ध्रुव { प्रत्यागम &ptr->entries[index].udata; }
		operator const_iterator() ध्रुव { प्रत्यागम const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(ध्रुव pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(ध्रुव pool &other) {
		entries = other.entries;
		do_rehash();
		प्रत्यागम *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		प्रत्यागम *this;
	}

	pool(ध्रुव std::initializer_list<K> &list)
	{
		प्रति (auto &it : list)
			जोड़ो(it);
	}

	template<class निवेश_Iterator>
	pool(निवेश_Iterator first, निवेश_Iterator last)
	{
		जोड़ो(first, last);
	}

	template<class निवेश_Iterator>
	शून्य जोड़ो(निवेश_Iterator first, निवेश_Iterator last)
	{
		प्रति (; first != last; ++first)
			जोड़ो(*first);
	}

	std::pair<iterator, bool> जोड़ो(ध्रुव K &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य);
		पूर्णाङ्क i = do_lookup(मूल्य, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(मूल्य, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णाङ्क i = do_lookup(rvalue, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::forward<K>(rvalue), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		प्रत्यागम जोड़ो(K(std::forward<Args>(args)...));
	}

	पूर्णाङ्क erase(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क index = do_lookup(कुंजी, hash);
		प्रत्यागम do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		प्रत्यागम ++it;
	}

	पूर्णाङ्क गिनती(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गिनती(ध्रुव K &कुंजी, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		प्रत्यागम i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम end();
		प्रत्यागम iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम end();
		प्रत्यागम const_iterator(this, i);
	}

	bool operator[](ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णाङ्क i = do_lookup(कुंजी, hash);
		प्रत्यागम i >= 0;
	}

	template<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ध्रुव entry_t &a, ध्रुव entry_t &b){ प्रत्यागम comp(b.udata, a.udata); });
		do_rehash();
	}

	K निकालो()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		प्रत्यागम ret;
	}

	शून्य swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ध्रुव pool &other) ध्रुव {
		यदि (size() != other.size())
			प्रत्यागम false;
		प्रति (auto &it : entries)
			यदि (!other.गिनती(it.udata))
				प्रत्यागम false;
		प्रत्यागम true;
	}

	bool operator!=(ध्रुव pool &other) ध्रुव {
		प्रत्यागम !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		प्रति (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		प्रत्यागम comm.hash_into(h);
	}

	शून्य reserve(size_t n) { entries.reserve(n); }
	size_t size() ध्रुव { प्रत्यागम entries.size(); }
	bool empty() ध्रुव { प्रत्यागम entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { प्रत्यागम iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { प्रत्यागम iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { प्रत्यागम iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() ध्रुव { प्रत्यागम const_iterator(nullptr, -1); }
};

template<typename K, पूर्णाङ्क offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

सार्वजनिक:
	class const_iterator
	{
		friend class idict;
	रक्षित:
		ध्रुव idict &container;
		पूर्णाङ्क index;
		const_iterator(ध्रुव idict &container, पूर्णाङ्क index) : container(container), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मूल्य_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; प्रत्यागम *this; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम index != other.index; }
		ध्रुव K &operator*() ध्रुव { प्रत्यागम container[index]; }
		ध्रुव K *operator->() ध्रुव { प्रत्यागम &container[index]; }
	};

	constexpr idict()
	{
	}

	पूर्णाङ्क operator()(ध्रुव K &कुंजी)
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णाङ्क i = database.do_lookup(कुंजी, hash);
		यदि (i < 0)
			i = database.do_जोड़ो(कुंजी, hash);
		प्रत्यागम i + offset;
	}

	पूर्णाङ्क at(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			throw std::out_of_range("idict::at()");
		प्रत्यागम i + offset;
	}

	पूर्णाङ्क at(ध्रुव K &कुंजी, पूर्णाङ्क defval) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुंजी, hash);
		यदि (i < 0)
			प्रत्यागम defval;
		प्रत्यागम i + offset;
	}

	पूर्णाङ्क गिनती(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुंजी, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क lookup(ध्रुव K &कुंजी) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		प्रत्यागम database.do_lookup_no_rehash(कुंजी, hash);
	}

	शून्य expect(ध्रुव K &कुंजी, पूर्णाङ्क i)
	{
		पूर्णाङ्क j = (*this)(कुंजी);
		यदि (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ध्रुव K &operator[](पूर्णाङ्क index) ध्रुव
	{
		प्रत्यागम database.entries.at(index - offset).udata;
	}

	शून्य swap(idict &other)
	{
		database.swap(other.database);
	}

	शून्य reserve(size_t n) { database.reserve(n); }
	size_t size() ध्रुव { प्रत्यागम database.size(); }
	bool empty() ध्रुव { प्रत्यागम database.empty(); }
	शून्य clear() { database.clear(); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(*this, offset); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(*this, n); }
	const_iterator end() ध्रुव { प्रत्यागम const_iterator(*this, offset + size()); }
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
	सार्वजनिक:
		explicit AtomicParent(पूर्णाङ्क p) : parent(p) {}
		AtomicParent(ध्रुव AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(ध्रुव AtomicParent &other) { set(other.get()); प्रत्यागम *this; }
		पूर्णाङ्क get() ध्रुव { प्रत्यागम parent.load(std::memory_order_relaxed); }
		शून्य set(पूर्णाङ्क p) { parent.store(p, std::memory_order_relaxed); }
	गुप्त:
		std::atomic<पूर्णाङ्क> parent;
	};
	std::vector<AtomicParent> parents;

सार्वजनिक:
	प्रकारनिर्देश typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	पूर्णाङ्क operator()(ध्रुव K &कुंजी)
	{
		पूर्णाङ्क i = database(कुंजी);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		यदि (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		प्रत्यागम i;
	}

	// Finds an element at given index
	ध्रुव K &operator[](पूर्णाङ्क index) ध्रुव
	{
		प्रत्यागम database[index];
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
	पूर्णाङ्क ifind(पूर्णाङ्क i) ध्रुव
	{
		पूर्णाङ्क p = i, k = i;

		यावत् (true) {
			पूर्णाङ्क pp = parents[p].get();
			यदि (pp < 0)
				भङ्ग;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		यावत् (k != p) {
			पूर्णाङ्क next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		प्रत्यागम p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	शून्य imerge(पूर्णाङ्क i, पूर्णाङ्क j)
	{
		i = ifind(i);
		j = ifind(j);

		यदि (i != j)
			parents[i].set(j);
	}

	शून्य ipromote(पूर्णाङ्क i)
	{
		पूर्णाङ्क k = i;

		यावत् (k != -1) {
			पूर्णाङ्क next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	पूर्णाङ्क lookup(ध्रुव K &a)
	{
		प्रत्यागम ifind((*this)(a));
	}

	ध्रुव K &find(ध्रुव K &a) ध्रुव
	{
		पूर्णाङ्क i = database.at(a, -1);
		यदि (i < 0)
			प्रत्यागम a;
		प्रत्यागम (*this)[ifind(i)];
	}

	शून्य merge(ध्रुव K &a, ध्रुव K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	शून्य promote(ध्रुव K &a)
	{
		पूर्णाङ्क i = database.at(a, -1);
		यदि (i >= 0)
			ipromote(i);
	}

	शून्य swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	शून्य reserve(size_t n) { database.reserve(n); }
	size_t size() ध्रुव { प्रत्यागम database.size(); }
	bool empty() ध्रुव { प्रत्यागम database.empty(); }
	शून्य clear() { database.clear(); parents.clear(); }

	const_iterator begin() ध्रुव { प्रत्यागम database.begin(); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम database.element(n); }
	const_iterator end() ध्रुव { प्रत्यागम database.end(); }
};

} /* namespace hashlib */

#endif
