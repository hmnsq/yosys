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

#सचे defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#अञ्ञथा
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

ध्रुव पूर्णाङ्क hashtable_size_trigger = 2;
ध्रुव पूर्णाङ्क hashtable_size_factor = 3;

namespace legacy {
	अन्तःस्थ uint32_t djb2_add(uint32_t a, uint32_t b) {
		पटिनिवत्त ((a << 5) + a) + b;
	}
};

template<typename T>
संरचना hash_ops;

अन्तःस्थ unsigned पूर्णाङ्क mkhash_xorshift(unsigned पूर्णाङ्क a) {
	सचे (परिमाण(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} अञ्ञथा सचे (परिमाण(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} अञ्ञथा
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	पटिनिवत्त a;
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
		पटिनिवत्त hash;
	}
सार्वजनिक:
	शून्य hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		पटिनिवत्त;
	}
	शून्य hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		पटिनिवत्त;
	}
	[[nodiscard]]
	hash_t yield() ध्रुव {
		पटिनिवत्त (hash_t)state;
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
#define HASH_शिखरम्_LOOP_FST [[nodiscard]] स्थिर अन्तःस्थ Hasher hash
#define HASH_शिखरम्_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	पटिनिवत्त h; \
}

template<typename T>
संरचना hash_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव T &a, ध्रुव T &b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T &a, Hasher h) {
		सचे constexpr (std::is_integral_v<T>) {
			static_assert(परिमाण(T) <= परिमाण(uint64_t));
			सचे (परिमाण(T) == परिमाण(uint64_t))
				h.hash64(a);
			अञ्ञथा
				h.hash32(a);
			पटिनिवत्त h;
		} अञ्ञथा सचे constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			पटिनिवत्त hash_ops<u_type>::hash_into((u_type) a, h);
		} अञ्ञथा सचे constexpr (std::is_pointer_v<T>) {
			पटिनिवत्त hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} अञ्ञथा सचे constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			पूर्णाङ्क size = a.size();
			पूर्णाङ्क i = 0;
			याव (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			पटिनिवत्त h;
		} अञ्ञथा {
			पटिनिवत्त a.hash_into(h);
		}
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव T &a) HASH_शिखरम्_LOOP_SND
};

template<typename P, typename Q> संरचना hash_ops<std::pair<P, Q>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::pair<P, Q> &a, ध्रुव std::pair<P, Q> &b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		पटिनिवत्त h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::pair<P, Q> &a) HASH_शिखरम्_LOOP_SND
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव P &p, ध्रुव Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		पटिनिवत्त h;
	}
};

template<typename... T> संरचना hash_ops<std::tuple<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::tuple<T...> &a, ध्रुव std::tuple<T...> &b) {
		पटिनिवत्त a == b;
	}
	template<size_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I == परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &, Hasher h) {
		पटिनिवत्त h;
	}
	template<size_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I != परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &a, Hasher h) {
		प्रकारनिर्देश hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		पटिनिवत्त h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::tuple<T...> &a) HASH_शिखरम्_LOOP_SND
};

template<typename T> संरचना hash_ops<std::vector<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::vector<T> &a, ध्रुव std::vector<T> &b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		अत्थाय (auto k : a)
			h.eat(k);
		पटिनिवत्त h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::vector<T> &a) HASH_शिखरम्_LOOP_SND
};

template<typename T, size_t N> संरचना hash_ops<std::array<T, N>> {
    स्थिर अन्तःस्थ bool cmp(ध्रुव std::array<T, N> &a, ध्रुव std::array<T, N> &b) {
        पटिनिवत्त a == b;
    }
    [[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::array<T, N> &a, Hasher h) {
        अत्थाय (ध्रुव auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        पटिनिवत्त h;
    }
	HASH_शिखरम्_LOOP_FST (ध्रुव std::array<T, N> &a) HASH_शिखरम्_LOOP_SND
};

संरचना hash_cstr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
		पटिनिवत्त strcmp(a, b) == 0;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव वर्ण *a, Hasher h) {
		याव (*a)
			h.hash32(*(a++));
		पटिनिवत्त h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव वर्ण *a) HASH_शिखरम्_LOOP_SND
};

template <> संरचना hash_ops<वर्ण*> : hash_cstr_ops {};

संरचना hash_ptr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव शून्य *a, Hasher h) {
		पटिनिवत्त hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव शून्य *a) HASH_शिखरम्_LOOP_SND
};

संरचना hash_obj_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		पटिनिवत्त a == b;
	}
	template<typename T>
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T *a, Hasher h) {
		सचे (a)
			h = a->hash_into(h);
		अञ्ञथा
			h.eat(0);
		पटिनिवत्त h;
	}
	template<typename T>
	HASH_शिखरम्_LOOP_FST (ध्रुव T *a) HASH_शिखरम्_LOOP_SND
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
	पटिनिवत्त hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
अन्तःस्थ unsigned पूर्णाङ्क mkhash(ध्रुव T &v) {
	पटिनिवत्त (unsigned पूर्णाङ्क) run_hash<T>(v);
}

template<> संरचना hash_ops<std::monostate> {
	स्थिर अन्तःस्थ bool cmp(std::monostate a, std::monostate b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(std::monostate, Hasher h) {
		पटिनिवत्त h;
	}
};

template<typename... T> संरचना hash_ops<std::variant<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::variant<T...> &a, ध्रुव std::variant<T...> &b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::variant<T...> &a, Hasher h) {
		std::visit([& h](ध्रुव auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		पटिनिवत्त h;
	}
};

template<typename T> संरचना hash_ops<std::optional<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::optional<T> &a, ध्रुव std::optional<T> &b) {
		पटिनिवत्त a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::optional<T> &a, Hasher h) {
		सचे(a.has_मानम्())
			h.eat(*a);
		अञ्ञथा
			h.eat(0);
		पटिनिवत्त h;
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

	अत्थाय (auto p : zero_and_some_primes)
		सचे (p >= min_size) पटिनिवत्त p;

	सचे (परिमाण(unsigned पूर्णाङ्क) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	अत्थाय (auto p : zero_and_some_primes)
		सचे (100129 * p > min_size) पटिनिवत्त 100129 * p;

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
		अत्थाय (auto b : buckets)
			h.eat(b);
		पटिनिवत्त h;
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
		bool operator<(ध्रुव entry_t &other) ध्रुव { पटिनिवत्त udata.first < other.udata.first; }
	};

	std::vector<पूर्णाङ्क> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अञ्ञथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		सचे (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = 0;
		सचे (!hashtable.empty())
			hash = ops.hash(कुञ्चिका).yield() % (unsigned पूर्णाङ्क)(hashtable.size());
		पटिनिवत्त hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		अत्थाय (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णाङ्क(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णाङ्क(entries.size()));
		सचे (hashtable.empty() || index < 0)
			पटिनिवत्त 0;

		पूर्णाङ्क k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));

		सचे (k == index) {
			hashtable[hash] = entries[index].next;
		} अञ्ञथा {
			याव (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णाङ्क back_idx = entries.size()-1;

		सचे (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));

			सचे (k == back_idx) {
				hashtable[back_hash] = index;
			} अञ्ञथा {
				याव (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.निष्कासय_back();

		सचे (entries.empty())
			hashtable.clear();

		पटिनिवत्त 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुञ्चिका, Hasher::hash_t &hash)
	{
		सचे (hashtable.empty())
			पटिनिवत्त -1;

		सचे (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(कुञ्चिका);
		}

		पटिनिवत्त do_lookup_internal(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_lookup_internal(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क index = hashtable[hash];

		याव (index >= 0 && !ops.cmp(entries[index].udata.first, कुञ्चिका)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		पटिनिवत्त index;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		सचे (hashtable.empty())
			पटिनिवत्त -1;

		पटिनिवत्त do_lookup_internal(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_निवेशय(ध्रुव K &कुञ्चिका, ध्रुव Hasher::hash_t &hash)
	{
		सचे (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(कुञ्चिका, T()), -1);
			do_rehash();
		} अञ्ञथा {
			entries.emplace_back(std::pair<K, T>(कुञ्चिका, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		पटिनिवत्त entries.size() - 1;
	}

	पूर्णाङ्क do_निवेशय(ध्रुव std::pair<K, T> &मानम्, ध्रुव Hasher::hash_t &hash)
	{
		सचे (hashtable.empty()) {
			entries.emplace_back(मानम्, -1);
			do_rehash();
		} अञ्ञथा {
			entries.emplace_back(मानम्, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		पटिनिवत्त entries.size() - 1;
	}

	पूर्णाङ्क do_निवेशय(std::pair<K, T> &&rvalue, ध्रुव Hasher::hash_t &hash)
	{
		सचे (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अञ्ञथा {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		पटिनिवत्त entries.size() - 1;
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
		प्रकारनिर्देश std::pair<K, T> मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश ध्रुव std::pair<K, T>* pointer;
		प्रकारनिर्देश ध्रुव std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; पटिनिवत्त *this; }
		const_iterator operator++(पूर्णाङ्क) { const_iterator tmp = *this; index--; पटिनिवत्त tmp; }
		const_iterator operator--() { index++; पटिनिवत्त *this; }
		const_iterator operator--(पूर्णाङ्क) { const_iterator tmp = *this; index++; पटिनिवत्त tmp; }
		const_iterator operator+=(पूर्णाङ्क amt) { index -= amt; पटिनिवत्त *this; }
		bool operator<(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index > other.index; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index != other.index; }
		ध्रुव std::pair<K, T> &operator*() ध्रुव { पटिनिवत्त ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> *operator->() ध्रुव { पटिनिवत्त &ptr->entries[index].udata; }
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
		प्रकारनिर्देश std::pair<K, T> मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश std::pair<K, T>* pointer;
		प्रकारनिर्देश std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; पटिनिवत्त *this; }
		iterator operator+=(पूर्णाङ्क amt) { index -= amt; पटिनिवत्त *this; }
		bool operator<(ध्रुव iterator &other) ध्रुव { पटिनिवत्त index > other.index; }
		bool operator==(ध्रुव iterator &other) ध्रुव { पटिनिवत्त index == other.index; }
		bool operator!=(ध्रुव iterator &other) ध्रुव { पटिनिवत्त index != other.index; }
		std::pair<K, T> &operator*() { पटिनिवत्त ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { पटिनिवत्त &ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> &operator*() ध्रुव { पटिनिवत्त ptr->entries[index].udata; }
		ध्रुव std::pair<K, T> *operator->() ध्रुव { पटिनिवत्त &ptr->entries[index].udata; }
		operator const_iterator() ध्रुव { पटिनिवत्त const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ध्रुव {
		पटिनिवत्त std::make_reverse_iterator(end());
	}
	reverse_iterator rend() ध्रुव {
		पटिनिवत्त std::make_reverse_iterator(begin());
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
		पटिनिवत्त *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		पटिनिवत्त *this;
	}

	dict(ध्रुव std::initializer_list<std::pair<K, T>> &list)
	{
		अत्थाय (auto &it : list)
			निवेशय(it);
	}

	template<class निवेशः_Iterator>
	dict(निवेशः_Iterator first, निवेशः_Iterator last)
	{
		निवेशय(first, last);
	}

	template<class निवेशः_Iterator>
	शून्य निवेशय(निवेशः_Iterator first, निवेशः_Iterator last)
	{
		अत्थाय (; first != last; ++first)
			निवेशय(*first);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(कुञ्चिका, hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव std::pair<K, T> &मानम्)
	{
		Hasher::hash_t hash = do_hash(मानम्.first);
		पूर्णाङ्क i = do_lookup(मानम्.first, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(मानम्, hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णाङ्क i = do_lookup(rvalue.first, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::forward<std::pair<K, T>>(rvalue), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुञ्चिका, T ध्रुव &मानम्)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(कुञ्चिका, मानम्), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुञ्चिका, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(कुञ्चिका, std::forward<T>(rvalue)), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ध्रुव &मानम्)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(std::forward<K>(rkey), मानम्), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णाङ्क erase(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क index = do_lookup(कुञ्चिका, hash);
		पटिनिवत्त do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		पटिनिवत्त ++it;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		पटिनिवत्त i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		पटिनिवत्त i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त end();
		पटिनिवत्त iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त end();
		पटिनिवत्त const_iterator(this, i);
	}

	T& at(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i < 0)
			throw std::out_of_range("dict::at()");
		पटिनिवत्त entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			throw std::out_of_range("dict::at()");
		पटिनिवत्त entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुञ्चिका, ध्रुव T &defval) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त defval;
		पटिनिवत्त entries[i].udata.second;
	}

	T& operator[](ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i < 0)
			i = do_निवेशय(std::pair<K, T>(कुञ्चिका, T()), hash);
		पटिनिवत्त entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ध्रुव entry_t &a, ध्रुव entry_t &b){ पटिनिवत्त comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	शून्य swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ध्रुव dict &other) ध्रुव {
		सचे (size() != other.size())
			पटिनिवत्त false;
		अत्थाय (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			सचे (oit == other.end() || !(oit->second == it.udata.second))
				पटिनिवत्त false;
		}
		पटिनिवत्त true;
	}

	bool operator!=(ध्रुव dict &other) ध्रुव {
		पटिनिवत्त !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		अत्थाय (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		पटिनिवत्त comm.hash_into(h);
	}

	शून्य reserve(size_t n) { entries.reserve(n); }
	size_t size() ध्रुव { पटिनिवत्त entries.size(); }
	bool empty() ध्रुव { पटिनिवत्त entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { पटिनिवत्त iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { पटिनिवत्त iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { पटिनिवत्त iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { पटिनिवत्त const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { पटिनिवत्त const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() ध्रुव { पटिनिवत्त const_iterator(this, -1); }
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
#अञ्ञथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		सचे (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = 0;
		सचे (!hashtable.empty())
			hash = ops.hash(कुञ्चिका).yield() % (unsigned पूर्णाङ्क)(hashtable.size());
		पटिनिवत्त hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		अत्थाय (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णाङ्क(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णाङ्क(entries.size()));
		सचे (hashtable.empty() || index < 0)
			पटिनिवत्त 0;

		पूर्णाङ्क k = hashtable[hash];
		सचे (k == index) {
			hashtable[hash] = entries[index].next;
		} अञ्ञथा {
			याव (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णाङ्क back_idx = entries.size()-1;

		सचे (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			सचे (k == back_idx) {
				hashtable[back_hash] = index;
			} अञ्ञथा {
				याव (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.निष्कासय_back();

		सचे (entries.empty())
			hashtable.clear();

		पटिनिवत्त 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुञ्चिका, Hasher::hash_t &hash)
	{
		सचे (hashtable.empty())
			पटिनिवत्त -1;

		सचे (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(कुञ्चिका);
		}

		पटिनिवत्त do_lookup_internal(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_lookup_internal(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क index = hashtable[hash];

		याव (index >= 0 && !ops.cmp(entries[index].udata, कुञ्चिका)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		पटिनिवत्त index;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		सचे (hashtable.empty())
			पटिनिवत्त -1;

		पटिनिवत्त do_lookup_internal(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_निवेशय(ध्रुव K &मानम्, Hasher::hash_t &hash)
	{
		सचे (hashtable.empty()) {
			entries.emplace_back(मानम्, -1);
			do_rehash();
			hash = do_hash(मानम्);
		} अञ्ञथा {
			entries.emplace_back(मानम्, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		पटिनिवत्त entries.size() - 1;
	}

	पूर्णाङ्क do_निवेशय(K &&rvalue, Hasher::hash_t &hash)
	{
		सचे (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} अञ्ञथा {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		पटिनिवत्त entries.size() - 1;
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
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; पटिनिवत्त *this; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index != other.index; }
		ध्रुव K &operator*() ध्रुव { पटिनिवत्त ptr->entries[index].udata; }
		ध्रुव K *operator->() ध्रुव { पटिनिवत्त &ptr->entries[index].udata; }
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
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		iterator() { }
		iterator operator++() { index--; पटिनिवत्त *this; }
		bool operator==(ध्रुव iterator &other) ध्रुव { पटिनिवत्त index == other.index; }
		bool operator!=(ध्रुव iterator &other) ध्रुव { पटिनिवत्त index != other.index; }
		K &operator*() { पटिनिवत्त ptr->entries[index].udata; }
		K *operator->() { पटिनिवत्त &ptr->entries[index].udata; }
		ध्रुव K &operator*() ध्रुव { पटिनिवत्त ptr->entries[index].udata; }
		ध्रुव K *operator->() ध्रुव { पटिनिवत्त &ptr->entries[index].udata; }
		operator const_iterator() ध्रुव { पटिनिवत्त const_iterator(ptr, index); }
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
		पटिनिवत्त *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		पटिनिवत्त *this;
	}

	pool(ध्रुव std::initializer_list<K> &list)
	{
		अत्थाय (auto &it : list)
			निवेशय(it);
	}

	template<class निवेशः_Iterator>
	pool(निवेशः_Iterator first, निवेशः_Iterator last)
	{
		निवेशय(first, last);
	}

	template<class निवेशः_Iterator>
	शून्य निवेशय(निवेशः_Iterator first, निवेशः_Iterator last)
	{
		अत्थाय (; first != last; ++first)
			निवेशय(*first);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव K &मानम्)
	{
		Hasher::hash_t hash = do_hash(मानम्);
		पूर्णाङ्क i = do_lookup(मानम्, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(मानम्, hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णाङ्क i = do_lookup(rvalue, hash);
		सचे (i >= 0)
			पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::forward<K>(rvalue), hash);
		पटिनिवत्त std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		पटिनिवत्त निवेशय(K(std::forward<Args>(args)...));
	}

	पूर्णाङ्क erase(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क index = do_lookup(कुञ्चिका, hash);
		पटिनिवत्त do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		पटिनिवत्त ++it;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		पटिनिवत्त i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		पटिनिवत्त i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त end();
		पटिनिवत्त iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त end();
		पटिनिवत्त const_iterator(this, i);
	}

	bool operator[](ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		पटिनिवत्त i >= 0;
	}

	template<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](ध्रुव entry_t &a, ध्रुव entry_t &b){ पटिनिवत्त comp(b.udata, a.udata); });
		do_rehash();
	}

	K निष्कासय()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		पटिनिवत्त ret;
	}

	शून्य swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(ध्रुव pool &other) ध्रुव {
		सचे (size() != other.size())
			पटिनिवत्त false;
		अत्थाय (auto &it : entries)
			सचे (!other.गणनम्(it.udata))
				पटिनिवत्त false;
		पटिनिवत्त true;
	}

	bool operator!=(ध्रुव pool &other) ध्रुव {
		पटिनिवत्त !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		अत्थाय (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		पटिनिवत्त comm.hash_into(h);
	}

	शून्य reserve(size_t n) { entries.reserve(n); }
	size_t size() ध्रुव { पटिनिवत्त entries.size(); }
	bool empty() ध्रुव { पटिनिवत्त entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { पटिनिवत्त iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { पटिनिवत्त iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { पटिनिवत्त iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { पटिनिवत्त const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { पटिनिवत्त const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() ध्रुव { पटिनिवत्त const_iterator(nullptr, -1); }
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
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; पटिनिवत्त *this; }
		bool operator==(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index == other.index; }
		bool operator!=(ध्रुव const_iterator &other) ध्रुव { पटिनिवत्त index != other.index; }
		ध्रुव K &operator*() ध्रुव { पटिनिवत्त container[index]; }
		ध्रुव K *operator->() ध्रुव { पटिनिवत्त &container[index]; }
	};

	constexpr idict()
	{
	}

	पूर्णाङ्क operator()(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup(कुञ्चिका, hash);
		सचे (i < 0)
			i = database.do_निवेशय(कुञ्चिका, hash);
		पटिनिवत्त i + offset;
	}

	पूर्णाङ्क at(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			throw std::out_of_range("idict::at()");
		पटिनिवत्त i + offset;
	}

	पूर्णाङ्क at(ध्रुव K &कुञ्चिका, पूर्णाङ्क defval) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		सचे (i < 0)
			पटिनिवत्त defval;
		पटिनिवत्त i + offset;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		पटिनिवत्त i < 0 ? 0 : 1;
	}

	पूर्णाङ्क lookup(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पटिनिवत्त database.do_lookup_no_rehash(कुञ्चिका, hash);
	}

	शून्य expect(ध्रुव K &कुञ्चिका, पूर्णाङ्क i)
	{
		पूर्णाङ्क j = (*this)(कुञ्चिका);
		सचे (i != j)
			throw std::out_of_range("idict::expect()");
	}

	ध्रुव K &operator[](पूर्णाङ्क index) ध्रुव
	{
		पटिनिवत्त database.entries.at(index - offset).udata;
	}

	शून्य swap(idict &other)
	{
		database.swap(other.database);
	}

	शून्य reserve(size_t n) { database.reserve(n); }
	size_t size() ध्रुव { पटिनिवत्त database.size(); }
	bool empty() ध्रुव { पटिनिवत्त database.empty(); }
	शून्य clear() { database.clear(); }

	const_iterator begin() ध्रुव { पटिनिवत्त const_iterator(*this, offset); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { पटिनिवत्त const_iterator(*this, n); }
	const_iterator end() ध्रुव { पटिनिवत्त const_iterator(*this, offset + size()); }
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
		AtomicParent &operator=(ध्रुव AtomicParent &other) { set(other.get()); पटिनिवत्त *this; }
		पूर्णाङ्क get() ध्रुव { पटिनिवत्त parent.load(std::memory_order_relaxed); }
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
	पूर्णाङ्क operator()(ध्रुव K &कुञ्चिका)
	{
		पूर्णाङ्क i = database(कुञ्चिका);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		सचे (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		पटिनिवत्त i;
	}

	// Finds an element at given index
	ध्रुव K &operator[](पूर्णाङ्क index) ध्रुव
	{
		पटिनिवत्त database[index];
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

		याव (true) {
			पूर्णाङ्क pp = parents[p].get();
			सचे (pp < 0)
				भङ्ग;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		याव (k != p) {
			पूर्णाङ्क next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		पटिनिवत्त p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	शून्य imerge(पूर्णाङ्क i, पूर्णाङ्क j)
	{
		i = ifind(i);
		j = ifind(j);

		सचे (i != j)
			parents[i].set(j);
	}

	शून्य ipromote(पूर्णाङ्क i)
	{
		पूर्णाङ्क k = i;

		याव (k != -1) {
			पूर्णाङ्क next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	पूर्णाङ्क lookup(ध्रुव K &a)
	{
		पटिनिवत्त ifind((*this)(a));
	}

	ध्रुव K &find(ध्रुव K &a) ध्रुव
	{
		पूर्णाङ्क i = database.at(a, -1);
		सचे (i < 0)
			पटिनिवत्त a;
		पटिनिवत्त (*this)[ifind(i)];
	}

	शून्य merge(ध्रुव K &a, ध्रुव K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	शून्य promote(ध्रुव K &a)
	{
		पूर्णाङ्क i = database.at(a, -1);
		सचे (i >= 0)
			ipromote(i);
	}

	शून्य swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	शून्य reserve(size_t n) { database.reserve(n); }
	size_t size() ध्रुव { पटिनिवत्त database.size(); }
	bool empty() ध्रुव { पटिनिवत्त database.empty(); }
	शून्य clear() { database.clear(); parents.clear(); }

	const_iterator begin() ध्रुव { पटिनिवत्त database.begin(); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { पटिनिवत्त database.element(n); }
	const_iterator end() ध्रुव { पटिनिवत्त database.end(); }
};

} /* namespace hashlib */

#endif
