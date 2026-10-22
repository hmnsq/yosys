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

अचर पूर्णाङ्क hashtable_size_trigger = 2;
अचर पूर्णाङ्क hashtable_size_factor = 3;

namespace legacy {
	अन्तःस्थ uint32_t djb2_add(uint32_t a, uint32_t b) {
		फर्काऊ ((a << 5) + a) + b;
	}
};

ढाँचा<typename T>
संरचना hash_ops;

अन्तःस्थ अचिन्हित पूर्णाङ्क mkhash_xorshift(अचिन्हित पूर्णाङ्क a) {
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
	फर्काऊ a;
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
		फर्काऊ hash;
	}
सार्वजनिक:
	शून्य hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		फर्काऊ;
	}
	शून्य hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		फर्काऊ;
	}
	[[nodiscard]]
	hash_t yield() अचर {
		फर्काऊ (hash_t)state;
	}

	ढाँचा<typename T>
	शून्य eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	ढाँचा<typename T>
	शून्य eat(अचर T& t) {
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
	फर्काऊ h; \
}

ढाँचा<typename T>
संरचना hash_ops {
	स्थिर अन्तःस्थ bool cmp(अचर T &a, अचर T &b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर T &a, Hasher h) {
		यदि constexpr (std::is_integral_v<T>) {
			static_assert(परिमाण(T) <= परिमाण(uint64_t));
			यदि (परिमाण(T) == परिमाण(uint64_t))
				h.hash64(a);
			अन्यथा
				h.hash32(a);
			फर्काऊ h;
		} अन्यथा यदि constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			फर्काऊ hash_ops<u_type>::hash_into((u_type) a, h);
		} अन्यथा यदि constexpr (std::is_pointer_v<T>) {
			फर्काऊ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} अन्यथा यदि constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			पूर्णाङ्क size = a.size();
			पूर्णाङ्क i = 0;
			जबसम्म (i + 8 < size) {
				uint64_t v;
				स_नकल(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			स_नकल(&v, a.data() + i, size - i);
			h.hash64(v);
			फर्काऊ h;
		} अन्यथा {
			फर्काऊ a.hash_into(h);
		}
	}
	HASH_शीर्ष_LOOP_FST (अचर T &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename P, typename Q> संरचना hash_ops<std::pair<P, Q>> {
	स्थिर अन्तःस्थ bool cmp(अचर std::pair<P, Q> &a, अचर std::pair<P, Q> &b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		फर्काऊ h;
	}
	HASH_शीर्ष_LOOP_FST (अचर std::pair<P, Q> &a) HASH_शीर्ष_LOOP_SND
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(अचर P &p, अचर Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		फर्काऊ h;
	}
};

ढाँचा<typename... T> संरचना hash_ops<std::tuple<T...>> {
	स्थिर अन्तःस्थ bool cmp(अचर std::tuple<T...> &a, अचर std::tuple<T...> &b) {
		फर्काऊ a == b;
	}
	ढाँचा<माप_प्रकार I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I == परिमाण...(T), Hasher>::type hash_into(अचर std::tuple<T...> &, Hasher h) {
		फर्काऊ h;
	}
	ढाँचा<माप_प्रकार I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I != परिमाण...(T), Hasher>::type hash_into(अचर std::tuple<T...> &a, Hasher h) {
		प्रकारनिर्देश hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		फर्काऊ h;
	}
	HASH_शीर्ष_LOOP_FST (अचर std::tuple<T...> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T> संरचना hash_ops<std::vector<T>> {
	स्थिर अन्तःस्थ bool cmp(अचर std::vector<T> &a, अचर std::vector<T> &b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		लागि (स्वतः k : a)
			h.eat(k);
		फर्काऊ h;
	}
	HASH_शीर्ष_LOOP_FST (अचर std::vector<T> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T, माप_प्रकार N> संरचना hash_ops<std::array<T, N>> {
    स्थिर अन्तःस्थ bool cmp(अचर std::array<T, N> &a, अचर std::array<T, N> &b) {
        फर्काऊ a == b;
    }
    [[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर std::array<T, N> &a, Hasher h) {
        लागि (अचर स्वतः& k : a)
            h = hash_ops<T>::hash_into(k, h);
        फर्काऊ h;
    }
	HASH_शीर्ष_LOOP_FST (अचर std::array<T, N> &a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_cstr_ops {
	स्थिर अन्तःस्थ bool cmp(अचर वर्ण *a, अचर वर्ण *b) {
		फर्काऊ म_भेद(a, b) == 0;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर वर्ण *a, Hasher h) {
		जबसम्म (*a)
			h.hash32(*(a++));
		फर्काऊ h;
	}
	HASH_शीर्ष_LOOP_FST (अचर वर्ण *a) HASH_शीर्ष_LOOP_SND
};

ढाँचा <> संरचना hash_ops<वर्ण*> : hash_cstr_ops {};

संरचना hash_ptr_ops {
	स्थिर अन्तःस्थ bool cmp(अचर शून्य *a, अचर शून्य *b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर शून्य *a, Hasher h) {
		फर्काऊ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शीर्ष_LOOP_FST (अचर शून्य *a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_obj_ops {
	स्थिर अन्तःस्थ bool cmp(अचर शून्य *a, अचर शून्य *b) {
		फर्काऊ a == b;
	}
	ढाँचा<typename T>
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर T *a, Hasher h) {
		यदि (a)
			h = a->hash_into(h);
		अन्यथा
			h.eat(0);
		फर्काऊ h;
	}
	ढाँचा<typename T>
	HASH_शीर्ष_LOOP_FST (अचर T *a) HASH_शीर्ष_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
ढाँचा<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(अचर T& obj) {
	फर्काऊ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
ढाँचा<typename T>
[[nodiscard]]
[[deprecated]]
अन्तःस्थ अचिन्हित पूर्णाङ्क mkhash(अचर T &v) {
	फर्काऊ (अचिन्हित पूर्णाङ्क) run_hash<T>(v);
}

ढाँचा<> संरचना hash_ops<std::monostate> {
	स्थिर अन्तःस्थ bool cmp(std::monostate a, std::monostate b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(std::monostate, Hasher h) {
		फर्काऊ h;
	}
};

ढाँचा<typename... T> संरचना hash_ops<std::variant<T...>> {
	स्थिर अन्तःस्थ bool cmp(अचर std::variant<T...> &a, अचर std::variant<T...> &b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर std::variant<T...> &a, Hasher h) {
		std::visit([& h](अचर स्वतः &v) { h.eat(v); }, a);
		h.eat(a.index());
		फर्काऊ h;
	}
};

ढाँचा<typename T> संरचना hash_ops<std::optional<T>> {
	स्थिर अन्तःस्थ bool cmp(अचर std::optional<T> &a, अचर std::optional<T> &b) {
		फर्काऊ a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर std::optional<T> &a, Hasher h) {
		यदि(a.has_मान())
			h.eat(*a);
		अन्यथा
			h.eat(0);
		फर्काऊ h;
	}
};

अन्तःस्थ अचिन्हित पूर्णाङ्क hashtable_size(अचिन्हित पूर्णाङ्क min_size)
{
	// Primes as generated by https://oeis.org/A175953
	स्थिर std::vector<अचिन्हित पूर्णाङ्क> zero_and_some_primes = {
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

	लागि (स्वतः p : zero_and_some_primes)
		यदि (p >= min_size) फर्काऊ p;

	यदि (परिमाण(अचिन्हित पूर्णाङ्क) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	लागि (स्वतः p : zero_and_some_primes)
		यदि (100129 * p > min_size) फर्काऊ 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

ढाँचा<typename K, typename T, typename OPS = hash_ops<K>> class dict;
ढाँचा<typename K, पूर्णाङ्क offset = 0, typename OPS = hash_ops<K>> class idict;
ढाँचा<typename K, typename OPS = hash_ops<K>> class pool;
ढाँचा<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
सार्वजनिक:
	commutative_hash() {
		buckets.fill(0);
	}
	ढाँचा <typename T>
	शून्य eat(अचर T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	शून्य eat(अचर Hasher &h) {
		Hasher::hash_t v = h.yield();
		माप_प्रकार index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) अचर {
		लागि (स्वतः b : buckets)
			h.eat(b);
		फर्काऊ h;
	}
गुप्त:
	std::array<Hasher::hash_t, 4> buckets;
};

ढाँचा<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	संरचना entry_t
	{
		std::pair<K, T> udata;
		पूर्णाङ्क next;

		entry_t() { }
		entry_t(अचर std::pair<K, T> &udata, पूर्णाङ्क next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, पूर्णाङ्क next) : udata(std::move(udata)), next(next) { }
		bool चालक<(अचर entry_t &other) अचर { फर्काऊ udata.first < other.udata.first; }
	};

	std::vector<पूर्णाङ्क> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef न_संशोधन
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.empty())
			hash = ops.hash(साँचो).yield() % (अचिन्हित पूर्णाङ्क)(hashtable.size());
		फर्काऊ hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		लागि (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
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
			फर्काऊ 0;

		पूर्णाङ्क k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));

		यदि (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			जबसम्म (entries[k].next != index) {
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
				जबसम्म (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.झिक_back();

		यदि (entries.empty())
			hashtable.clear();

		फर्काऊ 1;
	}

	पूर्णाङ्क do_lookup(अचर K &साँचो, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty())
			फर्काऊ -1;

		यदि (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(साँचो);
		}

		फर्काऊ do_lookup_internal(साँचो, hash);
	}

	पूर्णाङ्क do_lookup_internal(अचर K &साँचो, Hasher::hash_t hash) अचर
	{
		पूर्णाङ्क index = hashtable[hash];

		जबसम्म (index >= 0 && !ops.cmp(entries[index].udata.first, साँचो)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		फर्काऊ index;
	}

	पूर्णाङ्क do_lookup_no_rehash(अचर K &साँचो, Hasher::hash_t hash) अचर
	{
		यदि (hashtable.empty())
			फर्काऊ -1;

		फर्काऊ do_lookup_internal(साँचो, hash);
	}

	पूर्णाङ्क do_घुसाउ(अचर K &साँचो, अचर Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(साँचो, T()), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::pair<K, T>(साँचो, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		फर्काऊ entries.size() - 1;
	}

	पूर्णाङ्क do_घुसाउ(अचर std::pair<K, T> &मान, अचर Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(मान, -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(मान, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		फर्काऊ entries.size() - 1;
	}

	पूर्णाङ्क do_घुसाउ(std::pair<K, T> &&rvalue, अचर Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		फर्काऊ entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्र class dict;
	रक्षित:
		अचर dict *ptr;
		पूर्णाङ्क index;
		const_iterator(अचर dict *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::bidirectional_iterator_tag iterator_category;
		प्रकारनिर्देश std::pair<K, T> मान_type;
		प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
		प्रकारनिर्देश अचर std::pair<K, T>* pointer;
		प्रकारनिर्देश अचर std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator चालक++() { index--; फर्काऊ *this; }
		const_iterator चालक++(पूर्णाङ्क) { const_iterator tmp = *this; index--; फर्काऊ tmp; }
		const_iterator चालक--() { index++; फर्काऊ *this; }
		const_iterator चालक--(पूर्णाङ्क) { const_iterator tmp = *this; index++; फर्काऊ tmp; }
		const_iterator चालक+=(पूर्णाङ्क amt) { index -= amt; फर्काऊ *this; }
		bool चालक<(अचर const_iterator &other) अचर { फर्काऊ index > other.index; }
		bool चालक==(अचर const_iterator &other) अचर { फर्काऊ index == other.index; }
		bool चालक!=(अचर const_iterator &other) अचर { फर्काऊ index != other.index; }
		अचर std::pair<K, T> &चालक*() अचर { फर्काऊ ptr->entries[index].udata; }
		अचर std::pair<K, T> *चालक->() अचर { फर्काऊ &ptr->entries[index].udata; }
	};

	class iterator
	{
		मित्र class dict;
	रक्षित:
		dict *ptr;
		पूर्णाङ्क index;
		iterator(dict *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश std::pair<K, T> मान_type;
		प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
		प्रकारनिर्देश std::pair<K, T>* pointer;
		प्रकारनिर्देश std::pair<K, T>& reference;
		iterator() { }
		iterator चालक++() { index--; फर्काऊ *this; }
		iterator चालक+=(पूर्णाङ्क amt) { index -= amt; फर्काऊ *this; }
		bool चालक<(अचर iterator &other) अचर { फर्काऊ index > other.index; }
		bool चालक==(अचर iterator &other) अचर { फर्काऊ index == other.index; }
		bool चालक!=(अचर iterator &other) अचर { फर्काऊ index != other.index; }
		std::pair<K, T> &चालक*() { फर्काऊ ptr->entries[index].udata; }
		std::pair<K, T> *चालक->() { फर्काऊ &ptr->entries[index].udata; }
		अचर std::pair<K, T> &चालक*() अचर { फर्काऊ ptr->entries[index].udata; }
		अचर std::pair<K, T> *चालक->() अचर { फर्काऊ &ptr->entries[index].udata; }
		चालक const_iterator() अचर { फर्काऊ const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() अचर {
		फर्काऊ std::make_reverse_iterator(end());
	}
	reverse_iterator rend() अचर {
		फर्काऊ std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(अचर dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &चालक=(अचर dict &other) {
		entries = other.entries;
		do_rehash();
		फर्काऊ *this;
	}

	dict &चालक=(dict &&other) noexcept {
		clear();
		swap(other);
		फर्काऊ *this;
	}

	dict(अचर std::initializer_list<std::pair<K, T>> &list)
	{
		लागि (स्वतः &it : list)
			घुसाउ(it);
	}

	ढाँचा<class आगत_Iterator>
	dict(आगत_Iterator first, आगत_Iterator last)
	{
		घुसाउ(first, last);
	}

	ढाँचा<class आगत_Iterator>
	शून्य घुसाउ(आगत_Iterator first, आगत_Iterator last)
	{
		लागि (; first != last; ++first)
			घुसाउ(*first);
	}

	std::pair<iterator, bool> घुसाउ(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(साँचो, hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घुसाउ(अचर std::pair<K, T> &मान)
	{
		Hasher::hash_t hash = do_hash(मान.first);
		पूर्णाङ्क i = do_lookup(मान.first, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(मान, hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घुसाउ(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णाङ्क i = do_lookup(rvalue.first, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::forward<std::pair<K, T>>(rvalue), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K अचर &साँचो, T अचर &मान)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::make_pair(साँचो, मान), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K अचर &साँचो, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::make_pair(साँचो, std::forward<T>(rvalue)), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T अचर &मान)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::make_pair(std::forward<K>(rkey), मान), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णाङ्क erase(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क index = do_lookup(साँचो, hash);
		फर्काऊ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		फर्काऊ ++it;
	}

	पूर्णाङ्क गन्ती(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		फर्काऊ i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गन्ती(अचर K &साँचो, const_iterator it) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		फर्काऊ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i < 0)
			फर्काऊ end();
		फर्काऊ iterator(this, i);
	}

	const_iterator find(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			फर्काऊ end();
		फर्काऊ const_iterator(this, i);
	}

	T& at(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i < 0)
			throw std::out_of_range("dict::at()");
		फर्काऊ entries[i].udata.second;
	}

	अचर T& at(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			throw std::out_of_range("dict::at()");
		फर्काऊ entries[i].udata.second;
	}

	अचर T& at(अचर K &साँचो, अचर T &defval) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			फर्काऊ defval;
		फर्काऊ entries[i].udata.second;
	}

	T& चालक[](अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i < 0)
			i = do_घुसाउ(std::pair<K, T>(साँचो, T()), hash);
		फर्काऊ entries[i].udata.second;
	}

	ढाँचा<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](अचर entry_t &a, अचर entry_t &b){ फर्काऊ comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	शून्य swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(अचर dict &other) अचर {
		यदि (size() != other.size())
			फर्काऊ false;
		लागि (स्वतः &it : entries) {
			स्वतः oit = other.find(it.udata.first);
			यदि (oit == other.end() || !(oit->second == it.udata.second))
				फर्काऊ false;
		}
		फर्काऊ true;
	}

	bool चालक!=(अचर dict &other) अचर {
		फर्काऊ !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर {
		commutative_hash comm;
		लागि (स्वतः &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		फर्काऊ comm.hash_into(h);
	}

	शून्य reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार size() अचर { फर्काऊ entries.size(); }
	bool empty() अचर { फर्काऊ entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { फर्काऊ iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { फर्काऊ iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { फर्काऊ iterator(nullptr, -1); }

	const_iterator begin() अचर { फर्काऊ const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) अचर { फर्काऊ const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() अचर { फर्काऊ const_iterator(this, -1); }
};

ढाँचा<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	ढाँचा<typename, पूर्णाङ्क, typename> मित्र class idict;

रक्षित:
	संरचना entry_t
	{
		K udata;
		पूर्णाङ्क next;

		entry_t() { }
		entry_t(अचर K &udata, पूर्णाङ्क next) : udata(udata), next(next) { }
		entry_t(K &&udata, पूर्णाङ्क next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<पूर्णाङ्क> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef न_संशोधन
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.empty())
			hash = ops.hash(साँचो).yield() % (अचिन्हित पूर्णाङ्क)(hashtable.size());
		फर्काऊ hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		लागि (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.size()); i++) {
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
			फर्काऊ 0;

		पूर्णाङ्क k = hashtable[hash];
		यदि (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			जबसम्म (entries[k].next != index) {
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
				जबसम्म (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.झिक_back();

		यदि (entries.empty())
			hashtable.clear();

		फर्काऊ 1;
	}

	पूर्णाङ्क do_lookup(अचर K &साँचो, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty())
			फर्काऊ -1;

		यदि (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(साँचो);
		}

		फर्काऊ do_lookup_internal(साँचो, hash);
	}

	पूर्णाङ्क do_lookup_internal(अचर K &साँचो, Hasher::hash_t hash) अचर
	{
		पूर्णाङ्क index = hashtable[hash];

		जबसम्म (index >= 0 && !ops.cmp(entries[index].udata, साँचो)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णाङ्क(entries.size()));
		}

		फर्काऊ index;
	}

	पूर्णाङ्क do_lookup_no_rehash(अचर K &साँचो, Hasher::hash_t hash) अचर
	{
		यदि (hashtable.empty())
			फर्काऊ -1;

		फर्काऊ do_lookup_internal(साँचो, hash);
	}

	पूर्णाङ्क do_घुसाउ(अचर K &मान, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(मान, -1);
			do_rehash();
			hash = do_hash(मान);
		} अन्यथा {
			entries.emplace_back(मान, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		फर्काऊ entries.size() - 1;
	}

	पूर्णाङ्क do_घुसाउ(K &&rvalue, Hasher::hash_t &hash)
	{
		यदि (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} अन्यथा {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		फर्काऊ entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्र class pool;
	रक्षित:
		अचर pool *ptr;
		पूर्णाङ्क index;
		const_iterator(अचर pool *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मान_type;
		प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator चालक++() { index--; फर्काऊ *this; }
		bool चालक==(अचर const_iterator &other) अचर { फर्काऊ index == other.index; }
		bool चालक!=(अचर const_iterator &other) अचर { फर्काऊ index != other.index; }
		अचर K &चालक*() अचर { फर्काऊ ptr->entries[index].udata; }
		अचर K *चालक->() अचर { फर्काऊ &ptr->entries[index].udata; }
	};

	class iterator
	{
		मित्र class pool;
	रक्षित:
		pool *ptr;
		पूर्णाङ्क index;
		iterator(pool *ptr, पूर्णाङ्क index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मान_type;
		प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		iterator() { }
		iterator चालक++() { index--; फर्काऊ *this; }
		bool चालक==(अचर iterator &other) अचर { फर्काऊ index == other.index; }
		bool चालक!=(अचर iterator &other) अचर { फर्काऊ index != other.index; }
		K &चालक*() { फर्काऊ ptr->entries[index].udata; }
		K *चालक->() { फर्काऊ &ptr->entries[index].udata; }
		अचर K &चालक*() अचर { फर्काऊ ptr->entries[index].udata; }
		अचर K *चालक->() अचर { फर्काऊ &ptr->entries[index].udata; }
		चालक const_iterator() अचर { फर्काऊ const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(अचर pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &चालक=(अचर pool &other) {
		entries = other.entries;
		do_rehash();
		फर्काऊ *this;
	}

	pool &चालक=(pool &&other) noexcept {
		clear();
		swap(other);
		फर्काऊ *this;
	}

	pool(अचर std::initializer_list<K> &list)
	{
		लागि (स्वतः &it : list)
			घुसाउ(it);
	}

	ढाँचा<class आगत_Iterator>
	pool(आगत_Iterator first, आगत_Iterator last)
	{
		घुसाउ(first, last);
	}

	ढाँचा<class आगत_Iterator>
	शून्य घुसाउ(आगत_Iterator first, आगत_Iterator last)
	{
		लागि (; first != last; ++first)
			घुसाउ(*first);
	}

	std::pair<iterator, bool> घुसाउ(अचर K &मान)
	{
		Hasher::hash_t hash = do_hash(मान);
		पूर्णाङ्क i = do_lookup(मान, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(मान, hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घुसाउ(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णाङ्क i = do_lookup(rvalue, hash);
		यदि (i >= 0)
			फर्काऊ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घुसाउ(std::forward<K>(rvalue), hash);
		फर्काऊ std::pair<iterator, bool>(iterator(this, i), true);
	}

	ढाँचा<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		फर्काऊ घुसाउ(K(std::forward<Args>(args)...));
	}

	पूर्णाङ्क erase(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क index = do_lookup(साँचो, hash);
		फर्काऊ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		फर्काऊ ++it;
	}

	पूर्णाङ्क गन्ती(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		फर्काऊ i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गन्ती(अचर K &साँचो, const_iterator it) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		फर्काऊ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		यदि (i < 0)
			फर्काऊ end();
		फर्काऊ iterator(this, i);
	}

	const_iterator find(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			फर्काऊ end();
		फर्काऊ const_iterator(this, i);
	}

	bool चालक[](अचर K &साँचो)
	{
		Hasher::hash_t hash = do_hash(साँचो);
		पूर्णाङ्क i = do_lookup(साँचो, hash);
		फर्काऊ i >= 0;
	}

	ढाँचा<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](अचर entry_t &a, अचर entry_t &b){ फर्काऊ comp(b.udata, a.udata); });
		do_rehash();
	}

	K झिक()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		फर्काऊ ret;
	}

	शून्य swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(अचर pool &other) अचर {
		यदि (size() != other.size())
			फर्काऊ false;
		लागि (स्वतः &it : entries)
			यदि (!other.गन्ती(it.udata))
				फर्काऊ false;
		फर्काऊ true;
	}

	bool चालक!=(अचर pool &other) अचर {
		फर्काऊ !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर {
		commutative_hash comm;
		लागि (स्वतः &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		फर्काऊ comm.hash_into(h);
	}

	शून्य reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार size() अचर { फर्काऊ entries.size(); }
	bool empty() अचर { फर्काऊ entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { फर्काऊ iterator(this, पूर्णाङ्क(entries.size())-1); }
	iterator element(पूर्णाङ्क n) { फर्काऊ iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	iterator end() { फर्काऊ iterator(nullptr, -1); }

	const_iterator begin() अचर { फर्काऊ const_iterator(this, पूर्णाङ्क(entries.size())-1); }
	const_iterator element(पूर्णाङ्क n) अचर { फर्काऊ const_iterator(this, पूर्णाङ्क(entries.size())-1-n); }
	const_iterator end() अचर { फर्काऊ const_iterator(nullptr, -1); }
};

ढाँचा<typename K, पूर्णाङ्क offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

सार्वजनिक:
	class const_iterator
	{
		मित्र class idict;
	रक्षित:
		अचर idict &container;
		पूर्णाङ्क index;
		const_iterator(अचर idict &container, पूर्णाङ्क index) : container(container), index(index) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_tag iterator_category;
		प्रकारनिर्देश K मान_type;
		प्रकारनिर्देश सूचक_भेद_प्रकार difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator चालक++() { index++; फर्काऊ *this; }
		bool चालक==(अचर const_iterator &other) अचर { फर्काऊ index == other.index; }
		bool चालक!=(अचर const_iterator &other) अचर { फर्काऊ index != other.index; }
		अचर K &चालक*() अचर { फर्काऊ container[index]; }
		अचर K *चालक->() अचर { फर्काऊ &container[index]; }
	};

	constexpr idict()
	{
	}

	पूर्णाङ्क चालक()(अचर K &साँचो)
	{
		Hasher::hash_t hash = database.do_hash(साँचो);
		पूर्णाङ्क i = database.do_lookup(साँचो, hash);
		यदि (i < 0)
			i = database.do_घुसाउ(साँचो, hash);
		फर्काऊ i + offset;
	}

	पूर्णाङ्क at(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = database.do_hash(साँचो);
		पूर्णाङ्क i = database.do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			throw std::out_of_range("idict::at()");
		फर्काऊ i + offset;
	}

	पूर्णाङ्क at(अचर K &साँचो, पूर्णाङ्क defval) अचर
	{
		Hasher::hash_t hash = database.do_hash(साँचो);
		पूर्णाङ्क i = database.do_lookup_no_rehash(साँचो, hash);
		यदि (i < 0)
			फर्काऊ defval;
		फर्काऊ i + offset;
	}

	पूर्णाङ्क गन्ती(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = database.do_hash(साँचो);
		पूर्णाङ्क i = database.do_lookup_no_rehash(साँचो, hash);
		फर्काऊ i < 0 ? 0 : 1;
	}

	पूर्णाङ्क lookup(अचर K &साँचो) अचर
	{
		Hasher::hash_t hash = database.do_hash(साँचो);
		फर्काऊ database.do_lookup_no_rehash(साँचो, hash);
	}

	शून्य expect(अचर K &साँचो, पूर्णाङ्क i)
	{
		पूर्णाङ्क j = (*this)(साँचो);
		यदि (i != j)
			throw std::out_of_range("idict::expect()");
	}

	अचर K &चालक[](पूर्णाङ्क index) अचर
	{
		फर्काऊ database.entries.at(index - offset).udata;
	}

	शून्य swap(idict &other)
	{
		database.swap(other.database);
	}

	शून्य reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार size() अचर { फर्काऊ database.size(); }
	bool empty() अचर { फर्काऊ database.empty(); }
	शून्य clear() { database.clear(); }

	const_iterator begin() अचर { फर्काऊ const_iterator(*this, offset); }
	const_iterator element(पूर्णाङ्क n) अचर { फर्काऊ const_iterator(*this, n); }
	const_iterator end() अचर { फर्काऊ const_iterator(*this, offset + size()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
ढाँचा<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED mfp
{
	idict<K, 0, OPS> database;
	class AtomicParent {
	सार्वजनिक:
		explicit AtomicParent(पूर्णाङ्क p) : parent(p) {}
		AtomicParent(अचर AtomicParent &other) : parent(other.get()) {}
		AtomicParent &चालक=(अचर AtomicParent &other) { set(other.get()); फर्काऊ *this; }
		पूर्णाङ्क get() अचर { फर्काऊ parent.load(std::memory_order_relaxed); }
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
	पूर्णाङ्क चालक()(अचर K &साँचो)
	{
		पूर्णाङ्क i = database(साँचो);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		यदि (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		फर्काऊ i;
	}

	// Finds an element at given index
	अचर K &चालक[](पूर्णाङ्क index) अचर
	{
		फर्काऊ database[index];
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
	पूर्णाङ्क ifind(पूर्णाङ्क i) अचर
	{
		पूर्णाङ्क p = i, k = i;

		जबसम्म (true) {
			पूर्णाङ्क pp = parents[p].get();
			यदि (pp < 0)
				रोक;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		जबसम्म (k != p) {
			पूर्णाङ्क next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		फर्काऊ p;
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

		जबसम्म (k != -1) {
			पूर्णाङ्क next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	पूर्णाङ्क lookup(अचर K &a)
	{
		फर्काऊ ifind((*this)(a));
	}

	अचर K &find(अचर K &a) अचर
	{
		पूर्णाङ्क i = database.at(a, -1);
		यदि (i < 0)
			फर्काऊ a;
		फर्काऊ (*this)[ifind(i)];
	}

	शून्य merge(अचर K &a, अचर K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	शून्य promote(अचर K &a)
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

	शून्य reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार size() अचर { फर्काऊ database.size(); }
	bool empty() अचर { फर्काऊ database.empty(); }
	शून्य clear() { database.clear(); parents.clear(); }

	const_iterator begin() अचर { फर्काऊ database.begin(); }
	const_iterator element(पूर्णाङ्क n) अचर { फर्काऊ database.element(n); }
	const_iterator end() अचर { फर्काऊ database.end(); }
};

} /* namespace hashlib */

#endif
