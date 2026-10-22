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

#അന്തര്ഭാവ <array>
#അന്തര്ഭാവ <atomic>
#അന്തര്ഭാവ <stdexcept>
#അന്തര്ഭാവ <algorithm>
#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <variant>
#അന്തര്ഭാവ <vector>
#അന്തര്ഭാവ <type_traits>
#അന്തര്ഭാവ <stdint.h>

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

#എങ്കിൽ defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED __attribute__((warn_unused))
#അല്ലെങ്കിൽ
#  define HASHLIB_ATTRIBUTE_WARN_UNUSED
#endif

സ്ഥിരം പൂർണ്ണസംഖ്യ hashtable_size_trigger = 2;
സ്ഥിരം പൂർണ്ണസംഖ്യ hashtable_size_factor = 3;

namespace legacy {
	വരിയിൽ uint32_t djb2_add(uint32_t a, uint32_t b) {
		തിരികെ ((a << 5) + a) + b;
	}
};

template<typename T>
ഘടന hash_ops;

വരിയിൽ unsigned പൂർണ്ണസംഖ്യ mkhash_xorshift(unsigned പൂർണ്ണസംഖ്യ a) {
	എങ്കിൽ (വലുപ്പം(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} അല്ലെങ്കിൽ എങ്കിൽ (വലുപ്പം(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} അല്ലെങ്കിൽ
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	തിരികെ a;
}

class HasherDJB32 {
പൊതു:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

സ്വകാര്യ:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	സ്ഥിതം uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		തിരികെ hash;
	}
പൊതു:
	ശൂന്യം hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		തിരികെ;
	}
	ശൂന്യം hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		തിരികെ;
	}
	[[nodiscard]]
	hash_t yield() സ്ഥിരം {
		തിരികെ (hash_t)state;
	}

	template<typename T>
	ശൂന്യം eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	template<typename T>
	ശൂന്യം eat(സ്ഥിരം T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	ശൂന്യം commutative_eat(hash_t t) {
		state ^= t;
	}

	ശൂന്യം force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#define HASH_മുകൾ_LOOP_FST [[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash
#define HASH_മുകൾ_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	തിരികെ h; \
}

template<typename T>
ഘടന hash_ops {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം T &a, സ്ഥിരം T &b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം T &a, Hasher h) {
		എങ്കിൽ constexpr (std::is_integral_v<T>) {
			static_assert(വലുപ്പം(T) <= വലുപ്പം(uint64_t));
			എങ്കിൽ (വലുപ്പം(T) == വലുപ്പം(uint64_t))
				h.hash64(a);
			അല്ലെങ്കിൽ
				h.hash32(a);
			തിരികെ h;
		} അല്ലെങ്കിൽ എങ്കിൽ constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			തിരികെ hash_ops<u_type>::hash_into((u_type) a, h);
		} അല്ലെങ്കിൽ എങ്കിൽ constexpr (std::is_pointer_v<T>) {
			തിരികെ hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} അല്ലെങ്കിൽ എങ്കിൽ constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			പൂർണ്ണസംഖ്യ size = a.size();
			പൂർണ്ണസംഖ്യ i = 0;
			ആകുമ്പോൾ (i + 8 < size) {
				uint64_t v;
				memcpy(&v, a.data() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			memcpy(&v, a.data() + i, size - i);
			h.hash64(v);
			തിരികെ h;
		} അല്ലെങ്കിൽ {
			തിരികെ a.hash_into(h);
		}
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം T &a) HASH_മുകൾ_LOOP_SND
};

template<typename P, typename Q> ഘടന hash_ops<std::pair<P, Q>> {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::pair<P, Q> &a, സ്ഥിരം std::pair<P, Q> &b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		തിരികെ h;
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം std::pair<P, Q> &a) HASH_മുകൾ_LOOP_SND
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash(സ്ഥിരം P &p, സ്ഥിരം Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		തിരികെ h;
	}
};

template<typename... T> ഘടന hash_ops<std::tuple<T...>> {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::tuple<T...> &a, സ്ഥിരം std::tuple<T...> &b) {
		തിരികെ a == b;
	}
	template<size_t I = 0>
	സ്ഥിതം വരിയിൽ typename std::enable_if<I == വലുപ്പം...(T), Hasher>::type hash_into(സ്ഥിരം std::tuple<T...> &, Hasher h) {
		തിരികെ h;
	}
	template<size_t I = 0>
	സ്ഥിതം വരിയിൽ typename std::enable_if<I != വലുപ്പം...(T), Hasher>::type hash_into(സ്ഥിരം std::tuple<T...> &a, Hasher h) {
		തരനിർവചനം hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		തിരികെ h;
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം std::tuple<T...> &a) HASH_മുകൾ_LOOP_SND
};

template<typename T> ഘടന hash_ops<std::vector<T>> {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::vector<T> &a, സ്ഥിരം std::vector<T> &b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം std::vector<T> &a, Hasher h) {
		h.eat((uint32_t)a.size());
		വേണ്ടി (auto k : a)
			h.eat(k);
		തിരികെ h;
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം std::vector<T> &a) HASH_മുകൾ_LOOP_SND
};

template<typename T, size_t N> ഘടന hash_ops<std::array<T, N>> {
    സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::array<T, N> &a, സ്ഥിരം std::array<T, N> &b) {
        തിരികെ a == b;
    }
    [[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം std::array<T, N> &a, Hasher h) {
        വേണ്ടി (സ്ഥിരം auto& k : a)
            h = hash_ops<T>::hash_into(k, h);
        തിരികെ h;
    }
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം std::array<T, N> &a) HASH_മുകൾ_LOOP_SND
};

ഘടന hash_cstr_ops {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
		തിരികെ strcmp(a, b) == 0;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം അക്ഷരം *a, Hasher h) {
		ആകുമ്പോൾ (*a)
			h.hash32(*(a++));
		തിരികെ h;
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം അക്ഷരം *a) HASH_മുകൾ_LOOP_SND
};

template <> ഘടന hash_ops<അക്ഷരം*> : hash_cstr_ops {};

ഘടന hash_ptr_ops {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം ശൂന്യം *a, Hasher h) {
		തിരികെ hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം ശൂന്യം *a) HASH_മുകൾ_LOOP_SND
};

ഘടന hash_obj_ops {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b) {
		തിരികെ a == b;
	}
	template<typename T>
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം T *a, Hasher h) {
		എങ്കിൽ (a)
			h = a->hash_into(h);
		അല്ലെങ്കിൽ
			h.eat(0);
		തിരികെ h;
	}
	template<typename T>
	HASH_മുകൾ_LOOP_FST (സ്ഥിരം T *a) HASH_മുകൾ_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
template<typename T>
[[nodiscard]]
Hasher::hash_t run_hash(സ്ഥിരം T& obj) {
	തിരികെ hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
template<typename T>
[[nodiscard]]
[[deprecated]]
വരിയിൽ unsigned പൂർണ്ണസംഖ്യ mkhash(സ്ഥിരം T &v) {
	തിരികെ (unsigned പൂർണ്ണസംഖ്യ) run_hash<T>(v);
}

template<> ഘടന hash_ops<std::monostate> {
	സ്ഥിതം വരിയിൽ bool cmp(std::monostate a, std::monostate b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(std::monostate, Hasher h) {
		തിരികെ h;
	}
};

template<typename... T> ഘടന hash_ops<std::variant<T...>> {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::variant<T...> &a, സ്ഥിരം std::variant<T...> &b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം std::variant<T...> &a, Hasher h) {
		std::visit([& h](സ്ഥിരം auto &v) { h.eat(v); }, a);
		h.eat(a.index());
		തിരികെ h;
	}
};

template<typename T> ഘടന hash_ops<std::optional<T>> {
	സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം std::optional<T> &a, സ്ഥിരം std::optional<T> &b) {
		തിരികെ a == b;
	}
	[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം std::optional<T> &a, Hasher h) {
		എങ്കിൽ(a.has_മൂല്യം())
			h.eat(*a);
		അല്ലെങ്കിൽ
			h.eat(0);
		തിരികെ h;
	}
};

വരിയിൽ unsigned പൂർണ്ണസംഖ്യ hashtable_size(unsigned പൂർണ്ണസംഖ്യ min_size)
{
	// Primes as generated by https://oeis.org/A175953
	സ്ഥിതം std::vector<unsigned പൂർണ്ണസംഖ്യ> zero_and_some_primes = {
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

	വേണ്ടി (auto p : zero_and_some_primes)
		എങ്കിൽ (p >= min_size) തിരികെ p;

	എങ്കിൽ (വലുപ്പം(unsigned പൂർണ്ണസംഖ്യ) == 4)
		throw std::length_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	വേണ്ടി (auto p : zero_and_some_primes)
		എങ്കിൽ (100129 * p > min_size) തിരികെ 100129 * p;

	throw std::length_error("hash table exceeded maximum size.");
}

template<typename K, typename T, typename OPS = hash_ops<K>> class dict;
template<typename K, പൂർണ്ണസംഖ്യ offset = 0, typename OPS = hash_ops<K>> class idict;
template<typename K, typename OPS = hash_ops<K>> class pool;
template<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
പൊതു:
	commutative_hash() {
		buckets.fill(0);
	}
	template <typename T>
	ശൂന്യം eat(സ്ഥിരം T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	ശൂന്യം eat(സ്ഥിരം Hasher &h) {
		Hasher::hash_t v = h.yield();
		size_t index = v & (buckets.size() - 1);
		buckets[index] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		വേണ്ടി (auto b : buckets)
			h.eat(b);
		തിരികെ h;
	}
സ്വകാര്യ:
	std::array<Hasher::hash_t, 4> buckets;
};

template<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED dict {
	ഘടന entry_t
	{
		std::pair<K, T> udata;
		പൂർണ്ണസംഖ്യ next;

		entry_t() { }
		entry_t(സ്ഥിരം std::pair<K, T> &udata, പൂർണ്ണസംഖ്യ next) : udata(udata), next(next) { }
		entry_t(std::pair<K, T> &&udata, പൂർണ്ണസംഖ്യ next) : udata(std::move(udata)), next(next) { }
		bool operator<(സ്ഥിരം entry_t &other) സ്ഥിരം { തിരികെ udata.first < other.udata.first; }
	};

	std::vector<പൂർണ്ണസംഖ്യ> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	സ്ഥിതം വരിയിൽ ശൂന്യം do_assert(bool) { }
#അല്ലെങ്കിൽ
	സ്ഥിതം വരിയിൽ ശൂന്യം do_assert(bool cond) {
		എങ്കിൽ (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = 0;
		എങ്കിൽ (!hashtable.empty())
			hash = ops.hash(താക്കോൽ).yield() % (unsigned പൂർണ്ണസംഖ്യ)(hashtable.size());
		തിരികെ hash;
	}

	ശൂന്യം do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < പൂർണ്ണസംഖ്യ(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < പൂർണ്ണസംഖ്യ(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	പൂർണ്ണസംഖ്യ do_erase(പൂർണ്ണസംഖ്യ index, Hasher::hash_t hash)
	{
		do_assert(index < പൂർണ്ണസംഖ്യ(entries.size()));
		എങ്കിൽ (hashtable.empty() || index < 0)
			തിരികെ 0;

		പൂർണ്ണസംഖ്യ k = hashtable[hash];
		do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));

		എങ്കിൽ (k == index) {
			hashtable[hash] = entries[index].next;
		} അല്ലെങ്കിൽ {
			ആകുമ്പോൾ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		പൂർണ്ണസംഖ്യ back_idx = entries.size()-1;

		എങ്കിൽ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata.first);

			k = hashtable[back_hash];
			do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));

			എങ്കിൽ (k == back_idx) {
				hashtable[back_hash] = index;
			} അല്ലെങ്കിൽ {
				ആകുമ്പോൾ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.എടുക്കുക_back();

		എങ്കിൽ (entries.empty())
			hashtable.clear();

		തിരികെ 1;
	}

	പൂർണ്ണസംഖ്യ do_lookup(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty())
			തിരികെ -1;

		എങ്കിൽ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(താക്കോൽ);
		}

		തിരികെ do_lookup_internal(താക്കോൽ, hash);
	}

	പൂർണ്ണസംഖ്യ do_lookup_internal(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t hash) സ്ഥിരം
	{
		പൂർണ്ണസംഖ്യ index = hashtable[hash];

		ആകുമ്പോൾ (index >= 0 && !ops.cmp(entries[index].udata.first, താക്കോൽ)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < പൂർണ്ണസംഖ്യ(entries.size()));
		}

		തിരികെ index;
	}

	പൂർണ്ണസംഖ്യ do_lookup_no_rehash(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t hash) സ്ഥിരം
	{
		എങ്കിൽ (hashtable.empty())
			തിരികെ -1;

		തിരികെ do_lookup_internal(താക്കോൽ, hash);
	}

	പൂർണ്ണസംഖ്യ do_തിരുകുക(സ്ഥിരം K &താക്കോൽ, സ്ഥിരം Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty()) {
			entries.emplace_back(std::pair<K, T>(താക്കോൽ, T()), -1);
			do_rehash();
		} അല്ലെങ്കിൽ {
			entries.emplace_back(std::pair<K, T>(താക്കോൽ, T()), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		തിരികെ entries.size() - 1;
	}

	പൂർണ്ണസംഖ്യ do_തിരുകുക(സ്ഥിരം std::pair<K, T> &മൂല്യം, സ്ഥിരം Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty()) {
			entries.emplace_back(മൂല്യം, -1);
			do_rehash();
		} അല്ലെങ്കിൽ {
			entries.emplace_back(മൂല്യം, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		തിരികെ entries.size() - 1;
	}

	പൂർണ്ണസംഖ്യ do_തിരുകുക(std::pair<K, T> &&rvalue, സ്ഥിരം Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty()) {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} അല്ലെങ്കിൽ {
			entries.emplace_back(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		തിരികെ entries.size() - 1;
	}

പൊതു:
	class const_iterator
	{
		friend class dict;
	സംരക്ഷിത:
		സ്ഥിരം dict *ptr;
		പൂർണ്ണസംഖ്യ index;
		const_iterator(സ്ഥിരം dict *ptr, പൂർണ്ണസംഖ്യ index) : ptr(ptr), index(index) { }
	പൊതു:
		തരനിർവചനം std::bidirectional_iterator_tag iterator_category;
		തരനിർവചനം std::pair<K, T> മൂല്യം_type;
		തരനിർവചനം ptrdiff_t difference_type;
		തരനിർവചനം സ്ഥിരം std::pair<K, T>* pointer;
		തരനിർവചനം സ്ഥിരം std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator operator++() { index--; തിരികെ *this; }
		const_iterator operator++(പൂർണ്ണസംഖ്യ) { const_iterator tmp = *this; index--; തിരികെ tmp; }
		const_iterator operator--() { index++; തിരികെ *this; }
		const_iterator operator--(പൂർണ്ണസംഖ്യ) { const_iterator tmp = *this; index++; തിരികെ tmp; }
		const_iterator operator+=(പൂർണ്ണസംഖ്യ amt) { index -= amt; തിരികെ *this; }
		bool operator<(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index > other.index; }
		bool operator==(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index == other.index; }
		bool operator!=(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index != other.index; }
		സ്ഥിരം std::pair<K, T> &operator*() സ്ഥിരം { തിരികെ ptr->entries[index].udata; }
		സ്ഥിരം std::pair<K, T> *operator->() സ്ഥിരം { തിരികെ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class dict;
	സംരക്ഷിത:
		dict *ptr;
		പൂർണ്ണസംഖ്യ index;
		iterator(dict *ptr, പൂർണ്ണസംഖ്യ index) : ptr(ptr), index(index) { }
	പൊതു:
		തരനിർവചനം std::forward_iterator_tag iterator_category;
		തരനിർവചനം std::pair<K, T> മൂല്യം_type;
		തരനിർവചനം ptrdiff_t difference_type;
		തരനിർവചനം std::pair<K, T>* pointer;
		തരനിർവചനം std::pair<K, T>& reference;
		iterator() { }
		iterator operator++() { index--; തിരികെ *this; }
		iterator operator+=(പൂർണ്ണസംഖ്യ amt) { index -= amt; തിരികെ *this; }
		bool operator<(സ്ഥിരം iterator &other) സ്ഥിരം { തിരികെ index > other.index; }
		bool operator==(സ്ഥിരം iterator &other) സ്ഥിരം { തിരികെ index == other.index; }
		bool operator!=(സ്ഥിരം iterator &other) സ്ഥിരം { തിരികെ index != other.index; }
		std::pair<K, T> &operator*() { തിരികെ ptr->entries[index].udata; }
		std::pair<K, T> *operator->() { തിരികെ &ptr->entries[index].udata; }
		സ്ഥിരം std::pair<K, T> &operator*() സ്ഥിരം { തിരികെ ptr->entries[index].udata; }
		സ്ഥിരം std::pair<K, T> *operator->() സ്ഥിരം { തിരികെ &ptr->entries[index].udata; }
		operator const_iterator() സ്ഥിരം { തിരികെ const_iterator(ptr, index); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() സ്ഥിരം {
		തിരികെ std::make_reverse_iterator(end());
	}
	reverse_iterator rend() സ്ഥിരം {
		തിരികെ std::make_reverse_iterator(begin());
	}

	constexpr dict()
	{
	}

	dict(സ്ഥിരം dict &other)
	{
		entries = other.entries;
		do_rehash();
	}

	dict(dict &&other) noexcept
	{
		swap(other);
	}

	dict &operator=(സ്ഥിരം dict &other) {
		entries = other.entries;
		do_rehash();
		തിരികെ *this;
	}

	dict &operator=(dict &&other) noexcept {
		clear();
		swap(other);
		തിരികെ *this;
	}

	dict(സ്ഥിരം std::initializer_list<std::pair<K, T>> &list)
	{
		വേണ്ടി (auto &it : list)
			തിരുകുക(it);
	}

	template<class നിവേശം_Iterator>
	dict(നിവേശം_Iterator first, നിവേശം_Iterator last)
	{
		തിരുകുക(first, last);
	}

	template<class നിവേശം_Iterator>
	ശൂന്യം തിരുകുക(നിവേശം_Iterator first, നിവേശം_Iterator last)
	{
		വേണ്ടി (; first != last; ++first)
			തിരുകുക(*first);
	}

	std::pair<iterator, bool> തിരുകുക(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(താക്കോൽ, hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> തിരുകുക(സ്ഥിരം std::pair<K, T> &മൂല്യം)
	{
		Hasher::hash_t hash = do_hash(മൂല്യം.first);
		പൂർണ്ണസംഖ്യ i = do_lookup(മൂല്യം.first, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(മൂല്യം, hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> തിരുകുക(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		പൂർണ്ണസംഖ്യ i = do_lookup(rvalue.first, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::forward<std::pair<K, T>>(rvalue), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K സ്ഥിരം &താക്കോൽ, T സ്ഥിരം &മൂല്യം)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::make_pair(താക്കോൽ, മൂല്യം), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K സ്ഥിരം &താക്കോൽ, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::make_pair(താക്കോൽ, std::forward<T>(rvalue)), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T സ്ഥിരം &മൂല്യം)
	{
		Hasher::hash_t hash = do_hash(rkey);
		പൂർണ്ണസംഖ്യ i = do_lookup(rkey, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::make_pair(std::forward<K>(rkey), മൂല്യം), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		പൂർണ്ണസംഖ്യ i = do_lookup(rkey, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	പൂർണ്ണസംഖ്യ erase(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ index = do_lookup(താക്കോൽ, hash);
		തിരികെ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.index, hash);
		തിരികെ ++it;
	}

	പൂർണ്ണസംഖ്യ എണ്ണം(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		തിരികെ i < 0 ? 0 : 1;
	}

	പൂർണ്ണസംഖ്യ എണ്ണം(സ്ഥിരം K &താക്കോൽ, const_iterator it) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		തിരികെ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ end();
		തിരികെ iterator(this, i);
	}

	const_iterator find(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ end();
		തിരികെ const_iterator(this, i);
	}

	T& at(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			throw std::out_of_range("dict::at()");
		തിരികെ entries[i].udata.second;
	}

	സ്ഥിരം T& at(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			throw std::out_of_range("dict::at()");
		തിരികെ entries[i].udata.second;
	}

	സ്ഥിരം T& at(സ്ഥിരം K &താക്കോൽ, സ്ഥിരം T &defval) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ defval;
		തിരികെ entries[i].udata.second;
	}

	T& operator[](സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			i = do_തിരുകുക(std::pair<K, T>(താക്കോൽ, T()), hash);
		തിരികെ entries[i].udata.second;
	}

	template<typename Compare = std::less<K>>
	ശൂന്യം sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](സ്ഥിരം entry_t &a, സ്ഥിരം entry_t &b){ തിരികെ comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	ശൂന്യം swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(സ്ഥിരം dict &other) സ്ഥിരം {
		എങ്കിൽ (size() != other.size())
			തിരികെ false;
		വേണ്ടി (auto &it : entries) {
			auto oit = other.find(it.udata.first);
			എങ്കിൽ (oit == other.end() || !(oit->second == it.udata.second))
				തിരികെ false;
		}
		തിരികെ true;
	}

	bool operator!=(സ്ഥിരം dict &other) സ്ഥിരം {
		തിരികെ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		commutative_hash comm;
		വേണ്ടി (auto &it : entries) {
			Hasher entry_hash;
			entry_hash.eat(it.udata.first);
			entry_hash.eat(it.udata.second);
			comm.eat(entry_hash);
		}
		തിരികെ comm.hash_into(h);
	}

	ശൂന്യം reserve(size_t n) { entries.reserve(n); }
	size_t size() സ്ഥിരം { തിരികെ entries.size(); }
	bool empty() സ്ഥിരം { തിരികെ entries.empty(); }
	ശൂന്യം clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { തിരികെ iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1); }
	iterator element(പൂർണ്ണസംഖ്യ n) { തിരികെ iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1-n); }
	iterator end() { തിരികെ iterator(nullptr, -1); }

	const_iterator begin() സ്ഥിരം { തിരികെ const_iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1); }
	const_iterator element(പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ const_iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1-n); }
	const_iterator end() സ്ഥിരം { തിരികെ const_iterator(this, -1); }
};

template<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED pool
{
	template<typename, പൂർണ്ണസംഖ്യ, typename> friend class idict;

സംരക്ഷിത:
	ഘടന entry_t
	{
		K udata;
		പൂർണ്ണസംഖ്യ next;

		entry_t() { }
		entry_t(സ്ഥിരം K &udata, പൂർണ്ണസംഖ്യ next) : udata(udata), next(next) { }
		entry_t(K &&udata, പൂർണ്ണസംഖ്യ next) : udata(std::move(udata)), next(next) { }
	};

	std::vector<പൂർണ്ണസംഖ്യ> hashtable;
	std::vector<entry_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#ifdef NDEBUG
	സ്ഥിതം വരിയിൽ ശൂന്യം do_assert(bool) { }
#അല്ലെങ്കിൽ
	സ്ഥിതം വരിയിൽ ശൂന്യം do_assert(bool cond) {
		എങ്കിൽ (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#endif

	Hasher::hash_t do_hash(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = 0;
		എങ്കിൽ (!hashtable.empty())
			hash = ops.hash(താക്കോൽ).yield() % (unsigned പൂർണ്ണസംഖ്യ)(hashtable.size());
		തിരികെ hash;
	}

	ശൂന്യം do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_size(entries.capacity() * hashtable_size_factor), -1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < പൂർണ്ണസംഖ്യ(entries.size()); i++) {
			do_assert(-1 <= entries[i].next && entries[i].next < പൂർണ്ണസംഖ്യ(entries.size()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].next = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	പൂർണ്ണസംഖ്യ do_erase(പൂർണ്ണസംഖ്യ index, Hasher::hash_t hash)
	{
		do_assert(index < പൂർണ്ണസംഖ്യ(entries.size()));
		എങ്കിൽ (hashtable.empty() || index < 0)
			തിരികെ 0;

		പൂർണ്ണസംഖ്യ k = hashtable[hash];
		എങ്കിൽ (k == index) {
			hashtable[hash] = entries[index].next;
		} അല്ലെങ്കിൽ {
			ആകുമ്പോൾ (entries[k].next != index) {
				k = entries[k].next;
				do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));
			}
			entries[k].next = entries[index].next;
		}

		പൂർണ്ണസംഖ്യ back_idx = entries.size()-1;

		എങ്കിൽ (index != back_idx)
		{
			Hasher::hash_t back_hash = do_hash(entries[back_idx].udata);

			k = hashtable[back_hash];
			എങ്കിൽ (k == back_idx) {
				hashtable[back_hash] = index;
			} അല്ലെങ്കിൽ {
				ആകുമ്പോൾ (entries[k].next != back_idx) {
					k = entries[k].next;
					do_assert(0 <= k && k < പൂർണ്ണസംഖ്യ(entries.size()));
				}
				entries[k].next = index;
			}

			entries[index] = std::move(entries[back_idx]);
		}

		entries.എടുക്കുക_back();

		എങ്കിൽ (entries.empty())
			hashtable.clear();

		തിരികെ 1;
	}

	പൂർണ്ണസംഖ്യ do_lookup(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty())
			തിരികെ -1;

		എങ്കിൽ (entries.size() * hashtable_size_trigger > hashtable.size()) {
			do_rehash();
			hash = do_hash(താക്കോൽ);
		}

		തിരികെ do_lookup_internal(താക്കോൽ, hash);
	}

	പൂർണ്ണസംഖ്യ do_lookup_internal(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t hash) സ്ഥിരം
	{
		പൂർണ്ണസംഖ്യ index = hashtable[hash];

		ആകുമ്പോൾ (index >= 0 && !ops.cmp(entries[index].udata, താക്കോൽ)) {
			index = entries[index].next;
			do_assert(-1 <= index && index < പൂർണ്ണസംഖ്യ(entries.size()));
		}

		തിരികെ index;
	}

	പൂർണ്ണസംഖ്യ do_lookup_no_rehash(സ്ഥിരം K &താക്കോൽ, Hasher::hash_t hash) സ്ഥിരം
	{
		എങ്കിൽ (hashtable.empty())
			തിരികെ -1;

		തിരികെ do_lookup_internal(താക്കോൽ, hash);
	}

	പൂർണ്ണസംഖ്യ do_തിരുകുക(സ്ഥിരം K &മൂല്യം, Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty()) {
			entries.emplace_back(മൂല്യം, -1);
			do_rehash();
			hash = do_hash(മൂല്യം);
		} അല്ലെങ്കിൽ {
			entries.emplace_back(മൂല്യം, hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		തിരികെ entries.size() - 1;
	}

	പൂർണ്ണസംഖ്യ do_തിരുകുക(K &&rvalue, Hasher::hash_t &hash)
	{
		എങ്കിൽ (hashtable.empty()) {
			entries.emplace_back(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.back().udata);
		} അല്ലെങ്കിൽ {
			entries.emplace_back(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.size() - 1;
		}
		തിരികെ entries.size() - 1;
	}

പൊതു:
	class const_iterator
	{
		friend class pool;
	സംരക്ഷിത:
		സ്ഥിരം pool *ptr;
		പൂർണ്ണസംഖ്യ index;
		const_iterator(സ്ഥിരം pool *ptr, പൂർണ്ണസംഖ്യ index) : ptr(ptr), index(index) { }
	പൊതു:
		തരനിർവചനം std::forward_iterator_tag iterator_category;
		തരനിർവചനം K മൂല്യം_type;
		തരനിർവചനം ptrdiff_t difference_type;
		തരനിർവചനം K* pointer;
		തരനിർവചനം K& reference;
		const_iterator() { }
		const_iterator operator++() { index--; തിരികെ *this; }
		bool operator==(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index == other.index; }
		bool operator!=(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index != other.index; }
		സ്ഥിരം K &operator*() സ്ഥിരം { തിരികെ ptr->entries[index].udata; }
		സ്ഥിരം K *operator->() സ്ഥിരം { തിരികെ &ptr->entries[index].udata; }
	};

	class iterator
	{
		friend class pool;
	സംരക്ഷിത:
		pool *ptr;
		പൂർണ്ണസംഖ്യ index;
		iterator(pool *ptr, പൂർണ്ണസംഖ്യ index) : ptr(ptr), index(index) { }
	പൊതു:
		തരനിർവചനം std::forward_iterator_tag iterator_category;
		തരനിർവചനം K മൂല്യം_type;
		തരനിർവചനം ptrdiff_t difference_type;
		തരനിർവചനം K* pointer;
		തരനിർവചനം K& reference;
		iterator() { }
		iterator operator++() { index--; തിരികെ *this; }
		bool operator==(സ്ഥിരം iterator &other) സ്ഥിരം { തിരികെ index == other.index; }
		bool operator!=(സ്ഥിരം iterator &other) സ്ഥിരം { തിരികെ index != other.index; }
		K &operator*() { തിരികെ ptr->entries[index].udata; }
		K *operator->() { തിരികെ &ptr->entries[index].udata; }
		സ്ഥിരം K &operator*() സ്ഥിരം { തിരികെ ptr->entries[index].udata; }
		സ്ഥിരം K *operator->() സ്ഥിരം { തിരികെ &ptr->entries[index].udata; }
		operator const_iterator() സ്ഥിരം { തിരികെ const_iterator(ptr, index); }
	};

	constexpr pool()
	{
	}

	pool(സ്ഥിരം pool &other)
	{
		entries = other.entries;
		do_rehash();
	}

	pool(pool &&other) noexcept
	{
		swap(other);
	}

	pool &operator=(സ്ഥിരം pool &other) {
		entries = other.entries;
		do_rehash();
		തിരികെ *this;
	}

	pool &operator=(pool &&other) noexcept {
		clear();
		swap(other);
		തിരികെ *this;
	}

	pool(സ്ഥിരം std::initializer_list<K> &list)
	{
		വേണ്ടി (auto &it : list)
			തിരുകുക(it);
	}

	template<class നിവേശം_Iterator>
	pool(നിവേശം_Iterator first, നിവേശം_Iterator last)
	{
		തിരുകുക(first, last);
	}

	template<class നിവേശം_Iterator>
	ശൂന്യം തിരുകുക(നിവേശം_Iterator first, നിവേശം_Iterator last)
	{
		വേണ്ടി (; first != last; ++first)
			തിരുകുക(*first);
	}

	std::pair<iterator, bool> തിരുകുക(സ്ഥിരം K &മൂല്യം)
	{
		Hasher::hash_t hash = do_hash(മൂല്യം);
		പൂർണ്ണസംഖ്യ i = do_lookup(മൂല്യം, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(മൂല്യം, hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> തിരുകുക(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		പൂർണ്ണസംഖ്യ i = do_lookup(rvalue, hash);
		എങ്കിൽ (i >= 0)
			തിരികെ std::pair<iterator, bool>(iterator(this, i), false);
		i = do_തിരുകുക(std::forward<K>(rvalue), hash);
		തിരികെ std::pair<iterator, bool>(iterator(this, i), true);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		തിരികെ തിരുകുക(K(std::forward<Args>(args)...));
	}

	പൂർണ്ണസംഖ്യ erase(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ index = do_lookup(താക്കോൽ, hash);
		തിരികെ do_erase(index, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.index, hash);
		തിരികെ ++it;
	}

	പൂർണ്ണസംഖ്യ എണ്ണം(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		തിരികെ i < 0 ? 0 : 1;
	}

	പൂർണ്ണസംഖ്യ എണ്ണം(സ്ഥിരം K &താക്കോൽ, const_iterator it) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		തിരികെ i < 0 || i > it.index ? 0 : 1;
	}

	iterator find(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ end();
		തിരികെ iterator(this, i);
	}

	const_iterator find(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ end();
		തിരികെ const_iterator(this, i);
	}

	bool operator[](സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = do_lookup(താക്കോൽ, hash);
		തിരികെ i >= 0;
	}

	template<typename Compare = std::less<K>>
	ശൂന്യം sort(Compare comp = Compare())
	{
		std::sort(entries.begin(), entries.end(), [comp](സ്ഥിരം entry_t &a, സ്ഥിരം entry_t &b){ തിരികെ comp(b.udata, a.udata); });
		do_rehash();
	}

	K എടുക്കുക()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		തിരികെ ret;
	}

	ശൂന്യം swap(pool &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool operator==(സ്ഥിരം pool &other) സ്ഥിരം {
		എങ്കിൽ (size() != other.size())
			തിരികെ false;
		വേണ്ടി (auto &it : entries)
			എങ്കിൽ (!other.എണ്ണം(it.udata))
				തിരികെ false;
		തിരികെ true;
	}

	bool operator!=(സ്ഥിരം pool &other) സ്ഥിരം {
		തിരികെ !operator==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		commutative_hash comm;
		വേണ്ടി (auto &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		തിരികെ comm.hash_into(h);
	}

	ശൂന്യം reserve(size_t n) { entries.reserve(n); }
	size_t size() സ്ഥിരം { തിരികെ entries.size(); }
	bool empty() സ്ഥിരം { തിരികെ entries.empty(); }
	ശൂന്യം clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { തിരികെ iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1); }
	iterator element(പൂർണ്ണസംഖ്യ n) { തിരികെ iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1-n); }
	iterator end() { തിരികെ iterator(nullptr, -1); }

	const_iterator begin() സ്ഥിരം { തിരികെ const_iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1); }
	const_iterator element(പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ const_iterator(this, പൂർണ്ണസംഖ്യ(entries.size())-1-n); }
	const_iterator end() സ്ഥിരം { തിരികെ const_iterator(nullptr, -1); }
};

template<typename K, പൂർണ്ണസംഖ്യ offset, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_UNUSED idict
{
	pool<K, OPS> database;

പൊതു:
	class const_iterator
	{
		friend class idict;
	സംരക്ഷിത:
		സ്ഥിരം idict &container;
		പൂർണ്ണസംഖ്യ index;
		const_iterator(സ്ഥിരം idict &container, പൂർണ്ണസംഖ്യ index) : container(container), index(index) { }
	പൊതു:
		തരനിർവചനം std::forward_iterator_tag iterator_category;
		തരനിർവചനം K മൂല്യം_type;
		തരനിർവചനം ptrdiff_t difference_type;
		തരനിർവചനം K* pointer;
		തരനിർവചനം K& reference;
		const_iterator() { }
		const_iterator operator++() { index++; തിരികെ *this; }
		bool operator==(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index == other.index; }
		bool operator!=(സ്ഥിരം const_iterator &other) സ്ഥിരം { തിരികെ index != other.index; }
		സ്ഥിരം K &operator*() സ്ഥിരം { തിരികെ container[index]; }
		സ്ഥിരം K *operator->() സ്ഥിരം { തിരികെ &container[index]; }
	};

	constexpr idict()
	{
	}

	പൂർണ്ണസംഖ്യ operator()(സ്ഥിരം K &താക്കോൽ)
	{
		Hasher::hash_t hash = database.do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = database.do_lookup(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			i = database.do_തിരുകുക(താക്കോൽ, hash);
		തിരികെ i + offset;
	}

	പൂർണ്ണസംഖ്യ at(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = database.do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = database.do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			throw std::out_of_range("idict::at()");
		തിരികെ i + offset;
	}

	പൂർണ്ണസംഖ്യ at(സ്ഥിരം K &താക്കോൽ, പൂർണ്ണസംഖ്യ defval) സ്ഥിരം
	{
		Hasher::hash_t hash = database.do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = database.do_lookup_no_rehash(താക്കോൽ, hash);
		എങ്കിൽ (i < 0)
			തിരികെ defval;
		തിരികെ i + offset;
	}

	പൂർണ്ണസംഖ്യ എണ്ണം(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = database.do_hash(താക്കോൽ);
		പൂർണ്ണസംഖ്യ i = database.do_lookup_no_rehash(താക്കോൽ, hash);
		തിരികെ i < 0 ? 0 : 1;
	}

	പൂർണ്ണസംഖ്യ lookup(സ്ഥിരം K &താക്കോൽ) സ്ഥിരം
	{
		Hasher::hash_t hash = database.do_hash(താക്കോൽ);
		തിരികെ database.do_lookup_no_rehash(താക്കോൽ, hash);
	}

	ശൂന്യം expect(സ്ഥിരം K &താക്കോൽ, പൂർണ്ണസംഖ്യ i)
	{
		പൂർണ്ണസംഖ്യ j = (*this)(താക്കോൽ);
		എങ്കിൽ (i != j)
			throw std::out_of_range("idict::expect()");
	}

	സ്ഥിരം K &operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം
	{
		തിരികെ database.entries.at(index - offset).udata;
	}

	ശൂന്യം swap(idict &other)
	{
		database.swap(other.database);
	}

	ശൂന്യം reserve(size_t n) { database.reserve(n); }
	size_t size() സ്ഥിരം { തിരികെ database.size(); }
	bool empty() സ്ഥിരം { തിരികെ database.empty(); }
	ശൂന്യം clear() { database.clear(); }

	const_iterator begin() സ്ഥിരം { തിരികെ const_iterator(*this, offset); }
	const_iterator element(പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ const_iterator(*this, n); }
	const_iterator end() സ്ഥിരം { തിരികെ const_iterator(*this, offset + size()); }
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
	പൊതു:
		explicit AtomicParent(പൂർണ്ണസംഖ്യ p) : parent(p) {}
		AtomicParent(സ്ഥിരം AtomicParent &other) : parent(other.get()) {}
		AtomicParent &operator=(സ്ഥിരം AtomicParent &other) { set(other.get()); തിരികെ *this; }
		പൂർണ്ണസംഖ്യ get() സ്ഥിരം { തിരികെ parent.load(std::memory_order_relaxed); }
		ശൂന്യം set(പൂർണ്ണസംഖ്യ p) { parent.store(p, std::memory_order_relaxed); }
	സ്വകാര്യ:
		std::atomic<പൂർണ്ണസംഖ്യ> parent;
	};
	std::vector<AtomicParent> parents;

പൊതു:
	തരനിർവചനം typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	പൂർണ്ണസംഖ്യ operator()(സ്ഥിരം K &താക്കോൽ)
	{
		പൂർണ്ണസംഖ്യ i = database(താക്കോൽ);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		എങ്കിൽ (parents.size() < database.size()) {
			parents.emplace_back(-1);
		}
		തിരികെ i;
	}

	// Finds an element at given index
	സ്ഥിരം K &operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം
	{
		തിരികെ database[index];
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
	പൂർണ്ണസംഖ്യ ifind(പൂർണ്ണസംഖ്യ i) സ്ഥിരം
	{
		പൂർണ്ണസംഖ്യ p = i, k = i;

		ആകുമ്പോൾ (true) {
			പൂർണ്ണസംഖ്യ pp = parents[p].get();
			എങ്കിൽ (pp < 0)
				നിർത്തുക;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		ആകുമ്പോൾ (k != p) {
			പൂർണ്ണസംഖ്യ next_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->set(p);
			k = next_k;
		}

		തിരികെ p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	ശൂന്യം imerge(പൂർണ്ണസംഖ്യ i, പൂർണ്ണസംഖ്യ j)
	{
		i = ifind(i);
		j = ifind(j);

		എങ്കിൽ (i != j)
			parents[i].set(j);
	}

	ശൂന്യം ipromote(പൂർണ്ണസംഖ്യ i)
	{
		പൂർണ്ണസംഖ്യ k = i;

		ആകുമ്പോൾ (k != -1) {
			പൂർണ്ണസംഖ്യ next_k = parents[k].get();
			parents[k].set(i);
			k = next_k;
		}

		parents[i].set(-1);
	}

	പൂർണ്ണസംഖ്യ lookup(സ്ഥിരം K &a)
	{
		തിരികെ ifind((*this)(a));
	}

	സ്ഥിരം K &find(സ്ഥിരം K &a) സ്ഥിരം
	{
		പൂർണ്ണസംഖ്യ i = database.at(a, -1);
		എങ്കിൽ (i < 0)
			തിരികെ a;
		തിരികെ (*this)[ifind(i)];
	}

	ശൂന്യം merge(സ്ഥിരം K &a, സ്ഥിരം K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	ശൂന്യം promote(സ്ഥിരം K &a)
	{
		പൂർണ്ണസംഖ്യ i = database.at(a, -1);
		എങ്കിൽ (i >= 0)
			ipromote(i);
	}

	ശൂന്യം swap(mfp &other)
	{
		database.swap(other.database);
		parents.swap(other.parents);
	}

	ശൂന്യം reserve(size_t n) { database.reserve(n); }
	size_t size() സ്ഥിരം { തിരികെ database.size(); }
	bool empty() സ്ഥിരം { തിരികെ database.empty(); }
	ശൂന്യം clear() { database.clear(); parents.clear(); }

	const_iterator begin() സ്ഥിരം { തിരികെ database.begin(); }
	const_iterator element(പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ database.element(n); }
	const_iterator end() സ്ഥിരം { തിരികെ database.end(); }
};

} /* namespace hashlib */

#endif
