// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#यदि_अपरिभाषितम् HASHLIB_H
#परिभाषय HASHLIB_H

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

#परिभाषय YS_HASHING_संस्करणम् 1

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
#  define HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम् __attribute__((warn_अप्रयुक्तम्))
#अन्यथा
#  define HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम्
#यदि_अन्तः

ध्रुव पूर्णाङ्क hashtable_आकार_trigger = 2;
ध्रुव पूर्णाङ्क hashtable_आकार_गुणकः = 3;

namespace legacy {
	अन्तःस्थ uint32_t djb2_सङ्कलय(uint32_t a, uint32_t b) {
		प्रत्यागम ((a << 5) + a) + b;
	}
};

प्रतिरूपम्<typename T>
संरचना hash_ops;

अन्तःस्थ अचिह्नित पूर्णाङ्क mkhash_xorshift(अचिह्नित पूर्णाङ्क a) {
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

	प्रतिरूपम्<typename T>
	शून्य eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	प्रतिरूपम्<typename T>
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
#परिभाषय HASH_शिखरम्_LOOP_FST [[nodiscard]] स्थिर अन्तःस्थ Hasher hash
#परिभाषय HASH_शिखरम्_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	प्रत्यागम h; \
}

प्रतिरूपम्<typename T>
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
			पूर्णाङ्क आकार = a.आकार();
			पूर्णाङ्क i = 0;
			यावत् (i + 8 < आकार) {
				uint64_t v;
				स्मृतिं_प्रतिलिख(&v, a.दत्त() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			स्मृतिं_प्रतिलिख(&v, a.दत्त() + i, आकार - i);
			h.hash64(v);
			प्रत्यागम h;
		} अन्यथा {
			प्रत्यागम a.hash_into(h);
		}
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव T &a) HASH_शिखरम्_LOOP_SND
};

प्रतिरूपम्<typename P, typename Q> संरचना hash_ops<std::pair<P, Q>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::pair<P, Q> &a, ध्रुव std::pair<P, Q> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.first, h);
		h = hash_ops<Q>::hash_into(a.second, h);
		प्रत्यागम h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::pair<P, Q> &a) HASH_शिखरम्_LOOP_SND
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव P &p, ध्रुव Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		प्रत्यागम h;
	}
};

प्रतिरूपम्<typename... T> संरचना hash_ops<std::tuple<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::tuple<T...> &a, ध्रुव std::tuple<T...> &b) {
		प्रत्यागम a == b;
	}
	प्रतिरूपम्<आकार_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I == परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &, Hasher h) {
		प्रत्यागम h;
	}
	प्रतिरूपम्<आकार_t I = 0>
	स्थिर अन्तःस्थ typename std::enable_if<I != परिमाण...(T), Hasher>::type hash_into(ध्रुव std::tuple<T...> &a, Hasher h) {
		प्रकारनिर्देश hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		प्रत्यागम h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::tuple<T...> &a) HASH_शिखरम्_LOOP_SND
};

प्रतिरूपम्<typename T> संरचना hash_ops<std::सदिशम्<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::सदिशम्<T> &a, ध्रुव std::सदिशम्<T> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::सदिशम्<T> &a, Hasher h) {
		h.eat((uint32_t)a.आकार());
		प्रति (स्वचालित k : a)
			h.eat(k);
		प्रत्यागम h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव std::सदिशम्<T> &a) HASH_शिखरम्_LOOP_SND
};

प्रतिरूपम्<typename T, आकार_t N> संरचना hash_ops<std::array<T, N>> {
    स्थिर अन्तःस्थ bool cmp(ध्रुव std::array<T, N> &a, ध्रुव std::array<T, N> &b) {
        प्रत्यागम a == b;
    }
    [[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::array<T, N> &a, Hasher h) {
        प्रति (ध्रुव स्वचालित& k : a)
            h = hash_ops<T>::hash_into(k, h);
        प्रत्यागम h;
    }
	HASH_शिखरम्_LOOP_FST (ध्रुव std::array<T, N> &a) HASH_शिखरम्_LOOP_SND
};

संरचना hash_cstr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
		प्रत्यागम वाक्यं_तुलय(a, b) == 0;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव वर्ण *a, Hasher h) {
		यावत् (*a)
			h.hash32(*(a++));
		प्रत्यागम h;
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव वर्ण *a) HASH_शिखरम्_LOOP_SND
};

प्रतिरूपम् <> संरचना hash_ops<वर्ण*> : hash_cstr_ops {};

संरचना hash_ptr_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव शून्य *a, Hasher h) {
		प्रत्यागम hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_शिखरम्_LOOP_FST (ध्रुव शून्य *a) HASH_शिखरम्_LOOP_SND
};

संरचना hash_obj_ops {
	स्थिर अन्तःस्थ bool cmp(ध्रुव शून्य *a, ध्रुव शून्य *b) {
		प्रत्यागम a == b;
	}
	प्रतिरूपम्<typename T>
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T *a, Hasher h) {
		यदि (a)
			h = a->hash_into(h);
		अन्यथा
			h.eat(0);
		प्रत्यागम h;
	}
	प्रतिरूपम्<typename T>
	HASH_शिखरम्_LOOP_FST (ध्रुव T *a) HASH_शिखरम्_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
प्रतिरूपम्<typename T>
[[nodiscard]]
Hasher::hash_t सततम्_hash(ध्रुव T& obj) {
	प्रत्यागम hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
प्रतिरूपम्<typename T>
[[nodiscard]]
[[deprecated]]
अन्तःस्थ अचिह्नित पूर्णाङ्क mkhash(ध्रुव T &v) {
	प्रत्यागम (अचिह्नित पूर्णाङ्क) सततम्_hash<T>(v);
}

प्रतिरूपम्<> संरचना hash_ops<std::monostate> {
	स्थिर अन्तःस्थ bool cmp(std::monostate a, std::monostate b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(std::monostate, Hasher h) {
		प्रत्यागम h;
	}
};

प्रतिरूपम्<typename... T> संरचना hash_ops<std::variant<T...>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::variant<T...> &a, ध्रुव std::variant<T...> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::variant<T...> &a, Hasher h) {
		std::दर्शय([& h](ध्रुव स्वचालित &v) { h.eat(v); }, a);
		h.eat(a.सूचकांक());
		प्रत्यागम h;
	}
};

प्रतिरूपम्<typename T> संरचना hash_ops<std::optional<T>> {
	स्थिर अन्तःस्थ bool cmp(ध्रुव std::optional<T> &a, ध्रुव std::optional<T> &b) {
		प्रत्यागम a == b;
	}
	[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव std::optional<T> &a, Hasher h) {
		यदि(a.has_मानम्())
			h.eat(*a);
		अन्यथा
			h.eat(0);
		प्रत्यागम h;
	}
};

अन्तःस्थ अचिह्नित पूर्णाङ्क hashtable_आकार(अचिह्नित पूर्णाङ्क min_आकार)
{
	// Primes as generated by https://oeis.org/A175953
	स्थिर std::सदिशम्<अचिह्नित पूर्णाङ्क> zero_and_some_अभाज्याः = {
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

	प्रति (स्वचालित p : zero_and_some_अभाज्याः)
		यदि (p >= min_आकार) प्रत्यागम p;

	यदि (परिमाण(अचिह्नित पूर्णाङ्क) == 4)
		throw std::दीर्घता_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	प्रति (स्वचालित p : zero_and_some_अभाज्याः)
		यदि (100129 * p > min_आकार) प्रत्यागम 100129 * p;

	throw std::दीर्घता_error("hash table exceeded maximum size.");
}

प्रतिरूपम्<typename K, typename T, typename OPS = hash_ops<K>> class dict;
प्रतिरूपम्<typename K, पूर्णाङ्क विस्थापनम् = 0, typename OPS = hash_ops<K>> class idict;
प्रतिरूपम्<typename K, typename OPS = hash_ops<K>> class pool;
प्रतिरूपम्<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
सार्वजनिक:
	commutative_hash() {
		buckets.fill(0);
	}
	प्रतिरूपम् <typename T>
	शून्य eat(ध्रुव T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	शून्य eat(ध्रुव Hasher &h) {
		Hasher::hash_t v = h.yield();
		आकार_t सूचकांक = v & (buckets.आकार() - 1);
		buckets[सूचकांक] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		प्रति (स्वचालित b : buckets)
			h.eat(b);
		प्रत्यागम h;
	}
गुप्त:
	std::array<Hasher::hash_t, 4> buckets;
};

प्रतिरूपम्<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम् dict {
	संरचना प्रविष्टिः_t
	{
		std::pair<K, T> udata;
		पूर्णाङ्क अग्र;

		प्रविष्टिः_t() { }
		प्रविष्टिः_t(ध्रुव std::pair<K, T> &udata, पूर्णाङ्क अग्र) : udata(udata), अग्र(अग्र) { }
		प्रविष्टिः_t(std::pair<K, T> &&udata, पूर्णाङ्क अग्र) : udata(std::move(udata)), अग्र(अग्र) { }
		bool संक्रियकः<(ध्रुव प्रविष्टिः_t &other) ध्रुव { प्रत्यागम udata.first < other.udata.first; }
	};

	std::सदिशम्<पूर्णाङ्क> hashtable;
	std::सदिशम्<प्रविष्टिः_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#यदि_परिभाषितम् NDEBUG
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#यदि_अन्तः

	Hasher::hash_t do_hash(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.रिक्तानि())
			hash = ops.hash(कुञ्चिका).yield() % (अचिह्नित पूर्णाङ्क)(hashtable.आकार());
		प्रत्यागम hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_आकार(entries.क्षमता() * hashtable_आकार_गुणकः), -1);

		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.आकार()); i++) {
			do_assert(-1 <= entries[i].अग्र && entries[i].अग्र < पूर्णाङ्क(entries.आकार()));
			Hasher::hash_t hash = do_hash(entries[i].udata.first);
			entries[i].अग्र = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क सूचकांक, Hasher::hash_t hash)
	{
		do_assert(सूचकांक < पूर्णाङ्क(entries.आकार()));
		यदि (hashtable.रिक्तानि() || सूचकांक < 0)
			प्रत्यागम 0;

		पूर्णाङ्क k = hashtable[hash];
		do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));

		यदि (k == सूचकांक) {
			hashtable[hash] = entries[सूचकांक].अग्र;
		} अन्यथा {
			यावत् (entries[k].अग्र != सूचकांक) {
				k = entries[k].अग्र;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));
			}
			entries[k].अग्र = entries[सूचकांक].अग्र;
		}

		पूर्णाङ्क पृष्ठभागः_idx = entries.आकार()-1;

		यदि (सूचकांक != पृष्ठभागः_idx)
		{
			Hasher::hash_t पृष्ठभागः_hash = do_hash(entries[पृष्ठभागः_idx].udata.first);

			k = hashtable[पृष्ठभागः_hash];
			do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));

			यदि (k == पृष्ठभागः_idx) {
				hashtable[पृष्ठभागः_hash] = सूचकांक;
			} अन्यथा {
				यावत् (entries[k].अग्र != पृष्ठभागः_idx) {
					k = entries[k].अग्र;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));
				}
				entries[k].अग्र = सूचकांक;
			}

			entries[सूचकांक] = std::move(entries[पृष्ठभागः_idx]);
		}

		entries.निष्कासय_पृष्ठभागः();

		यदि (entries.रिक्तानि())
			hashtable.clear();

		प्रत्यागम 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुञ्चिका, Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि())
			प्रत्यागम -1;

		यदि (entries.आकार() * hashtable_आकार_trigger > hashtable.आकार()) {
			do_rehash();
			hash = do_hash(कुञ्चिका);
		}

		प्रत्यागम do_lookup_आन्तरिकम्(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_lookup_आन्तरिकम्(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क सूचकांक = hashtable[hash];

		यावत् (सूचकांक >= 0 && !ops.cmp(entries[सूचकांक].udata.first, कुञ्चिका)) {
			सूचकांक = entries[सूचकांक].अग्र;
			do_assert(-1 <= सूचकांक && सूचकांक < पूर्णाङ्क(entries.आकार()));
		}

		प्रत्यागम सूचकांक;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		यदि (hashtable.रिक्तानि())
			प्रत्यागम -1;

		प्रत्यागम do_lookup_आन्तरिकम्(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_निवेशय(ध्रुव K &कुञ्चिका, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि()) {
			entries.emplace_पृष्ठभागः(std::pair<K, T>(कुञ्चिका, T()), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_पृष्ठभागः(std::pair<K, T>(कुञ्चिका, T()), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		प्रत्यागम entries.आकार() - 1;
	}

	पूर्णाङ्क do_निवेशय(ध्रुव std::pair<K, T> &मानम्, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि()) {
			entries.emplace_पृष्ठभागः(मानम्, -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_पृष्ठभागः(मानम्, hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		प्रत्यागम entries.आकार() - 1;
	}

	पूर्णाङ्क do_निवेशय(std::pair<K, T> &&rvalue, ध्रुव Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि()) {
			entries.emplace_पृष्ठभागः(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} अन्यथा {
			entries.emplace_पृष्ठभागः(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		प्रत्यागम entries.आकार() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्रम् class dict;
	रक्षित:
		ध्रुव dict *ptr;
		पूर्णाङ्क सूचकांक;
		const_iterator(ध्रुव dict *ptr, पूर्णाङ्क सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	सार्वजनिक:
		प्रकारनिर्देश std::bidirectional_iterator_चिह्नकम् iterator_category;
		प्रकारनिर्देश std::pair<K, T> मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश ध्रुव std::pair<K, T>* pointer;
		प्रकारनिर्देश ध्रुव std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator संक्रियकः++() { सूचकांक--; प्रत्यागम *this; }
		const_iterator संक्रियकः++(पूर्णाङ्क) { const_iterator tmp = *this; सूचकांक--; प्रत्यागम tmp; }
		const_iterator संक्रियकः--() { सूचकांक++; प्रत्यागम *this; }
		const_iterator संक्रियकः--(पूर्णाङ्क) { const_iterator tmp = *this; सूचकांक++; प्रत्यागम tmp; }
		const_iterator संक्रियकः+=(पूर्णाङ्क amt) { सूचकांक -= amt; प्रत्यागम *this; }
		bool संक्रियकः<(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक > other.सूचकांक; }
		bool संक्रियकः==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
		bool संक्रियकः!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
		ध्रुव std::pair<K, T> &संक्रियकः*() ध्रुव { प्रत्यागम ptr->entries[सूचकांक].udata; }
		ध्रुव std::pair<K, T> *संक्रियकः->() ध्रुव { प्रत्यागम &ptr->entries[सूचकांक].udata; }
	};

	class iterator
	{
		मित्रम् class dict;
	रक्षित:
		dict *ptr;
		पूर्णाङ्क सूचकांक;
		iterator(dict *ptr, पूर्णाङ्क सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_चिह्नकम् iterator_category;
		प्रकारनिर्देश std::pair<K, T> मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश std::pair<K, T>* pointer;
		प्रकारनिर्देश std::pair<K, T>& reference;
		iterator() { }
		iterator संक्रियकः++() { सूचकांक--; प्रत्यागम *this; }
		iterator संक्रियकः+=(पूर्णाङ्क amt) { सूचकांक -= amt; प्रत्यागम *this; }
		bool संक्रियकः<(ध्रुव iterator &other) ध्रुव { प्रत्यागम सूचकांक > other.सूचकांक; }
		bool संक्रियकः==(ध्रुव iterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
		bool संक्रियकः!=(ध्रुव iterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
		std::pair<K, T> &संक्रियकः*() { प्रत्यागम ptr->entries[सूचकांक].udata; }
		std::pair<K, T> *संक्रियकः->() { प्रत्यागम &ptr->entries[सूचकांक].udata; }
		ध्रुव std::pair<K, T> &संक्रियकः*() ध्रुव { प्रत्यागम ptr->entries[सूचकांक].udata; }
		ध्रुव std::pair<K, T> *संक्रियकः->() ध्रुव { प्रत्यागम &ptr->entries[सूचकांक].udata; }
		संक्रियकः const_iterator() ध्रुव { प्रत्यागम const_iterator(ptr, सूचकांक); }
	};
	using reverse_iterator = std::reverse_iterator<const_iterator>;
	reverse_iterator rbegin() ध्रुव {
		प्रत्यागम std::make_reverse_iterator(अन्तः());
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

	dict &संक्रियकः=(ध्रुव dict &other) {
		entries = other.entries;
		do_rehash();
		प्रत्यागम *this;
	}

	dict &संक्रियकः=(dict &&other) noexcept {
		clear();
		swap(other);
		प्रत्यागम *this;
	}

	dict(ध्रुव std::initializer_list<std::pair<K, T>> &list)
	{
		प्रति (स्वचालित &it : list)
			निवेशय(it);
	}

	प्रतिरूपम्<class निवेशः_Iterator>
	dict(निवेशः_Iterator first, निवेशः_Iterator चरमः)
	{
		निवेशय(first, चरमः);
	}

	प्रतिरूपम्<class निवेशः_Iterator>
	शून्य निवेशय(निवेशः_Iterator first, निवेशः_Iterator चरमः)
	{
		प्रति (; first != चरमः; ++first)
			निवेशय(*first);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(कुञ्चिका, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव std::pair<K, T> &मानम्)
	{
		Hasher::hash_t hash = do_hash(मानम्.first);
		पूर्णाङ्क i = do_lookup(मानम्.first, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(मानम्, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.first);
		पूर्णाङ्क i = do_lookup(rvalue.first, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::forward<std::pair<K, T>>(rvalue), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुञ्चिका, T ध्रुव &मानम्)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(कुञ्चिका, मानम्), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ध्रुव &कुञ्चिका, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(कुञ्चिका, std::forward<T>(rvalue)), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ध्रुव &मानम्)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(std::forward<K>(rkey), मानम्), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		पूर्णाङ्क i = do_lookup(rkey, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	पूर्णाङ्क erase(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क सूचकांक = do_lookup(कुञ्चिका, hash);
		प्रत्यागम do_erase(सूचकांक, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->first);
		do_erase(it.सूचकांक, hash);
		प्रत्यागम ++it;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		प्रत्यागम i < 0 || i > it.सूचकांक ? 0 : 1;
	}

	iterator find(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम अन्तः();
		प्रत्यागम iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम अन्तः();
		प्रत्यागम const_iterator(this, i);
	}

	T& at(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i < 0)
			throw std::बहिर्गमनम्_सम्बन्धि_range("dict::at()");
		प्रत्यागम entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			throw std::बहिर्गमनम्_सम्बन्धि_range("dict::at()");
		प्रत्यागम entries[i].udata.second;
	}

	ध्रुव T& at(ध्रुव K &कुञ्चिका, ध्रुव T &defval) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम defval;
		प्रत्यागम entries[i].udata.second;
	}

	T& संक्रियकः[](ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i < 0)
			i = do_निवेशय(std::pair<K, T>(कुञ्चिका, T()), hash);
		प्रत्यागम entries[i].udata.second;
	}

	प्रतिरूपम्<typename Compare = std::less<K>>
	शून्य क्रमय(Compare comp = Compare())
	{
		std::क्रमय(entries.begin(), entries.अन्तः(), [comp](ध्रुव प्रविष्टिः_t &a, ध्रुव प्रविष्टिः_t &b){ प्रत्यागम comp(b.udata.first, a.udata.first); });
		do_rehash();
	}

	शून्य swap(dict &other)
	{
		hashtable.swap(other.hashtable);
		entries.swap(other.entries);
	}

	bool संक्रियकः==(ध्रुव dict &other) ध्रुव {
		यदि (आकार() != other.आकार())
			प्रत्यागम false;
		प्रति (स्वचालित &it : entries) {
			स्वचालित oit = other.find(it.udata.first);
			यदि (oit == other.अन्तः() || !(oit->second == it.udata.second))
				प्रत्यागम false;
		}
		प्रत्यागम true;
	}

	bool संक्रियकः!=(ध्रुव dict &other) ध्रुव {
		प्रत्यागम !संक्रियकः==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		प्रति (स्वचालित &it : entries) {
			Hasher प्रविष्टिः_hash;
			प्रविष्टिः_hash.eat(it.udata.first);
			प्रविष्टिः_hash.eat(it.udata.second);
			comm.eat(प्रविष्टिः_hash);
		}
		प्रत्यागम comm.hash_into(h);
	}

	शून्य reserve(आकार_t n) { entries.reserve(n); }
	आकार_t आकार() ध्रुव { प्रत्यागम entries.आकार(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम entries.रिक्तानि(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { प्रत्यागम iterator(this, पूर्णाङ्क(entries.आकार())-1); }
	iterator element(पूर्णाङ्क n) { प्रत्यागम iterator(this, पूर्णाङ्क(entries.आकार())-1-n); }
	iterator अन्तः() { प्रत्यागम iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.आकार())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.आकार())-1-n); }
	const_iterator अन्तः() ध्रुव { प्रत्यागम const_iterator(this, -1); }
};

प्रतिरूपम्<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम् pool
{
	प्रतिरूपम्<typename, पूर्णाङ्क, typename> मित्रम् class idict;

रक्षित:
	संरचना प्रविष्टिः_t
	{
		K udata;
		पूर्णाङ्क अग्र;

		प्रविष्टिः_t() { }
		प्रविष्टिः_t(ध्रुव K &udata, पूर्णाङ्क अग्र) : udata(udata), अग्र(अग्र) { }
		प्रविष्टिः_t(K &&udata, पूर्णाङ्क अग्र) : udata(std::move(udata)), अग्र(अग्र) { }
	};

	std::सदिशम्<पूर्णाङ्क> hashtable;
	std::सदिशम्<प्रविष्टिः_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#यदि_परिभाषितम् NDEBUG
	स्थिर अन्तःस्थ शून्य do_assert(bool) { }
#अन्यथा
	स्थिर अन्तःस्थ शून्य do_assert(bool cond) {
		यदि (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#यदि_अन्तः

	Hasher::hash_t do_hash(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = 0;
		यदि (!hashtable.रिक्तानि())
			hash = ops.hash(कुञ्चिका).yield() % (अचिह्नित पूर्णाङ्क)(hashtable.आकार());
		प्रत्यागम hash;
	}

	शून्य do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_आकार(entries.क्षमता() * hashtable_आकार_गुणकः), -1);

		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(entries.आकार()); i++) {
			do_assert(-1 <= entries[i].अग्र && entries[i].अग्र < पूर्णाङ्क(entries.आकार()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].अग्र = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	पूर्णाङ्क do_erase(पूर्णाङ्क सूचकांक, Hasher::hash_t hash)
	{
		do_assert(सूचकांक < पूर्णाङ्क(entries.आकार()));
		यदि (hashtable.रिक्तानि() || सूचकांक < 0)
			प्रत्यागम 0;

		पूर्णाङ्क k = hashtable[hash];
		यदि (k == सूचकांक) {
			hashtable[hash] = entries[सूचकांक].अग्र;
		} अन्यथा {
			यावत् (entries[k].अग्र != सूचकांक) {
				k = entries[k].अग्र;
				do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));
			}
			entries[k].अग्र = entries[सूचकांक].अग्र;
		}

		पूर्णाङ्क पृष्ठभागः_idx = entries.आकार()-1;

		यदि (सूचकांक != पृष्ठभागः_idx)
		{
			Hasher::hash_t पृष्ठभागः_hash = do_hash(entries[पृष्ठभागः_idx].udata);

			k = hashtable[पृष्ठभागः_hash];
			यदि (k == पृष्ठभागः_idx) {
				hashtable[पृष्ठभागः_hash] = सूचकांक;
			} अन्यथा {
				यावत् (entries[k].अग्र != पृष्ठभागः_idx) {
					k = entries[k].अग्र;
					do_assert(0 <= k && k < पूर्णाङ्क(entries.आकार()));
				}
				entries[k].अग्र = सूचकांक;
			}

			entries[सूचकांक] = std::move(entries[पृष्ठभागः_idx]);
		}

		entries.निष्कासय_पृष्ठभागः();

		यदि (entries.रिक्तानि())
			hashtable.clear();

		प्रत्यागम 1;
	}

	पूर्णाङ्क do_lookup(ध्रुव K &कुञ्चिका, Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि())
			प्रत्यागम -1;

		यदि (entries.आकार() * hashtable_आकार_trigger > hashtable.आकार()) {
			do_rehash();
			hash = do_hash(कुञ्चिका);
		}

		प्रत्यागम do_lookup_आन्तरिकम्(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_lookup_आन्तरिकम्(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		पूर्णाङ्क सूचकांक = hashtable[hash];

		यावत् (सूचकांक >= 0 && !ops.cmp(entries[सूचकांक].udata, कुञ्चिका)) {
			सूचकांक = entries[सूचकांक].अग्र;
			do_assert(-1 <= सूचकांक && सूचकांक < पूर्णाङ्क(entries.आकार()));
		}

		प्रत्यागम सूचकांक;
	}

	पूर्णाङ्क do_lookup_no_rehash(ध्रुव K &कुञ्चिका, Hasher::hash_t hash) ध्रुव
	{
		यदि (hashtable.रिक्तानि())
			प्रत्यागम -1;

		प्रत्यागम do_lookup_आन्तरिकम्(कुञ्चिका, hash);
	}

	पूर्णाङ्क do_निवेशय(ध्रुव K &मानम्, Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि()) {
			entries.emplace_पृष्ठभागः(मानम्, -1);
			do_rehash();
			hash = do_hash(मानम्);
		} अन्यथा {
			entries.emplace_पृष्ठभागः(मानम्, hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		प्रत्यागम entries.आकार() - 1;
	}

	पूर्णाङ्क do_निवेशय(K &&rvalue, Hasher::hash_t &hash)
	{
		यदि (hashtable.रिक्तानि()) {
			entries.emplace_पृष्ठभागः(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.पृष्ठभागः().udata);
		} अन्यथा {
			entries.emplace_पृष्ठभागः(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.आकार() - 1;
		}
		प्रत्यागम entries.आकार() - 1;
	}

सार्वजनिक:
	class const_iterator
	{
		मित्रम् class pool;
	रक्षित:
		ध्रुव pool *ptr;
		पूर्णाङ्क सूचकांक;
		const_iterator(ध्रुव pool *ptr, पूर्णाङ्क सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_चिह्नकम् iterator_category;
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator संक्रियकः++() { सूचकांक--; प्रत्यागम *this; }
		bool संक्रियकः==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
		bool संक्रियकः!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
		ध्रुव K &संक्रियकः*() ध्रुव { प्रत्यागम ptr->entries[सूचकांक].udata; }
		ध्रुव K *संक्रियकः->() ध्रुव { प्रत्यागम &ptr->entries[सूचकांक].udata; }
	};

	class iterator
	{
		मित्रम् class pool;
	रक्षित:
		pool *ptr;
		पूर्णाङ्क सूचकांक;
		iterator(pool *ptr, पूर्णाङ्क सूचकांक) : ptr(ptr), सूचकांक(सूचकांक) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_चिह्नकम् iterator_category;
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		iterator() { }
		iterator संक्रियकः++() { सूचकांक--; प्रत्यागम *this; }
		bool संक्रियकः==(ध्रुव iterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
		bool संक्रियकः!=(ध्रुव iterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
		K &संक्रियकः*() { प्रत्यागम ptr->entries[सूचकांक].udata; }
		K *संक्रियकः->() { प्रत्यागम &ptr->entries[सूचकांक].udata; }
		ध्रुव K &संक्रियकः*() ध्रुव { प्रत्यागम ptr->entries[सूचकांक].udata; }
		ध्रुव K *संक्रियकः->() ध्रुव { प्रत्यागम &ptr->entries[सूचकांक].udata; }
		संक्रियकः const_iterator() ध्रुव { प्रत्यागम const_iterator(ptr, सूचकांक); }
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

	pool &संक्रियकः=(ध्रुव pool &other) {
		entries = other.entries;
		do_rehash();
		प्रत्यागम *this;
	}

	pool &संक्रियकः=(pool &&other) noexcept {
		clear();
		swap(other);
		प्रत्यागम *this;
	}

	pool(ध्रुव std::initializer_list<K> &list)
	{
		प्रति (स्वचालित &it : list)
			निवेशय(it);
	}

	प्रतिरूपम्<class निवेशः_Iterator>
	pool(निवेशः_Iterator first, निवेशः_Iterator चरमः)
	{
		निवेशय(first, चरमः);
	}

	प्रतिरूपम्<class निवेशः_Iterator>
	शून्य निवेशय(निवेशः_Iterator first, निवेशः_Iterator चरमः)
	{
		प्रति (; first != चरमः; ++first)
			निवेशय(*first);
	}

	std::pair<iterator, bool> निवेशय(ध्रुव K &मानम्)
	{
		Hasher::hash_t hash = do_hash(मानम्);
		पूर्णाङ्क i = do_lookup(मानम्, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(मानम्, hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> निवेशय(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		पूर्णाङ्क i = do_lookup(rvalue, hash);
		यदि (i >= 0)
			प्रत्यागम std::pair<iterator, bool>(iterator(this, i), false);
		i = do_निवेशय(std::forward<K>(rvalue), hash);
		प्रत्यागम std::pair<iterator, bool>(iterator(this, i), true);
	}

	प्रतिरूपम्<typename... तर्काः_3>
	std::pair<iterator, bool> emplace(तर्काः_3&&... तर्काः_2)
	{
		प्रत्यागम निवेशय(K(std::forward<तर्काः_3>(तर्काः_2)...));
	}

	पूर्णाङ्क erase(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क सूचकांक = do_lookup(कुञ्चिका, hash);
		प्रत्यागम do_erase(सूचकांक, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.सूचकांक, hash);
		प्रत्यागम ++it;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका, const_iterator it) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		प्रत्यागम i < 0 || i > it.सूचकांक ? 0 : 1;
	}

	iterator find(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम अन्तः();
		प्रत्यागम iterator(this, i);
	}

	const_iterator find(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम अन्तः();
		प्रत्यागम const_iterator(this, i);
	}

	bool संक्रियकः[](ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = do_hash(कुञ्चिका);
		पूर्णाङ्क i = do_lookup(कुञ्चिका, hash);
		प्रत्यागम i >= 0;
	}

	प्रतिरूपम्<typename Compare = std::less<K>>
	शून्य क्रमय(Compare comp = Compare())
	{
		std::क्रमय(entries.begin(), entries.अन्तः(), [comp](ध्रुव प्रविष्टिः_t &a, ध्रुव प्रविष्टिः_t &b){ प्रत्यागम comp(b.udata, a.udata); });
		do_rehash();
	}

	K निष्कासय()
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

	bool संक्रियकः==(ध्रुव pool &other) ध्रुव {
		यदि (आकार() != other.आकार())
			प्रत्यागम false;
		प्रति (स्वचालित &it : entries)
			यदि (!other.गणनम्(it.udata))
				प्रत्यागम false;
		प्रत्यागम true;
	}

	bool संक्रियकः!=(ध्रुव pool &other) ध्रुव {
		प्रत्यागम !संक्रियकः==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		commutative_hash comm;
		प्रति (स्वचालित &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		प्रत्यागम comm.hash_into(h);
	}

	शून्य reserve(आकार_t n) { entries.reserve(n); }
	आकार_t आकार() ध्रुव { प्रत्यागम entries.आकार(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम entries.रिक्तानि(); }
	शून्य clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { प्रत्यागम iterator(this, पूर्णाङ्क(entries.आकार())-1); }
	iterator element(पूर्णाङ्क n) { प्रत्यागम iterator(this, पूर्णाङ्क(entries.आकार())-1-n); }
	iterator अन्तः() { प्रत्यागम iterator(nullptr, -1); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.आकार())-1); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(this, पूर्णाङ्क(entries.आकार())-1-n); }
	const_iterator अन्तः() ध्रुव { प्रत्यागम const_iterator(nullptr, -1); }
};

प्रतिरूपम्<typename K, पूर्णाङ्क विस्थापनम्, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम् idict
{
	pool<K, OPS> database;

सार्वजनिक:
	class const_iterator
	{
		मित्रम् class idict;
	रक्षित:
		ध्रुव idict &container;
		पूर्णाङ्क सूचकांक;
		const_iterator(ध्रुव idict &container, पूर्णाङ्क सूचकांक) : container(container), सूचकांक(सूचकांक) { }
	सार्वजनिक:
		प्रकारनिर्देश std::forward_iterator_चिह्नकम् iterator_category;
		प्रकारनिर्देश K मानम्_type;
		प्रकारनिर्देश ptrdiff_t difference_type;
		प्रकारनिर्देश K* pointer;
		प्रकारनिर्देश K& reference;
		const_iterator() { }
		const_iterator संक्रियकः++() { सूचकांक++; प्रत्यागम *this; }
		bool संक्रियकः==(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
		bool संक्रियकः!=(ध्रुव const_iterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
		ध्रुव K &संक्रियकः*() ध्रुव { प्रत्यागम container[सूचकांक]; }
		ध्रुव K *संक्रियकः->() ध्रुव { प्रत्यागम &container[सूचकांक]; }
	};

	constexpr idict()
	{
	}

	पूर्णाङ्क संक्रियकः()(ध्रुव K &कुञ्चिका)
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup(कुञ्चिका, hash);
		यदि (i < 0)
			i = database.do_निवेशय(कुञ्चिका, hash);
		प्रत्यागम i + विस्थापनम्;
	}

	पूर्णाङ्क at(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			throw std::बहिर्गमनम्_सम्बन्धि_range("idict::at()");
		प्रत्यागम i + विस्थापनम्;
	}

	पूर्णाङ्क at(ध्रुव K &कुञ्चिका, पूर्णाङ्क defval) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		यदि (i < 0)
			प्रत्यागम defval;
		प्रत्यागम i + विस्थापनम्;
	}

	पूर्णाङ्क गणनम्(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		पूर्णाङ्क i = database.do_lookup_no_rehash(कुञ्चिका, hash);
		प्रत्यागम i < 0 ? 0 : 1;
	}

	पूर्णाङ्क lookup(ध्रुव K &कुञ्चिका) ध्रुव
	{
		Hasher::hash_t hash = database.do_hash(कुञ्चिका);
		प्रत्यागम database.do_lookup_no_rehash(कुञ्चिका, hash);
	}

	शून्य expect(ध्रुव K &कुञ्चिका, पूर्णाङ्क i)
	{
		पूर्णाङ्क j = (*this)(कुञ्चिका);
		यदि (i != j)
			throw std::बहिर्गमनम्_सम्बन्धि_range("idict::expect()");
	}

	ध्रुव K &संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव
	{
		प्रत्यागम database.entries.at(सूचकांक - विस्थापनम्).udata;
	}

	शून्य swap(idict &other)
	{
		database.swap(other.database);
	}

	शून्य reserve(आकार_t n) { database.reserve(n); }
	आकार_t आकार() ध्रुव { प्रत्यागम database.आकार(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम database.रिक्तानि(); }
	शून्य clear() { database.clear(); }

	const_iterator begin() ध्रुव { प्रत्यागम const_iterator(*this, विस्थापनम्); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम const_iterator(*this, n); }
	const_iterator अन्तः() ध्रुव { प्रत्यागम const_iterator(*this, विस्थापनम् + आकार()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
प्रतिरूपम्<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_अप्रयुक्तम् mfp
{
	idict<K, 0, OPS> database;
	class AtomicParent {
	सार्वजनिक:
		explicit AtomicParent(पूर्णाङ्क p) : parent(p) {}
		AtomicParent(ध्रुव AtomicParent &other) : parent(other.get()) {}
		AtomicParent &संक्रियकः=(ध्रुव AtomicParent &other) { समुच्चयः(other.get()); प्रत्यागम *this; }
		पूर्णाङ्क get() ध्रुव { प्रत्यागम parent.आरोपय(std::स्मृतिः_क्रमः_relaxed); }
		शून्य समुच्चयः(पूर्णाङ्क p) { parent.store(p, std::स्मृतिः_क्रमः_relaxed); }
	गुप्त:
		std::atomic<पूर्णाङ्क> parent;
	};
	std::सदिशम्<AtomicParent> parents;

सार्वजनिक:
	प्रकारनिर्देश typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	पूर्णाङ्क संक्रियकः()(ध्रुव K &कुञ्चिका)
	{
		पूर्णाङ्क i = database(कुञ्चिका);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		यदि (parents.आकार() < database.आकार()) {
			parents.emplace_पृष्ठभागः(-1);
		}
		प्रत्यागम i;
	}

	// Finds an element at given index
	ध्रुव K &संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव
	{
		प्रत्यागम database[सूचकांक];
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
			पूर्णाङ्क अग्र_k = parents[k].get();
			const_cast<AtomicParent*>(&parents[k])->समुच्चयः(p);
			k = अग्र_k;
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
			parents[i].समुच्चयः(j);
	}

	शून्य ipromote(पूर्णाङ्क i)
	{
		पूर्णाङ्क k = i;

		यावत् (k != -1) {
			पूर्णाङ्क अग्र_k = parents[k].get();
			parents[k].समुच्चयः(i);
			k = अग्र_k;
		}

		parents[i].समुच्चयः(-1);
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

	शून्य reserve(आकार_t n) { database.reserve(n); }
	आकार_t आकार() ध्रुव { प्रत्यागम database.आकार(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम database.रिक्तानि(); }
	शून्य clear() { database.clear(); parents.clear(); }

	const_iterator begin() ध्रुव { प्रत्यागम database.begin(); }
	const_iterator element(पूर्णाङ्क n) ध्रुव { प्रत्यागम database.element(n); }
	const_iterator अन्तः() ध्रुव { प्रत्यागम database.अन्तः(); }
};

} /* namespace hashlib */

#यदि_अन्तः
