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

#जर defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#अन्यथा
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

स्थिरांक पूर्णांक hashtable_size_trigger = 2;
स्थिरांक पूर्णांक hashtable_size_factor = 3;

namespace legacy {
	ओळीत uint32_t djb2_add(uint32_t a, uint32_t b) {
		परत ((a << 5) + a) + b;
	}
};

ढाँचा<typename T>
संरचना hash_ops;

ओळीत अचिन्हित पूर्णांक mkhash_xorshift(अचिन्हित पूर्णांक a) {
	जर (आकार(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} अन्यथा जर (आकार(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} अन्यथा
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	परत a;
}

class HasherDJB32 {
सार्वजनिक:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

खाजगी:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	स्थिर uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		परत hash;
	}
सार्वजनिक:
	शून्य hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		परत;
	}
	शून्य hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		परत;
	}
	[[nodiscard]]
	hash_t yield() स्थिरांक {
		परत (hash_t)state;
	}

	ढाँचा<typename T>
	शून्य eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	ढाँचा<typename T>
	शून्य eat(स्थिरांक T& t) {
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
#define HASH_शीर्ष_LOOP_FST [[nodiscard]] स्थिर ओळीत Hasher hash
#define HASH_शीर्ष_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	परत h; \
}

ढाँचा<typename T>
संरचना hash_ops {
	स्थिर ओळीत bool cmp(स्थिरांक T &a, स्थिरांक T &b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक T &a, Hasher h) {
		जर constexpr (std::is_integral_v<T>) {
			static_assert(आकार(T) <= आकार(uint64_t));
			जर (आकार(T) == आकार(uint64_t))
				h.hash64(a);
			अन्यथा
				h.hash32(a);
			परत h;
		} अन्यथा जर constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			परत hash_ops<u_type>::hash_into((u_type) a, h);
		} अन्यथा जर constexpr (std::is_pointer_v<T>) {
			परत hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} अन्यथा जर constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			पूर्णांक size = a.size();
			पूर्णांक i = 0;
			पर्यंत (i + 8 < size) {
				uint64_t v;
				स_नकल(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			स_नकल(&v, a.data() + i, size - i);
			h.hash64(v);
			परत h;
		} अन्यथा {
			परत a.hash_into(h);
		}
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक T &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename P, typename Q> संरचना hash_ops<std::pair<P, Q>> {
	स्थिर ओळीत bool cmp(स्थिरांक std::pair<P, Q> &a, स्थिरांक std::pair<P, Q> &b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		परत h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक std::pair<P, Q> &a) HASH_शीर्ष_LOOP_SND
	[[nodiscard]] स्थिर ओळीत Hasher hash(स्थिरांक P &p, स्थिरांक Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		परत h;
	}
};

ढाँचा<typename... T> संरचना hash_ops<std::tuple<T...>> {
	स्थिर ओळीत bool cmp(स्थिरांक std::tuple<T...> &a, स्थिरांक std::tuple<T...> &b) {
		परत a == b;
	}
	ढाँचा<माप_प्रकार I = 0>
	स्थिर ओळीत typename std::enable_if<I == आकार...(T), Hasher>::type hash_into(स्थिरांक std::tuple<T...> &, Hasher h) {
		परत h;
	}
	ढाँचा<माप_प्रकार I = 0>
	स्थिर ओळीत typename std::enable_if<I != आकार...(T), Hasher>::type hash_into(स्थिरांक std::tuple<T...> &a, Hasher h) {
		प्रकारव्याख्या hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		परत h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक std::tuple<T...> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T> संरचना hash_ops<std::vector<T>> {
	स्थिर ओळीत bool cmp(स्थिरांक std::vector<T> &a, स्थिरांक std::vector<T> &b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		साठी (स्वतः k : a)
			h.eat(k);
		परत h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक std::vector<T> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T, माप_प्रकार N> संरचना hash_ops<std::array<T, N>> {
    स्थिर ओळीत bool cmp(स्थिरांक std::array<T, N> &a, स्थिरांक std::array<T, N> &b) {
        परत a == b;
    }
    [[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक std::array<T, N> &a, Hasher h) {
        साठी (स्थिरांक स्वतः& k : a)
            h = hash_ops<T>::hash_into(k, h);
        परत h;
    }
	HASH_शीर्ष_LOOP_FST (स्थिरांक std::array<T, N> &a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_cstr_ops {
	स्थिर ओळीत bool cmp(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
		परत म_भेद(a, b) == 0;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक अक्षर *a, Hasher h) {
		पर्यंत (*a)
			h.hash32(*(a++));
		परत h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक अक्षर *a) HASH_शीर्ष_LOOP_SND
};

ढाँचा <> संरचना hash_ops<अक्षर*> : hash_cstr_ops {};

संरचना hash_ptr_ops {
	स्थिर ओळीत bool cmp(स्थिरांक शून्य *a, स्थिरांक शून्य *b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक शून्य *a, Hasher h) {
		परत hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शीर्ष_LOOP_FST (स्थिरांक शून्य *a) HASH_शीर्ष_LOOP_SND
};

संरचना hash_obj_ops {
	स्थिर ओळीत bool cmp(स्थिरांक शून्य *a, स्थिरांक शून्य *b) {
		परत a == b;
	}
	ढाँचा<typename T>
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक T *a, Hasher h) {
		जर (a)
			h = a->hash_into(h);
		अन्यथा
			h.eat(0);
		परत h;
	}
	ढाँचा<typename T>
	HASH_शीर्ष_LOOP_FST (स्थिरांक T *a) HASH_शीर्ष_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
ढाँचा<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(स्थिरांक T& obj) {
	परत hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
ढाँचा<typename T>
[[nodiscard]]
[[deprecated]]
ओळीत अचिन्हित पूर्णांक mkhash(स्थिरांक T &v) {
	परत (अचिन्हित पूर्णांक) run_hash<T>(v);
}

ढाँचा<> संरचना hash_ops<std::monostate> {
	स्थिर ओळीत bool cmp(std::monostate a, std::monostate b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(std::monostate, Hasher h) {
		परत h;
	}
};

ढाँचा<typename... T> संरचना hash_ops<std::variant<T...>> {
	स्थिर ओळीत bool cmp(स्थिरांक std::variant<T...> &a, स्थिरांक std::variant<T...> &b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक std::variant<T...> &a, Hasher h) {
		std::visit([& h](स्थिरांक स्वतः &v) { h.eat(v); }, a);
		h.eat(a.index());
		परत h;
	}
};

ढाँचा<typename T> संरचना hash_ops<std::optional<T>> {
	स्थिर ओळीत bool cmp(स्थिरांक std::optional<T> &a, स्थिरांक std::optional<T> &b) {
		परत a == b;
	}
	[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक std::optional<T> &a, Hasher h) {
		जर(a.has_मूल्य())
			h.eat(*a);
		अन्यथा
			h.eat(0);
		परत h;
	}
};

ओळीत अचिन्हित पूर्णांक hashtable_size(अचिन्हित पूर्णांक min_size)
{
	// Primes as generated by https://oeis.org/A175953
	स्थिर std::vector<अचिन्हित पूर्णांक> zero_and_some_primes = {
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

	साठी (स्वतः p : zero_and_some_primes)
		जर (p >= min_size) परत p;

	जर (आकार(अचिन्हित पूर्णांक) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	साठी (स्वतः p : zero_and_some_primes)
		जर (100129 * p > min_size) परत 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

ढाँचा<typename K, typename T, typename OPS = hash_ops<K>> class dict;
ढाँचा<typename K, पूर्णांक offset = 0, typename OPS = hash_ops<K>> class idict;
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
	शून्य eat(स्थिरांक T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	शून्य eat(स्थिरांक Hasher &h) {
		Hasher::hash_t v = h.yield();
		माप_प्रकार index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
		साठी (स्वतः b : buckets)
			h.eat(b);
		परत h;
	}
खाजगी:
	std::array<Hasher::hash_t, 4> buckets;
};

ढाँचा<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	संरचना entry_t
	{
		std::pair<K, T> udata;
		पूर्णांक next;

		entry_t() { }
		entry_t(स्थिरांक std::pair<K, T> &udata, पूर्णांक next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, पूर्णांक next) : udata(std::move(udata)), next(next) { }
		bool चालक<(स्थिरांक entry_t &other) स्थिरांक { परत udata.first < other.udata.first; }
	};

	std::vector<पूर्णांक> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef न_संशोधन
	स्थिर ओळीत शून्य do_assert(bool) { }
#अन्यथा
	स्थिर ओळीत शून्य do_assert(bool cond) {
		जर (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = 0;
		जर (!hashtable.empty())
			hash = ops.hash(किल्ली).yield() % (अचिन्हित पूर्णांक)(hashtable.size());
		परत hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		साठी (पूर्णांक i = 0; i < पूर्णांक(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णांक(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णांक do_erase(पूर्णांक index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णांक(entries.size()));
		जर (hashtable.empty() || index < 0)
			परत 0;

		पूर्णांक k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णांक(entries.size()));

		जर (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			पर्यंत (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णांक(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णांक back_idx = entries.size()-1;

		जर (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < पूर्णांक(entries.size()));

			जर (k == back_idx) {
				hashtable[back_hash] = index;
			} अन्यथा {
				पर्यंत (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णांक(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.काढा_back();

		जर (entries.empty())
			hashtable.clear();

		परत 1;
	}

	पूर्णांक do_lookup(स्थिरांक K &किल्ली, Hasher::hash_t &hash)
	{
		जर (hashtable.empty())
			परत -1;

		जर (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(किल्ली);
		}

		परत do_lookup_internal(किल्ली, hash);
	}

	पूर्णांक do_lookup_internal(स्थिरांक K &किल्ली, Hasher::hash_t hash) स्थिरांक
	{
		पूर्णांक index = hashtable[hash];

		पर्यंत (index >= 0 && !ops.cmp(entries[index].udata.first, किल्ली)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णांक(entries.size()));
		}

		परत index;
	}

	पूर्णांक do_lookup_no_rehash(स्थिरांक K &किल्ली, Hasher::hash_t hash) स्थिरांक
	{
		जर (hashtable.empty())
			परत -1;

		परत do_lookup_internal(किल्ली, hash);
	}

	पूर्णांक do_घाला(स्थिरांक K &किल्ली, स्थिरांक Hasher::hash_t &hash)
	{
		जर (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(किल्ली, T()), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::pair<K, T>(किल्ली, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		परत entries.size() - 1;
	}

	पूर्णांक do_घाला(स्थिरांक std::pair<K, T> &मूल्य, स्थिरांक Hasher::hash_t &hash)
	{
		जर (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		परत entries.size() - 1;
	}

	पूर्णांक do_घाला(std::pair<K, T> &&rvalue, स्थिरांक Hasher::hash_t &hash)
	{
		जर (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		परत entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्र class dict;
	संरक्षित:
		स्थिरांक dict *ptr;
		पूर्णांक index;
		const_iterator(स्थिरांक dict *ptr, पूर्णांक index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारव्याख्या std::bidirectional_iterator_tag iterator_category;
		प्रकारव्याख्या std::pair<K, T> मूल्य_type;
		प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
		प्रकारव्याख्या स्थिरांक std::pair<K, T>* pointer;
		प्रकारव्याख्या स्थिरांक std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator चालक++() { index--; परत *this; }
		const_iterator चालक++(पूर्णांक) { const_iterator tmp = *this; index--; परत tmp; }
		const_iterator चालक--() { index++; परत *this; }
		const_iterator चालक--(पूर्णांक) { const_iterator tmp = *this; index++; परत tmp; }
		const_iterator चालक+=(पूर्णांक amt) { index -= amt; परत *this; }
		bool चालक<(स्थिरांक const_iterator &other) स्थिरांक { परत index > other.index; }
		bool चालक==(स्थिरांक const_iterator &other) स्थिरांक { परत index == other.index; }
		bool चालक!=(स्थिरांक const_iterator &other) स्थिरांक { परत index != other.index; }
		स्थिरांक std::pair<K, T> &चालक*() स्थिरांक { परत ptr->entries[index].udata; }
		स्थिरांक std::pair<K, T> *चालक->() स्थिरांक { परत &ptr->entries[index].udata; }
	};

	class iterator
	{
		मित्र class dict;
	संरक्षित:
		dict *ptr;
		पूर्णांक index;
		iterator(dict *ptr, पूर्णांक index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारव्याख्या std::forward_iterator_tag iterator_category;
		प्रकारव्याख्या std::pair<K, T> मूल्य_type;
		प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
		प्रकारव्याख्या std::pair<K, T>* pointer;
		प्रकारव्याख्या std::pair<K, T>& reference;
		iterator() { }
		iterator चालक++() { index--; परत *this; }
		iterator चालक+=(पूर्णांक amt) { index -= amt; परत *this; }
		bool चालक<(स्थिरांक iterator &other) स्थिरांक { परत index > other.index; }
		bool चालक==(स्थिरांक iterator &other) स्थिरांक { परत index == other.index; }
		bool चालक!=(स्थिरांक iterator &other) स्थिरांक { परत index != other.index; }
		std::pair<K, T> &चालक*() { परत ptr->entries[index].udata; }
		std::pair<K, T> *चालक->() { परत &ptr->entries[index].udata; }
		स्थिरांक std::pair<K, T> &चालक*() स्थिरांक { परत ptr->entries[index].udata; }
		स्थिरांक std::pair<K, T> *चालक->() स्थिरांक { परत &ptr->entries[index].udata; }
		चालक const_iterator() स्थिरांक { परत const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() स्थिरांक {
		परत std::make_reverse_iterator(end());
	}
	reverse_iterator rend() स्थिरांक {
		परत std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(स्थिरांक dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &चालक=(स्थिरांक dict &other) {
		entries = other.entries;
		do_rehash();
		परत *this;
	}

	dict &चालक=(dict &&other) noexcept {
		clear();
		swap(other);
		परत *this;
	}

	dict(स्थिरांक std::initializer_list<std::pair<K, T>> &list)
	{
		साठी (स्वतः &it : list)
			घाला(it);
	}

	ढाँचा<class आदान_Iterator>
	dict(आदान_Iterator first, आदान_Iterator last)
	{
		घाला(first, last);
	}

	ढाँचा<class आदान_Iterator>
	शून्य घाला(आदान_Iterator first, आदान_Iterator last)
	{
		साठी (; first != last; ++first)
			घाला(*first);
	}

	std::pair<iterator, bool> घाला(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(किल्ली, hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घाला(स्थिरांक std::pair<K, T> &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य.first);
		पूर्णांक i = do_lookup(मूल्य.first, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(मूल्य, hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घाला(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णांक i = do_lookup(rvalue.first, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::forward<std::pair<K, T>>(rvalue), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K स्थिरांक &किल्ली, T स्थिरांक &मूल्य)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::make_pair(किल्ली, मूल्य), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K स्थिरांक &किल्ली, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::make_pair(किल्ली, std::forward<T>(rvalue)), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T स्थिरांक &मूल्य)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णांक i = do_lookup(rkey, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::make_pair(std::forward<K>(rkey), मूल्य), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णांक i = do_lookup(rkey, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णांक erase(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक index = do_lookup(किल्ली, hash);
		परत do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		परत ++it;
	}

	पूर्णांक मोजणी(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		परत i < 0 ? 0 : 1;
	}

	पूर्णांक मोजणी(स्थिरांक K &किल्ली, const_iterator it) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		परत i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i < 0)
			परत end();
		परत iterator(this, i);
	}

	const_iterator find(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			परत end();
		परत const_iterator(this, i);
	}

	T& at(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i < 0)
			throw std::out_of_range("dict::at()");
		परत entries[i].udata.second;
	}

	स्थिरांक T& at(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			throw std::out_of_range("dict::at()");
		परत entries[i].udata.second;
	}

	स्थिरांक T& at(स्थिरांक K &किल्ली, स्थिरांक T &defval) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			परत defval;
		परत entries[i].udata.second;
	}

	T& चालक[](स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i < 0)
			i = do_घाला(std::pair<K, T>(किल्ली, T()), hash);
		परत entries[i].udata.second;
	}

	ढाँचा<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](स्थिरांक entry_t &a, स्थिरांक entry_t &b){ परत comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	शून्य swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(स्थिरांक dict &other) स्थिरांक {
		जर (size() != other.size())
			परत false;
		साठी (स्वतः &it : entries) {
			स्वतः oit = other.find(it.udata.first);
			जर (oit == other.end() || !(oit->second == it.udata.second))
				परत false;
		}
		परत true;
	}

	bool चालक!=(स्थिरांक dict &other) स्थिरांक {
		परत !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
		commutative_hash comm;
		साठी (स्वतः &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		परत comm.hash_into(h);
	}

	शून्य reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार size() स्थिरांक { परत entries.size(); }
	bool empty() स्थिरांक { परत entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { परत iterator(this, पूर्णांक(entries.size())-1); }
	iterator element(पूर्णांक n) { परत iterator(this, पूर्णांक(entries.size())-1-n); }
	iterator end() { परत iterator(nullptr, -1); }

	const_iterator begin() स्थिरांक { परत const_iterator(this, पूर्णांक(entries.size())-1); }
	const_iterator element(पूर्णांक n) स्थिरांक { परत const_iterator(this, पूर्णांक(entries.size())-1-n); }
	const_iterator end() स्थिरांक { परत const_iterator(this, -1); }
};

ढाँचा<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	ढाँचा<typename, पूर्णांक, typename> मित्र class idict;

संरक्षित:
	संरचना entry_t
	{
		K udata;
		पूर्णांक next;

		entry_t() { }
		entry_t(स्थिरांक K &udata, पूर्णांक next) : udata(udata), next(next) { }
		entry_t(K &&udata, पूर्णांक next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<पूर्णांक> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef न_संशोधन
	स्थिर ओळीत शून्य do_assert(bool) { }
#अन्यथा
	स्थिर ओळीत शून्य do_assert(bool cond) {
		जर (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = 0;
		जर (!hashtable.empty())
			hash = ops.hash(किल्ली).yield() % (अचिन्हित पूर्णांक)(hashtable.size());
		परत hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		साठी (पूर्णांक i = 0; i < पूर्णांक(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < पूर्णांक(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णांक do_erase(पूर्णांक index, Hasher::hash_t hash)
	{
		do_assert(index < पूर्णांक(entries.size()));
		जर (hashtable.empty() || index < 0)
			परत 0;

		पूर्णांक k = hashtable[hash];
		जर (k == index) {
			hashtable[hash] = entries[index].next;
		} अन्यथा {
			पर्यंत (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < पूर्णांक(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		पूर्णांक back_idx = entries.size()-1;

		जर (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			जर (k == back_idx) {
				hashtable[back_hash] = index;
			} अन्यथा {
				पर्यंत (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < पूर्णांक(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.काढा_back();

		जर (entries.empty())
			hashtable.clear();

		परत 1;
	}

	पूर्णांक do_lookup(स्थिरांक K &किल्ली, Hasher::hash_t &hash)
	{
		जर (hashtable.empty())
			परत -1;

		जर (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(किल्ली);
		}

		परत do_lookup_internal(किल्ली, hash);
	}

	पूर्णांक do_lookup_internal(स्थिरांक K &किल्ली, Hasher::hash_t hash) स्थिरांक
	{
		पूर्णांक index = hashtable[hash];

		पर्यंत (index >= 0 && !ops.cmp(entries[index].udata, किल्ली)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < पूर्णांक(entries.size()));
		}

		परत index;
	}

	पूर्णांक do_lookup_no_rehash(स्थिरांक K &किल्ली, Hasher::hash_t hash) स्थिरांक
	{
		जर (hashtable.empty())
			परत -1;

		परत do_lookup_internal(किल्ली, hash);
	}

	पूर्णांक do_घाला(स्थिरांक K &मूल्य, Hasher::hash_t &hash)
	{
		जर (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
			hash = do_hash(मूल्य);
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		परत entries.size() - 1;
	}

	पूर्णांक do_घाला(K &&rvalue, Hasher::hash_t &hash)
	{
		जर (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} अन्यथा {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		परत entries.size() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्र class pool;
	संरक्षित:
		स्थिरांक pool *ptr;
		पूर्णांक index;
		const_iterator(स्थिरांक pool *ptr, पूर्णांक index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारव्याख्या std::forward_iterator_tag iterator_category;
		प्रकारव्याख्या K मूल्य_type;
		प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
		प्रकारव्याख्या K* pointer;
		प्रकारव्याख्या K& reference;
		const_iterator() { }
		const_iterator चालक++() { index--; परत *this; }
		bool चालक==(स्थिरांक const_iterator &other) स्थिरांक { परत index == other.index; }
		bool चालक!=(स्थिरांक const_iterator &other) स्थिरांक { परत index != other.index; }
		स्थिरांक K &चालक*() स्थिरांक { परत ptr->entries[index].udata; }
		स्थिरांक K *चालक->() स्थिरांक { परत &ptr->entries[index].udata; }
	};

	class iterator
	{
		मित्र class pool;
	संरक्षित:
		pool *ptr;
		पूर्णांक index;
		iterator(pool *ptr, पूर्णांक index) : ptr(ptr), index(index) { }
	सार्वजनिक:
		प्रकारव्याख्या std::forward_iterator_tag iterator_category;
		प्रकारव्याख्या K मूल्य_type;
		प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
		प्रकारव्याख्या K* pointer;
		प्रकारव्याख्या K& reference;
		iterator() { }
		iterator चालक++() { index--; परत *this; }
		bool चालक==(स्थिरांक iterator &other) स्थिरांक { परत index == other.index; }
		bool चालक!=(स्थिरांक iterator &other) स्थिरांक { परत index != other.index; }
		K &चालक*() { परत ptr->entries[index].udata; }
		K *चालक->() { परत &ptr->entries[index].udata; }
		स्थिरांक K &चालक*() स्थिरांक { परत ptr->entries[index].udata; }
		स्थिरांक K *चालक->() स्थिरांक { परत &ptr->entries[index].udata; }
		चालक const_iterator() स्थिरांक { परत const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(स्थिरांक pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &चालक=(स्थिरांक pool &other) {
		entries = other.entries;
		do_rehash();
		परत *this;
	}

	pool &चालक=(pool &&other) noexcept {
		clear();
		swap(other);
		परत *this;
	}

	pool(स्थिरांक std::initializer_list<K> &list)
	{
		साठी (स्वतः &it : list)
			घाला(it);
	}

	ढाँचा<class आदान_Iterator>
	pool(आदान_Iterator first, आदान_Iterator last)
	{
		घाला(first, last);
	}

	ढाँचा<class आदान_Iterator>
	शून्य घाला(आदान_Iterator first, आदान_Iterator last)
	{
		साठी (; first != last; ++first)
			घाला(*first);
	}

	std::pair<iterator, bool> घाला(स्थिरांक K &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य);
		पूर्णांक i = do_lookup(मूल्य, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(मूल्य, hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> घाला(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णांक i = do_lookup(rvalue, hash);
		जर (i >= 0)
			परत std::pair<iterator, bool>(iterator(this, i), false);
		i = do_घाला(std::forward<K>(rvalue), hash);
		परत std::pair<iterator, bool>(iterator(this, i), true);
	}

	ढाँचा<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		परत घाला(K(std::forward<Args>(args)...));
	}

	पूर्णांक erase(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक index = do_lookup(किल्ली, hash);
		परत do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		परत ++it;
	}

	पूर्णांक मोजणी(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		परत i < 0 ? 0 : 1;
	}

	पूर्णांक मोजणी(स्थिरांक K &किल्ली, const_iterator it) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		परत i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		जर (i < 0)
			परत end();
		परत iterator(this, i);
	}

	const_iterator find(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			परत end();
		परत const_iterator(this, i);
	}

	bool चालक[](स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = do_hash(किल्ली);
		पूर्णांक i = do_lookup(किल्ली, hash);
		परत i >= 0;
	}

	ढाँचा<typename Compare = std::less<K>>
	शून्य sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](स्थिरांक entry_t &a, स्थिरांक entry_t &b){ परत comp(b.udata, a.udata); });
		do_rehash();
	}

	K काढा()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		परत ret;
	}

	शून्य swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(स्थिरांक pool &other) स्थिरांक {
		जर (size() != other.size())
			परत false;
		साठी (स्वतः &it : entries)
			जर (!other.मोजणी(it.udata))
				परत false;
		परत true;
	}

	bool चालक!=(स्थिरांक pool &other) स्थिरांक {
		परत !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
		commutative_hash comm;
		साठी (स्वतः &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		परत comm.hash_into(h);
	}

	शून्य reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार size() स्थिरांक { परत entries.size(); }
	bool empty() स्थिरांक { परत entries.empty(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { परत iterator(this, पूर्णांक(entries.size())-1); }
	iterator element(पूर्णांक n) { परत iterator(this, पूर्णांक(entries.size())-1-n); }
	iterator end() { परत iterator(nullptr, -1); }

	const_iterator begin() स्थिरांक { परत const_iterator(this, पूर्णांक(entries.size())-1); }
	const_iterator element(पूर्णांक n) स्थिरांक { परत const_iterator(this, पूर्णांक(entries.size())-1-n); }
	const_iterator end() स्थिरांक { परत const_iterator(nullptr, -1); }
};

ढाँचा<typename K, पूर्णांक offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

सार्वजनिक:
	class const_iterator
	{
		मित्र class idict;
	संरक्षित:
		स्थिरांक idict &container;
		पूर्णांक index;
		const_iterator(स्थिरांक idict &container, पूर्णांक index) : container(container), index(index) { }
	सार्वजनिक:
		प्रकारव्याख्या std::forward_iterator_tag iterator_category;
		प्रकारव्याख्या K मूल्य_type;
		प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
		प्रकारव्याख्या K* pointer;
		प्रकारव्याख्या K& reference;
		const_iterator() { }
		const_iterator चालक++() { index++; परत *this; }
		bool चालक==(स्थिरांक const_iterator &other) स्थिरांक { परत index == other.index; }
		bool चालक!=(स्थिरांक const_iterator &other) स्थिरांक { परत index != other.index; }
		स्थिरांक K &चालक*() स्थिरांक { परत container[index]; }
		स्थिरांक K *चालक->() स्थिरांक { परत &container[index]; }
	};

	constexpr idict()
	{
	}

	पूर्णांक चालक()(स्थिरांक K &किल्ली)
	{
		Hasher::hash_t hash = database.do_hash(किल्ली);
		पूर्णांक i = database.do_lookup(किल्ली, hash);
		जर (i < 0)
			i = database.do_घाला(किल्ली, hash);
		परत i + offset;
	}

	पूर्णांक at(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = database.do_hash(किल्ली);
		पूर्णांक i = database.do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			throw std::out_of_range("idict::at()");
		परत i + offset;
	}

	पूर्णांक at(स्थिरांक K &किल्ली, पूर्णांक defval) स्थिरांक
	{
		Hasher::hash_t hash = database.do_hash(किल्ली);
		पूर्णांक i = database.do_lookup_no_rehash(किल्ली, hash);
		जर (i < 0)
			परत defval;
		परत i + offset;
	}

	पूर्णांक मोजणी(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = database.do_hash(किल्ली);
		पूर्णांक i = database.do_lookup_no_rehash(किल्ली, hash);
		परत i < 0 ? 0 : 1;
	}

	पूर्णांक lookup(स्थिरांक K &किल्ली) स्थिरांक
	{
		Hasher::hash_t hash = database.do_hash(किल्ली);
		परत database.do_lookup_no_rehash(किल्ली, hash);
	}

	शून्य expect(स्थिरांक K &किल्ली, पूर्णांक i)
	{
		पूर्णांक j = (*this)(किल्ली);
		जर (i != j)
			throw std::out_of_range("idict::expect()");
	}

	स्थिरांक K &चालक[](पूर्णांक index) स्थिरांक
	{
		परत database.entries.at(index - offset).udata;
	}

	शून्य swap(idict &other)
	{
		database.swap(other.database);
	}

	शून्य reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार size() स्थिरांक { परत database.size(); }
	bool empty() स्थिरांक { परत database.empty(); }
	शून्य clear() { database.clear(); }

	const_iterator begin() स्थिरांक { परत const_iterator(*this, offset); }
	const_iterator element(पूर्णांक n) स्थिरांक { परत const_iterator(*this, n); }
	const_iterator end() स्थिरांक { परत const_iterator(*this, offset + size()); }
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
		explicit AtomicParent(पूर्णांक p) : parent(p) {}
		AtomicParent(स्थिरांक AtomicParent &other) : parent(other.get()) {}
		AtomicParent &चालक=(स्थिरांक AtomicParent &other) { set(other.get()); परत *this; }
		पूर्णांक get() स्थिरांक { परत parent.load(std::memory_order_relaxed); }
		शून्य set(पूर्णांक p) { parent.store(p, std::memory_order_relaxed); }
	खाजगी:
		std::atomic<पूर्णांक> parent;
	};
	std::vector<AtomicParent> parents;

सार्वजनिक:
	प्रकारव्याख्या typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	पूर्णांक चालक()(स्थिरांक K &किल्ली)
	{
		पूर्णांक i = database(किल्ली);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		जर (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		परत i;
	}

	// Finds an element at given index
	स्थिरांक K &चालक[](पूर्णांक index) स्थिरांक
	{
		परत database[index];
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
	पूर्णांक ifind(पूर्णांक i) स्थिरांक
	{
		पूर्णांक p = i, k = i;

		पर्यंत (true) {
			पूर्णांक pp = parents[p].get();
			जर (pp < 0)
				थांबा;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		पर्यंत (k != p) {
			पूर्णांक next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		परत p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	शून्य imerge(पूर्णांक i, पूर्णांक j)
	{
		i = ifind(i);
		j = ifind(j);

		जर (i != j)
			parents[i].set(j);
	}

	शून्य ipromote(पूर्णांक i)
	{
		पूर्णांक k = i;

		पर्यंत (k != -1) {
			पूर्णांक next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	पूर्णांक lookup(स्थिरांक K &a)
	{
		परत ifind((*this)(a));
	}

	स्थिरांक K &find(स्थिरांक K &a) स्थिरांक
	{
		पूर्णांक i = database.at(a, -1);
		जर (i < 0)
			परत a;
		परत (*this)[ifind(i)];
	}

	शून्य merge(स्थिरांक K &a, स्थिरांक K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	शून्य promote(स्थिरांक K &a)
	{
		पूर्णांक i = database.at(a, -1);
		जर (i >= 0)
			ipromote(i);
	}

	शून्य swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	शून्य reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार size() स्थिरांक { परत database.size(); }
	bool empty() स्थिरांक { परत database.empty(); }
	शून्य clear() { database.clear(); parents.clear(); }

	const_iterator begin() स्थिरांक { परत database.begin(); }
	const_iterator element(पूर्णांक n) स्थिरांक { परत database.element(n); }
	const_iterator end() स्थिरांक { परत database.end(); }
};

} /* namespace hashlib */

#endif
