// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#अगर_अघोषित HASHLIB_H
#घोषणा HASHLIB_H

#समावेश <array>
#समावेश <atomic>
#समावेश <stdexcept>
#समावेश <algorithm>
#समावेश <optional>
#समावेश <string>
#समावेश <variant>
#समावेश <vector>
#समावेश <type_traits>
#समावेश <stdint.h>

#घोषणा YS_HASHING_VERSION 1

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

#अगर defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#अन्यथा
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#पूर्ण_अगर

स्थिर पूर्णांक hashtable_आकार_trigger = 2;
स्थिर पूर्णांक hashtable_आकार_factor = 3;

namespace legacy {
	अंतरभूत uint32_t djb2_add(uint32_t a, uint32_t b) {
		वापस ((a << 5) + a) + b;
	}
};

ढाँचा<typename T>
काष्ठा hash_ops;

अंतरभूत अचिन्हित पूर्णांक mkhash_xorshift(अचिन्हित पूर्णांक a) {
	अगर (माप(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} अन्यथा अगर (माप(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} अन्यथा
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	वापस a;
}

class HasherDJB32 {
खुला:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

निजी:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	अटल uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		वापस hash;
	}
खुला:
	व्योम hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		वापस;
	}
	व्योम hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		वापस;
	}
	[[nodiscard]]
	hash_t yield() स्थिर {
		वापस (hash_t)state;
	}

	ढाँचा<typename T>
	व्योम eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	ढाँचा<typename T>
	व्योम eat(स्थिर T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	व्योम commutative_eat(hash_t t) {
		state ^= t;
	}

	व्योम force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#घोषणा HASH_शीर्ष_LOOP_FST [[nodiscard]] अटल अंतरभूत Hasher hash
#घोषणा HASH_शीर्ष_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	वापस h; \
}

ढाँचा<typename T>
काष्ठा hash_ops {
	अटल अंतरभूत bool cmp(स्थिर T &a, स्थिर T &b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर T &a, Hasher h) {
		अगर constexpr (std::is_integral_v<T>) {
			static_assert(माप(T) <= माप(uint64_t));
			अगर (माप(T) == माप(uint64_t))
				h.hash64(a);
			अन्यथा
				h.hash32(a);
			वापस h;
		} अन्यथा अगर constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			वापस hash_ops<u_type>::hash_into((u_type) a, h);
		} अन्यथा अगर constexpr (std::is_pointer_v<T>) {
			वापस hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} अन्यथा अगर constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			पूर्णांक आकार = a.आकार();
			पूर्णांक i = 0;
			जबतक (i + 8 < आकार) {
				uint64_t v;
				स_नकल(&v, a.तथ्य() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			स_नकल(&v, a.तथ्य() + i, आकार - i);
			h.hash64(v);
			वापस h;
		} अन्यथा {
			वापस a.hash_into(h);
		}
	}
	HASH_शीर्ष_LOOP_FST (स्थिर T &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename P, typename Q> काष्ठा hash_ops<std::pair<P, Q>> {
	अटल अंतरभूत bool cmp(स्थिर std::pair<P, Q> &a, स्थिर std::pair<P, Q> &b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		वापस h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिर std::pair<P, Q> &a) HASH_शीर्ष_LOOP_SND
	[[nodiscard]] अटल अंतरभूत Hasher hash(स्थिर P &p, स्थिर Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		वापस h;
	}
};

ढाँचा<typename... T> काष्ठा hash_ops<std::tuple<T...>> {
	अटल अंतरभूत bool cmp(स्थिर std::tuple<T...> &a, स्थिर std::tuple<T...> &b) {
		वापस a == b;
	}
	ढाँचा<माप_प्रकार I = 0>
	अटल अंतरभूत typename std::enable_if<I == माप...(T), Hasher>::type hash_into(स्थिर std::tuple<T...> &, Hasher h) {
		वापस h;
	}
	ढाँचा<माप_प्रकार I = 0>
	अटल अंतरभूत typename std::enable_if<I != माप...(T), Hasher>::type hash_into(स्थिर std::tuple<T...> &a, Hasher h) {
		प्रकार hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		वापस h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिर std::tuple<T...> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T> काष्ठा hash_ops<std::vector<T>> {
	अटल अंतरभूत bool cmp(स्थिर std::vector<T> &a, स्थिर std::vector<T> &b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.आकार());
		क्रम (स्वतः k : a)
			h.eat(k);
		वापस h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिर std::vector<T> &a) HASH_शीर्ष_LOOP_SND
};

ढाँचा<typename T, माप_प्रकार N> काष्ठा hash_ops<std::array<T, N>> {
    अटल अंतरभूत bool cmp(स्थिर std::array<T, N> &a, स्थिर std::array<T, N> &b) {
        वापस a == b;
    }
    [[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर std::array<T, N> &a, Hasher h) {
        क्रम (स्थिर स्वतः& k : a)
            h = hash_ops<T>::hash_into(k, h);
        वापस h;
    }
	HASH_शीर्ष_LOOP_FST (स्थिर std::array<T, N> &a) HASH_शीर्ष_LOOP_SND
};

काष्ठा hash_cstr_ops {
	अटल अंतरभूत bool cmp(स्थिर अक्षर *a, स्थिर अक्षर *b) {
		वापस म_भेद(a, b) == 0;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर अक्षर *a, Hasher h) {
		जबतक (*a)
			h.hash32(*(a++));
		वापस h;
	}
	HASH_शीर्ष_LOOP_FST (स्थिर अक्षर *a) HASH_शीर्ष_LOOP_SND
};

ढाँचा <> काष्ठा hash_ops<अक्षर*> : hash_cstr_ops {};

काष्ठा hash_ptr_ops {
	अटल अंतरभूत bool cmp(स्थिर व्योम *a, स्थिर व्योम *b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर व्योम *a, Hasher h) {
		वापस hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शीर्ष_LOOP_FST (स्थिर व्योम *a) HASH_शीर्ष_LOOP_SND
};

काष्ठा hash_obj_ops {
	अटल अंतरभूत bool cmp(स्थिर व्योम *a, स्थिर व्योम *b) {
		वापस a == b;
	}
	ढाँचा<typename T>
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर T *a, Hasher h) {
		अगर (a)
			h = a->hash_into(h);
		अन्यथा
			h.eat(0);
		वापस h;
	}
	ढाँचा<typename T>
	HASH_शीर्ष_LOOP_FST (स्थिर T *a) HASH_शीर्ष_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
ढाँचा<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(स्थिर T& obj) {
	वापस hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
ढाँचा<typename T>
[[nodiscard]]
[[deprecated]]
अंतरभूत अचिन्हित पूर्णांक mkhash(स्थिर T &v) {
	वापस (अचिन्हित पूर्णांक) run_hash<T>(v);
}

ढाँचा<> काष्ठा hash_ops<std::monostate> {
	अटल अंतरभूत bool cmp(std::monostate a, std::monostate b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(std::monostate, Hasher h) {
		वापस h;
	}
};

ढाँचा<typename... T> काष्ठा hash_ops<std::variant<T...>> {
	अटल अंतरभूत bool cmp(स्थिर std::variant<T...> &a, स्थिर std::variant<T...> &b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर std::variant<T...> &a, Hasher h) {
		std::visit([& h](स्थिर स्वतः &v) { h.eat(v); }, a);
		h.eat(a.सूचकांक());
		वापस h;
	}
};

ढाँचा<typename T> काष्ठा hash_ops<std::optional<T>> {
	अटल अंतरभूत bool cmp(स्थिर std::optional<T> &a, स्थिर std::optional<T> &b) {
		वापस a == b;
	}
	[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर std::optional<T> &a, Hasher h) {
		अगर(a.has_मूल्य())
			h.eat(*a);
		अन्यथा
			h.eat(0);
		वापस h;
	}
};

अंतरभूत अचिन्हित पूर्णांक hashtable_आकार(अचिन्हित पूर्णांक min_आकार)
{
	// Primes as generated by https://oeis.org/A175953
	अटल std::vector<अचिन्हित पूर्णांक> zero_and_some_primes = {
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

	क्रम (स्वतः p : zero_and_some_primes)
		अगर (p >= min_आकार) वापस p;

	अगर (माप(अचिन्हित पूर्णांक) == 4)
		throw std::लंबाई_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	क्रम (स्वतः p : zero_and_some_primes)
		अगर (100129 * p > min_आकार) वापस 100129 * p;

	throw std::लंबाई_error("hash table exceeded maximum size.");
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
खुला:
	commutative_hash() {
		buckets.fill(0);
	}
	ढाँचा <typename T>
	व्योम eat(स्थिर T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	व्योम eat(स्थिर Hasher &h) {
		Hasher::hash_t v = h.yield();
		माप_प्रकार सूचकांक = v & (buckets.आकार() - 1);
		buckets[सूचकांक] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		क्रम (स्वतः b : buckets)
			h.eat(b);
		वापस h;
	}
निजी:
	std::array<Hasher::hash_t, 4> buckets;
};

ढाँचा<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	काष्ठा entry_t
	{
		std::pair<K, T> udata;
		पूर्णांक अग्र;

		entry_t() { }
		entry_t(स्थिर std::pair<K, T> &udata, पूर्णांक अग्र) : udata(udata), अग्र(अग्र) { }
		entry_t(std::pair<K, T> &&udata, पूर्णांक अग्र) : udata(std::move(udata)), अग्र(अग्र) { }
		bool चालक<(स्थिर entry_t &other) स्थिर { वापस udata.first < other.udata.first; }
	};

	std::vector<पूर्णांक> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#अगर_घोषित न_संशोधन
	अटल अंतरभूत व्योम do_assert(bool) { }
#अन्यथा
	अटल अंतरभूत व्योम do_assert(bool cond) {
		अगर (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#पूर्ण_अगर

	Hasher::hash_t do_hash(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = 0;
		अगर (!hashtable.empty())
			hash = ops.hash(कुंजी).yield() % (अचिन्हित पूर्णांक)(hashtable.आकार());
		वापस hash;
	}

	व्योम do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_आकार(entries.capacity() * hashtable_आकार_factor), -1);

		क्रम (पूर्णांक i = 0; i < पूर्णांक(entries.आकार()); i++) {
			do_assert(-1 <= entries[i].अग्र && entries[i].अग्र < पूर्णांक(entries.आकार()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].अग्र = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णांक do_erase(पूर्णांक सूचकांक, Hasher::hash_t hash)
	{
		do_assert(सूचकांक < पूर्णांक(entries.आकार()));
		अगर (hashtable.empty() || सूचकांक < 0)
			वापस 0;

		पूर्णांक k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णांक(entries.आकार()));

		अगर (k == सूचकांक) {
			hashtable[hash] = entries[सूचकांक].अग्र;
		} अन्यथा {
			जबतक (entries[k].अग्र != सूचकांक) {
				k = entries[k].अग्र;
				do_assert(0 <= k && k < पूर्णांक(entries.आकार()));
			}
			entries[k].अग्र = entries[सूचकांक].अग्र;
		}

		पूर्णांक back_idx = entries.आकार()-1;

		अगर (सूचकांक != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < पूर्णांक(entries.आकार()));

			अगर (k == back_idx) {
				hashtable[back_hash] = सूचकांक;
			} अन्यथा {
				जबतक (entries[k].अग्र != back_idx) {
					k = entries[k].अग्र;
					do_assert(0 <= k && k < पूर्णांक(entries.आकार()));
				}
				entries[k].अग्र = सूचकांक;
			}

			entries[सूचकांक] = std::move(entries[back_idx]);
		}

		entries.निकालो_back();

		अगर (entries.empty())
			hashtable.clear();

		वापस 1;
	}

	पूर्णांक do_lookup(स्थिर K &कुंजी, Hasher::hash_t &hash)
	{
		अगर (hashtable.empty())
			वापस -1;

		अगर (entries.आकार() * hashtable_आकार_trigger > hashtable.आकार()) {
			do_rehash();
			hash = do_hash(कुंजी);
		}

		वापस do_lookup_internal(कुंजी, hash);
	}

	पूर्णांक do_lookup_internal(स्थिर K &कुंजी, Hasher::hash_t hash) स्थिर
	{
		पूर्णांक सूचकांक = hashtable[hash];

		जबतक (सूचकांक >= 0 && !ops.cmp(entries[सूचकांक].udata.first, कुंजी)) {
			सूचकांक = entries[सूचकांक].अग्र;
			do_assert(-1 <= सूचकांक && सूचकांक < पूर्णांक(entries.आकार()));
		}

		वापस सूचकांक;
	}

	पूर्णांक do_lookup_no_rehash(स्थिर K &कुंजी, Hasher::hash_t hash) स्थिर
	{
		अगर (hashtable.empty())
			वापस -1;

		वापस do_lookup_internal(कुंजी, hash);
	}

	पूर्णांक do_जोड़ो(स्थिर K &कुंजी, स्थिर Hasher::hash_t &hash)
	{
		अगर (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(कुंजी, T()), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::pair<K, T>(कुंजी, T()), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		वापस entries.आकार() - 1;
	}

	पूर्णांक do_जोड़ो(स्थिर std::pair<K, T> &मूल्य, स्थिर Hasher::hash_t &hash)
	{
		अगर (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		वापस entries.आकार() - 1;
	}

	पूर्णांक do_जोड़ो(std::pair<K, T> &&rvalue, स्थिर Hasher::hash_t &hash)
	{
		अगर (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		वापस entries.आकार() - 1;
	}

खुला:
	class const_iterator
	{
		मित्र class dict;
	रक्षित:
		स्थिर dict *ptr;
		पूर्णांक सूचकांक;
		const_iterator(स्थिर dict *ptr, पूर्णांक सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	खुला:
		प्रकार std::bidirectional_iterator_tag iterator_category;
		प्रकार std::pair<K, T> मूल्य_type;
		प्रकार सूचक_भेद_प्रकार difference_type;
		प्रकार स्थिर std::pair<K, T>* pointer;
		प्रकार स्थिर std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator चालक++() { सूचकांक--; वापस *this; }
		const_iterator चालक++(पूर्णांक) { const_iterator tmp = *this; सूचकांक--; वापस tmp; }
		const_iterator चालक--() { सूचकांक++; वापस *this; }
		const_iterator चालक--(पूर्णांक) { const_iterator tmp = *this; सूचकांक++; वापस tmp; }
		const_iterator चालक+=(पूर्णांक amt) { सूचकांक -= amt; वापस *this; }
		bool चालक<(स्थिर const_iterator &other) स्थिर { वापस सूचकांक > other.सूचकांक; }
		bool चालक==(स्थिर const_iterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
		bool चालक!=(स्थिर const_iterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
		स्थिर std::pair<K, T> &चालक*() स्थिर { वापस ptr->entries[सूचकांक].udata; }
		स्थिर std::pair<K, T> *चालक->() स्थिर { वापस &ptr->entries[सूचकांक].udata; }
	};

	class iterator
	{
		मित्र class dict;
	रक्षित:
		dict *ptr;
		पूर्णांक सूचकांक;
		iterator(dict *ptr, पूर्णांक सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	खुला:
		प्रकार std::forward_iterator_tag iterator_category;
		प्रकार std::pair<K, T> मूल्य_type;
		प्रकार सूचक_भेद_प्रकार difference_type;
		प्रकार std::pair<K, T>* pointer;
		प्रकार std::pair<K, T>& reference;
		iterator() { }
		iterator चालक++() { सूचकांक--; वापस *this; }
		iterator चालक+=(पूर्णांक amt) { सूचकांक -= amt; वापस *this; }
		bool चालक<(स्थिर iterator &other) स्थिर { वापस सूचकांक > other.सूचकांक; }
		bool चालक==(स्थिर iterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
		bool चालक!=(स्थिर iterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
		std::pair<K, T> &चालक*() { वापस ptr->entries[सूचकांक].udata; }
		std::pair<K, T> *चालक->() { वापस &ptr->entries[सूचकांक].udata; }
		स्थिर std::pair<K, T> &चालक*() स्थिर { वापस ptr->entries[सूचकांक].udata; }
		स्थिर std::pair<K, T> *चालक->() स्थिर { वापस &ptr->entries[सूचकांक].udata; }
		चालक const_iterator() स्थिर { वापस const_iterator(ptr, सूचकांक); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() स्थिर {
		वापस std::make_reverse_iterator(end());
	}
	reverse_iterator rend() स्थिर {
		वापस std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(स्थिर dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &चालक=(स्थिर dict &other) {
		entries = other.entries;
		do_rehash();
		वापस *this;
	}

	dict &चालक=(dict &&other) noexcept {
		clear();
		swap(other);
		वापस *this;
	}

	dict(स्थिर std::initializer_list<std::pair<K, T>> &list)
	{
		क्रम (स्वतः &it : list)
			जोड़ो(it);
	}

	ढाँचा<class निवेश_Iterator>
	dict(निवेश_Iterator first, निवेश_Iterator last)
	{
		जोड़ो(first, last);
	}

	ढाँचा<class निवेश_Iterator>
	व्योम जोड़ो(निवेश_Iterator first, निवेश_Iterator last)
	{
		क्रम (; first != last; ++first)
			जोड़ो(*first);
	}

	std::pair<iterator, bool> जोड़ो(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(कुंजी, hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(स्थिर std::pair<K, T> &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य.first);
		पूर्णांक i = do_lookup(मूल्य.first, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(मूल्य, hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णांक i = do_lookup(rvalue.first, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::forward<std::pair<K, T>>(rvalue), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K स्थिर &कुंजी, T स्थिर &मूल्य)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(कुंजी, मूल्य), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K स्थिर &कुंजी, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(कुंजी, std::forward<T>(rvalue)), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T स्थिर &मूल्य)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णांक i = do_lookup(rkey, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(std::forward<K>(rkey), मूल्य), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णांक i = do_lookup(rkey, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णांक erase(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक सूचकांक = do_lookup(कुंजी, hash);
		वापस do_erase(सूचकांक, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.सूचकांक, hash);
		वापस ++it;
	}

	पूर्णांक गिनती(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		वापस i < 0 ? 0 : 1;
	}

	पूर्णांक गिनती(स्थिर K &कुंजी, const_iterator it) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		वापस i < 0 || i > it.सूचकांक ? 0 : 1;
	}

	iterator find(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i < 0)
			वापस end();
		वापस iterator(this, i);
	}

	const_iterator find(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			वापस end();
		वापस const_iterator(this, i);
	}

	T& at(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i < 0)
			throw std::out_of_range("dict::at()");
		वापस entries[i].udata.second;
	}

	स्थिर T& at(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			throw std::out_of_range("dict::at()");
		वापस entries[i].udata.second;
	}

	स्थिर T& at(स्थिर K &कुंजी, स्थिर T &defval) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			वापस defval;
		वापस entries[i].udata.second;
	}

	T& चालक[](स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i < 0)
			i = do_जोड़ो(std::pair<K, T>(कुंजी, T()), hash);
		वापस entries[i].udata.second;
	}

	ढाँचा<typename Compare = std::less<K>>
	व्योम sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](स्थिर entry_t &a, स्थिर entry_t &b){ वापस comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	व्योम swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(स्थिर dict &other) स्थिर {
		अगर (आकार() != other.आकार())
			वापस false;
		क्रम (स्वतः &it : entries) {
			स्वतः oit = other.find(it.udata.first);
			अगर (oit == other.end() || !(oit->second == it.udata.second))
				वापस false;
		}
		वापस true;
	}

	bool चालक!=(स्थिर dict &other) स्थिर {
		वापस !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		commutative_hash comm;
		क्रम (स्वतः &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		वापस comm.hash_into(h);
	}

	व्योम reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार आकार() स्थिर { वापस entries.आकार(); }
	bool empty() स्थिर { वापस entries.empty(); }
	व्योम clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { वापस iterator(this, पूर्णांक(entries.आकार())-1); }
	iterator element(पूर्णांक n) { वापस iterator(this, पूर्णांक(entries.आकार())-1-n); }
	iterator end() { वापस iterator(nullptr, -1); }

	const_iterator begin() स्थिर { वापस const_iterator(this, पूर्णांक(entries.आकार())-1); }
	const_iterator element(पूर्णांक n) स्थिर { वापस const_iterator(this, पूर्णांक(entries.आकार())-1-n); }
	const_iterator end() स्थिर { वापस const_iterator(this, -1); }
};

ढाँचा<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	ढाँचा<typename, पूर्णांक, typename> मित्र class idict;

रक्षित:
	काष्ठा entry_t
	{
		K udata;
		पूर्णांक अग्र;

		entry_t() { }
		entry_t(स्थिर K &udata, पूर्णांक अग्र) : udata(udata), अग्र(अग्र) { }
		entry_t(K &&udata, पूर्णांक अग्र) : udata(std::move(udata)), अग्र(अग्र) { }
	};

	std::vector<पूर्णांक> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#अगर_घोषित न_संशोधन
	अटल अंतरभूत व्योम do_assert(bool) { }
#अन्यथा
	अटल अंतरभूत व्योम do_assert(bool cond) {
		अगर (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#पूर्ण_अगर

	Hasher::hash_t do_hash(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = 0;
		अगर (!hashtable.empty())
			hash = ops.hash(कुंजी).yield() % (अचिन्हित पूर्णांक)(hashtable.आकार());
		वापस hash;
	}

	व्योम do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_आकार(entries.capacity() * hashtable_आकार_factor), -1);

		क्रम (पूर्णांक i = 0; i < पूर्णांक(entries.आकार()); i++) {
			do_assert(-1 <= entries[i].अग्र && entries[i].अग्र < पूर्णांक(entries.आकार()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].अग्र = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णांक do_erase(पूर्णांक सूचकांक, Hasher::hash_t hash)
	{
		do_assert(सूचकांक < पूर्णांक(entries.आकार()));
		अगर (hashtable.empty() || सूचकांक < 0)
			वापस 0;

		पूर्णांक k = hashtable[hash];
		अगर (k == सूचकांक) {
			hashtable[hash] = entries[सूचकांक].अग्र;
		} अन्यथा {
			जबतक (entries[k].अग्र != सूचकांक) {
				k = entries[k].अग्र;
				do_assert(0 <= k && k < पूर्णांक(entries.आकार()));
			}
			entries[k].अग्र = entries[सूचकांक].अग्र;
		}

		पूर्णांक back_idx = entries.आकार()-1;

		अगर (सूचकांक != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			अगर (k == back_idx) {
				hashtable[back_hash] = सूचकांक;
			} अन्यथा {
				जबतक (entries[k].अग्र != back_idx) {
					k = entries[k].अग्र;
					do_assert(0 <= k && k < पूर्णांक(entries.आकार()));
				}
				entries[k].अग्र = सूचकांक;
			}

			entries[सूचकांक] = std::move(entries[back_idx]);
		}

		entries.निकालो_back();

		अगर (entries.empty())
			hashtable.clear();

		वापस 1;
	}

	पूर्णांक do_lookup(स्थिर K &कुंजी, Hasher::hash_t &hash)
	{
		अगर (hashtable.empty())
			वापस -1;

		अगर (entries.आकार() * hashtable_आकार_trigger > hashtable.आकार()) {
			do_rehash();
			hash = do_hash(कुंजी);
		}

		वापस do_lookup_internal(कुंजी, hash);
	}

	पूर्णांक do_lookup_internal(स्थिर K &कुंजी, Hasher::hash_t hash) स्थिर
	{
		पूर्णांक सूचकांक = hashtable[hash];

		जबतक (सूचकांक >= 0 && !ops.cmp(entries[सूचकांक].udata, कुंजी)) {
			सूचकांक = entries[सूचकांक].अग्र;
			do_assert(-1 <= सूचकांक && सूचकांक < पूर्णांक(entries.आकार()));
		}

		वापस सूचकांक;
	}

	पूर्णांक do_lookup_no_rehash(स्थिर K &कुंजी, Hasher::hash_t hash) स्थिर
	{
		अगर (hashtable.empty())
			वापस -1;

		वापस do_lookup_internal(कुंजी, hash);
	}

	पूर्णांक do_जोड़ो(स्थिर K &मूल्य, Hasher::hash_t &hash)
	{
		अगर (hashtable.empty()) {
			entries.emplace_back(मूल्य, -1);
			do_rehash();
			hash = do_hash(मूल्य);
		} अन्यथा {
			entries.emplace_back(मूल्य, hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		वापस entries.आकार() - 1;
	}

	पूर्णांक do_जोड़ो(K &&rvalue, Hasher::hash_t &hash)
	{
		अगर (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} अन्यथा {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		वापस entries.आकार() - 1;
	}

खुला:
	class const_iterator
	{
		मित्र class pool;
	रक्षित:
		स्थिर pool *ptr;
		पूर्णांक सूचकांक;
		const_iterator(स्थिर pool *ptr, पूर्णांक सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	खुला:
		प्रकार std::forward_iterator_tag iterator_category;
		प्रकार K मूल्य_type;
		प्रकार सूचक_भेद_प्रकार difference_type;
		प्रकार K* pointer;
		प्रकार K& reference;
		const_iterator() { }
		const_iterator चालक++() { सूचकांक--; वापस *this; }
		bool चालक==(स्थिर const_iterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
		bool चालक!=(स्थिर const_iterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
		स्थिर K &चालक*() स्थिर { वापस ptr->entries[सूचकांक].udata; }
		स्थिर K *चालक->() स्थिर { वापस &ptr->entries[सूचकांक].udata; }
	};

	class iterator
	{
		मित्र class pool;
	रक्षित:
		pool *ptr;
		पूर्णांक सूचकांक;
		iterator(pool *ptr, पूर्णांक सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	खुला:
		प्रकार std::forward_iterator_tag iterator_category;
		प्रकार K मूल्य_type;
		प्रकार सूचक_भेद_प्रकार difference_type;
		प्रकार K* pointer;
		प्रकार K& reference;
		iterator() { }
		iterator चालक++() { सूचकांक--; वापस *this; }
		bool चालक==(स्थिर iterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
		bool चालक!=(स्थिर iterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
		K &चालक*() { वापस ptr->entries[सूचकांक].udata; }
		K *चालक->() { वापस &ptr->entries[सूचकांक].udata; }
		स्थिर K &चालक*() स्थिर { वापस ptr->entries[सूचकांक].udata; }
		स्थिर K *चालक->() स्थिर { वापस &ptr->entries[सूचकांक].udata; }
		चालक const_iterator() स्थिर { वापस const_iterator(ptr, सूचकांक); }
	};

	constexpr pool()
	{
	}

	pool(स्थिर pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &चालक=(स्थिर pool &other) {
		entries = other.entries;
		do_rehash();
		वापस *this;
	}

	pool &चालक=(pool &&other) noexcept {
		clear();
		swap(other);
		वापस *this;
	}

	pool(स्थिर std::initializer_list<K> &list)
	{
		क्रम (स्वतः &it : list)
			जोड़ो(it);
	}

	ढाँचा<class निवेश_Iterator>
	pool(निवेश_Iterator first, निवेश_Iterator last)
	{
		जोड़ो(first, last);
	}

	ढाँचा<class निवेश_Iterator>
	व्योम जोड़ो(निवेश_Iterator first, निवेश_Iterator last)
	{
		क्रम (; first != last; ++first)
			जोड़ो(*first);
	}

	std::pair<iterator, bool> जोड़ो(स्थिर K &मूल्य)
	{
		Hasher::hash_t hash = do_hash(मूल्य);
		पूर्णांक i = do_lookup(मूल्य, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(मूल्य, hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> जोड़ो(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णांक i = do_lookup(rvalue, hash);
		अगर (i >= 0)
			वापस std::pair<iterator, bool>(iterator(this, i), false);
		i = do_जोड़ो(std::forward<K>(rvalue), hash);
		वापस std::pair<iterator, bool>(iterator(this, i), true);
	}

	ढाँचा<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		वापस जोड़ो(K(std::forward<Args>(args)...));
	}

	पूर्णांक erase(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक सूचकांक = do_lookup(कुंजी, hash);
		वापस do_erase(सूचकांक, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.सूचकांक, hash);
		वापस ++it;
	}

	पूर्णांक गिनती(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		वापस i < 0 ? 0 : 1;
	}

	पूर्णांक गिनती(स्थिर K &कुंजी, const_iterator it) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		वापस i < 0 || i > it.सूचकांक ? 0 : 1;
	}

	iterator find(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		अगर (i < 0)
			वापस end();
		वापस iterator(this, i);
	}

	const_iterator find(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			वापस end();
		वापस const_iterator(this, i);
	}

	bool चालक[](स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = do_hash(कुंजी);
		पूर्णांक i = do_lookup(कुंजी, hash);
		वापस i >= 0;
	}

	ढाँचा<typename Compare = std::less<K>>
	व्योम sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](स्थिर entry_t &a, स्थिर entry_t &b){ वापस comp(b.udata, a.udata); });
		do_rehash();
	}

	K निकालो()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		वापस ret;
	}

	व्योम swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool चालक==(स्थिर pool &other) स्थिर {
		अगर (आकार() != other.आकार())
			वापस false;
		क्रम (स्वतः &it : entries)
			अगर (!other.गिनती(it.udata))
				वापस false;
		वापस true;
	}

	bool चालक!=(स्थिर pool &other) स्थिर {
		वापस !चालक==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		commutative_hash comm;
		क्रम (स्वतः &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		वापस comm.hash_into(h);
	}

	व्योम reserve(माप_प्रकार n) { entries.reserve(n); }
	माप_प्रकार आकार() स्थिर { वापस entries.आकार(); }
	bool empty() स्थिर { वापस entries.empty(); }
	व्योम clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { वापस iterator(this, पूर्णांक(entries.आकार())-1); }
	iterator element(पूर्णांक n) { वापस iterator(this, पूर्णांक(entries.आकार())-1-n); }
	iterator end() { वापस iterator(nullptr, -1); }

	const_iterator begin() स्थिर { वापस const_iterator(this, पूर्णांक(entries.आकार())-1); }
	const_iterator element(पूर्णांक n) स्थिर { वापस const_iterator(this, पूर्णांक(entries.आकार())-1-n); }
	const_iterator end() स्थिर { वापस const_iterator(nullptr, -1); }
};

ढाँचा<typename K, पूर्णांक offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

खुला:
	class const_iterator
	{
		मित्र class idict;
	रक्षित:
		स्थिर idict &container;
		पूर्णांक सूचकांक;
		const_iterator(स्थिर idict &container, पूर्णांक सूचकांक) : container(container), सूचकांक(सूचकांक) { }
	खुला:
		प्रकार std::forward_iterator_tag iterator_category;
		प्रकार K मूल्य_type;
		प्रकार सूचक_भेद_प्रकार difference_type;
		प्रकार K* pointer;
		प्रकार K& reference;
		const_iterator() { }
		const_iterator चालक++() { सूचकांक++; वापस *this; }
		bool चालक==(स्थिर const_iterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
		bool चालक!=(स्थिर const_iterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
		स्थिर K &चालक*() स्थिर { वापस container[सूचकांक]; }
		स्थिर K *चालक->() स्थिर { वापस &container[सूचकांक]; }
	};

	constexpr idict()
	{
	}

	पूर्णांक चालक()(स्थिर K &कुंजी)
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णांक i = database.do_lookup(कुंजी, hash);
		अगर (i < 0)
			i = database.do_जोड़ो(कुंजी, hash);
		वापस i + offset;
	}

	पूर्णांक at(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णांक i = database.do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			throw std::out_of_range("idict::at()");
		वापस i + offset;
	}

	पूर्णांक at(स्थिर K &कुंजी, पूर्णांक defval) स्थिर
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णांक i = database.do_lookup_no_rehash(कुंजी, hash);
		अगर (i < 0)
			वापस defval;
		वापस i + offset;
	}

	पूर्णांक गिनती(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		पूर्णांक i = database.do_lookup_no_rehash(कुंजी, hash);
		वापस i < 0 ? 0 : 1;
	}

	पूर्णांक lookup(स्थिर K &कुंजी) स्थिर
	{
		Hasher::hash_t hash = database.do_hash(कुंजी);
		वापस database.do_lookup_no_rehash(कुंजी, hash);
	}

	व्योम expect(स्थिर K &कुंजी, पूर्णांक i)
	{
		पूर्णांक j = (*this)(कुंजी);
		अगर (i != j)
			throw std::out_of_range("idict::expect()");
	}

	स्थिर K &चालक[](पूर्णांक सूचकांक) स्थिर
	{
		वापस database.entries.at(सूचकांक - offset).udata;
	}

	व्योम swap(idict &other)
	{
		database.swap(other.database);
	}

	व्योम reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार आकार() स्थिर { वापस database.आकार(); }
	bool empty() स्थिर { वापस database.empty(); }
	व्योम clear() { database.clear(); }

	const_iterator begin() स्थिर { वापस const_iterator(*this, offset); }
	const_iterator element(पूर्णांक n) स्थिर { वापस const_iterator(*this, n); }
	const_iterator end() स्थिर { वापस const_iterator(*this, offset + आकार()); }
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
	खुला:
		explicit AtomicParent(पूर्णांक p) : parent(p) {}
		AtomicParent(स्थिर AtomicParent &other) : parent(other.get()) {}
		AtomicParent &चालक=(स्थिर AtomicParent &other) { set(other.get()); वापस *this; }
		पूर्णांक get() स्थिर { वापस parent.load(std::memory_order_relaxed); }
		व्योम set(पूर्णांक p) { parent.store(p, std::memory_order_relaxed); }
	निजी:
		std::atomic<पूर्णांक> parent;
	};
	std::vector<AtomicParent> parents;

खुला:
	प्रकार typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	पूर्णांक चालक()(स्थिर K &कुंजी)
	{
		पूर्णांक i = database(कुंजी);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		अगर (parents.आकार() < database.आकार()) {
			parents.emplace_back(-1);
		}
		वापस i;
	}

	// Finds an element at given index
	स्थिर K &चालक[](पूर्णांक सूचकांक) स्थिर
	{
		वापस database[सूचकांक];
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
	पूर्णांक ifind(पूर्णांक i) स्थिर
	{
		पूर्णांक p = i, k = i;

		जबतक (true) {
			पूर्णांक pp = parents[p].get();
			अगर (pp < 0)
				अवरोध;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		जबतक (k != p) {
			पूर्णांक अग्र_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = अग्र_k;
		}

		वापस p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	व्योम imerge(पूर्णांक i, पूर्णांक j)
	{
		i = ifind(i);
		j = ifind(j);

		अगर (i != j)
			parents[i].set(j);
	}

	व्योम ipromote(पूर्णांक i)
	{
		पूर्णांक k = i;

		जबतक (k != -1) {
			पूर्णांक अग्र_k = parents[k].get();
			parents[k].set(i);
			k = अग्र_k;
		}

		parents[i].set(-1);
	}

	पूर्णांक lookup(स्थिर K &a)
	{
		वापस ifind((*this)(a));
	}

	स्थिर K &find(स्थिर K &a) स्थिर
	{
		पूर्णांक i = database.at(a, -1);
		अगर (i < 0)
			वापस a;
		वापस (*this)[ifind(i)];
	}

	व्योम merge(स्थिर K &a, स्थिर K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	व्योम promote(स्थिर K &a)
	{
		पूर्णांक i = database.at(a, -1);
		अगर (i >= 0)
			ipromote(i);
	}

	व्योम swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	व्योम reserve(माप_प्रकार n) { database.reserve(n); }
	माप_प्रकार आकार() स्थिर { वापस database.आकार(); }
	bool empty() स्थिर { वापस database.empty(); }
	व्योम clear() { database.clear(); parents.clear(); }

	const_iterator begin() स्थिर { वापस database.begin(); }
	const_iterator element(पूर्णांक n) स्थिर { वापस database.element(n); }
	const_iterator end() स्थिर { वापस database.end(); }
};

} /* namespace hashlib */

#पूर्ण_अगर
