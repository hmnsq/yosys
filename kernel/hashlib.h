// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#جيڪڏهن_وصف_نه_ٿيل HASHLIB_H
#وصف_ڪريو HASHLIB_H

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

#وصف_ڪريو YS_HASHING_نسخو 1

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

#إذا defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل __attribute__((warn_اڻ_استعمال_ٿيل))
#وإلا
#  define HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

ثابت صحيح hashtable_سائيز_trigger = 2;
ثابت صحيح hashtable_سائيز_ضرب_ڪندڙ = 3;

namespace legacy {
	مضمن uint32_t djb2_جوڙيو(uint32_t a, uint32_t b) {
		أرجع ((a << 5) + a) + b;
	}
};

سانچو<typename T>
بنية hash_ops;

مضمن بنانشان صحيح mkhash_xorshift(بنانشان صحيح a) {
	إذا (حجم(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} وإلا إذا (حجم(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} وإلا
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	أرجع a;
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
		أرجع hash;
	}
عام:
	فراغ hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		أرجع;
	}
	فراغ hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		أرجع;
	}
	[[nodiscard]]
	hash_t yield() ثابت {
		أرجع (hash_t)state;
	}

	سانچو<typename T>
	فراغ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	سانچو<typename T>
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
#وصف_ڪريو HASH_چوٽي_LOOP_FST [[nodiscard]] ساكن مضمن Hasher hash
#وصف_ڪريو HASH_چوٽي_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	أرجع h; \
}

سانچو<typename T>
بنية hash_ops {
	ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
		إذا constexpr (std::is_integral_v<T>) {
			static_assert(حجم(T) <= حجم(uint64_t));
			إذا (حجم(T) == حجم(uint64_t))
				h.hash64(a);
			وإلا
				h.hash32(a);
			أرجع h;
		} وإلا إذا constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			أرجع hash_ops<u_type>::hash_into((u_type) a, h);
		} وإلا إذا constexpr (std::is_اشارو_v<T>) {
			أرجع hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} وإلا إذا constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحيح سائيز = a.سائيز();
			صحيح i = 0;
			طالما (i + 8 < سائيز) {
				uint64_t v;
				يادگيري_نقل(&v, a.معلومات() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			يادگيري_نقل(&v, a.معلومات() + i, سائيز - i);
			h.hash64(v);
			أرجع h;
		} وإلا {
			أرجع a.hash_into(h);
		}
	}
	HASH_چوٽي_LOOP_FST (ثابت T &a) HASH_چوٽي_LOOP_SND
};

سانچو<typename P, typename Q> بنية hash_ops<std::pair<P, Q>> {
	ساكن مضمن bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.پهريون, h);
		h = hash_ops<Q>::hash_into(a.ٻيو, h);
		أرجع h;
	}
	HASH_چوٽي_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_چوٽي_LOOP_SND
	[[nodiscard]] ساكن مضمن Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		أرجع h;
	}
};

سانچو<typename... T> بنية hash_ops<std::tuple<T...>> {
	ساكن مضمن bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		أرجع a == b;
	}
	سانچو<سائيز_t I = 0>
	ساكن مضمن typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		أرجع h;
	}
	سانچو<سائيز_t I = 0>
	ساكن مضمن typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعريفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		أرجع h;
	}
	HASH_چوٽي_LOOP_FST (ثابت std::tuple<T...> &a) HASH_چوٽي_LOOP_SND
};

سانچو<typename T> بنية hash_ops<std::ويڪٽر<T>> {
	ساكن مضمن bool cmp(ثابت std::ويڪٽر<T> &a, ثابت std::ويڪٽر<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::ويڪٽر<T> &a, Hasher h) {
		h.eat((uint32_t)a.سائيز());
		لكل (خودڪار k : a)
			h.eat(k);
		أرجع h;
	}
	HASH_چوٽي_LOOP_FST (ثابت std::ويڪٽر<T> &a) HASH_چوٽي_LOOP_SND
};

سانچو<typename T, سائيز_t N> بنية hash_ops<std::array<T, N>> {
    ساكن مضمن bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        أرجع a == b;
    }
    [[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        لكل (ثابت خودڪار& k : a)
            h = hash_ops<T>::hash_into(k, h);
        أرجع h;
    }
	HASH_چوٽي_LOOP_FST (ثابت std::array<T, N> &a) HASH_چوٽي_LOOP_SND
};

بنية hash_cstr_ops {
	ساكن مضمن bool cmp(ثابت حرف *a, ثابت حرف *b) {
		أرجع strcmp(a, b) == 0;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت حرف *a, Hasher h) {
		طالما (*a)
			h.hash32(*(a++));
		أرجع h;
	}
	HASH_چوٽي_LOOP_FST (ثابت حرف *a) HASH_چوٽي_LOOP_SND
};

سانچو <> بنية hash_ops<حرف*> : hash_cstr_ops {};

بنية hash_ptr_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت فراغ *a, Hasher h) {
		أرجع hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_چوٽي_LOOP_FST (ثابت فراغ *a) HASH_چوٽي_LOOP_SND
};

بنية hash_obj_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	سانچو<typename T>
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T *a, Hasher h) {
		إذا (a)
			h = a->hash_into(h);
		وإلا
			h.eat(0);
		أرجع h;
	}
	سانچو<typename T>
	HASH_چوٽي_LOOP_FST (ثابت T *a) HASH_چوٽي_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
سانچو<typename T>
[[nodiscard]]
Hasher::hash_t لاڳيتو_hash(ثابت T& obj) {
	أرجع hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
سانچو<typename T>
[[nodiscard]]
[[deprecated]]
مضمن بنانشان صحيح mkhash(ثابت T &v) {
	أرجع (بنانشان صحيح) لاڳيتو_hash<T>(v);
}

سانچو<> بنية hash_ops<std::monostate> {
	ساكن مضمن bool cmp(std::monostate a, std::monostate b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(std::monostate, Hasher h) {
		أرجع h;
	}
};

سانچو<typename... T> بنية hash_ops<std::variant<T...>> {
	ساكن مضمن bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::گهمو([& h](ثابت خودڪار &v) { h.eat(v); }, a);
		h.eat(a.اشاريو());
		أرجع h;
	}
};

سانچو<typename T> بنية hash_ops<std::optional<T>> {
	ساكن مضمن bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		إذا(a.has_قدر())
			h.eat(*a);
		وإلا
			h.eat(0);
		أرجع h;
	}
};

مضمن بنانشان صحيح hashtable_سائيز(بنانشان صحيح min_سائيز)
{
	// Primes as generated by https://oeis.org/A175953
	ساكن std::ويڪٽر<بنانشان صحيح> zero_and_some_مفرد_انگ = {
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

	لكل (خودڪار p : zero_and_some_مفرد_انگ)
		إذا (p >= min_سائيز) أرجع p;

	إذا (حجم(بنانشان صحيح) == 4)
		throw std::ڊيگهائي_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	لكل (خودڪار p : zero_and_some_مفرد_انگ)
		إذا (100129 * p > min_سائيز) أرجع 100129 * p;

	throw std::ڊيگهائي_error("hash table exceeded maximum size.");
}

سانچو<typename K, typename T, typename OPS = hash_ops<K>> class dict;
سانچو<typename K, صحيح وٿي = 0, typename OPS = hash_ops<K>> class idict;
سانچو<typename K, typename OPS = hash_ops<K>> class pool;
سانچو<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عام:
	commutative_hash() {
		buckets.fill(0);
	}
	سانچو <typename T>
	فراغ eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	فراغ eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		سائيز_t اشاريو = v & (buckets.سائيز() - 1);
		buckets[اشاريو] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		لكل (خودڪار b : buckets)
			h.eat(b);
		أرجع h;
	}
خاص:
	std::array<Hasher::hash_t, 4> buckets;
};

سانچو<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل dict {
	بنية داخلا_t
	{
		std::pair<K, T> udata;
		صحيح ايندڙ;

		داخلا_t() { }
		داخلا_t(ثابت std::pair<K, T> &udata, صحيح ايندڙ) : udata(udata), ايندڙ(ايندڙ) { }
		داخلا_t(std::pair<K, T> &&udata, صحيح ايندڙ) : udata(std::move(udata)), ايندڙ(ايندڙ) { }
		bool عامل<(ثابت داخلا_t &other) ثابت { أرجع udata.پهريون < other.udata.پهريون; }
	};

	std::ويڪٽر<صحيح> hashtable;
	std::ويڪٽر<داخلا_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#جيڪڏهن_وصف_ٿيل NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#جيڪڏهن_پورو

	Hasher::hash_t do_hash(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالي_جايون())
			hash = ops.hash(ڪنجي).yield() % (بنانشان صحيح)(hashtable.سائيز());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_سائيز(entries.صلاحيت() * hashtable_سائيز_ضرب_ڪندڙ), -1);

		لكل (صحيح i = 0; i < صحيح(entries.سائيز()); i++) {
			do_assert(-1 <= entries[i].ايندڙ && entries[i].ايندڙ < صحيح(entries.سائيز()));
			Hasher::hash_t hash = do_hash(entries[i].udata.پهريون);
			entries[i].ايندڙ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح اشاريو, Hasher::hash_t hash)
	{
		do_assert(اشاريو < صحيح(entries.سائيز()));
		إذا (hashtable.خالي_جايون() || اشاريو < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		do_assert(0 <= k && k < صحيح(entries.سائيز()));

		إذا (k == اشاريو) {
			hashtable[hash] = entries[اشاريو].ايندڙ;
		} وإلا {
			طالما (entries[k].ايندڙ != اشاريو) {
				k = entries[k].ايندڙ;
				do_assert(0 <= k && k < صحيح(entries.سائيز()));
			}
			entries[k].ايندڙ = entries[اشاريو].ايندڙ;
		}

		صحيح پٺ_idx = entries.سائيز()-1;

		إذا (اشاريو != پٺ_idx)
		{
			Hasher::hash_t پٺ_hash = do_hash(entries[پٺ_idx].udata.پهريون);

			k = hashtable[پٺ_hash];
			do_assert(0 <= k && k < صحيح(entries.سائيز()));

			إذا (k == پٺ_idx) {
				hashtable[پٺ_hash] = اشاريو;
			} وإلا {
				طالما (entries[k].ايندڙ != پٺ_idx) {
					k = entries[k].ايندڙ;
					do_assert(0 <= k && k < صحيح(entries.سائيز()));
				}
				entries[k].ايندڙ = اشاريو;
			}

			entries[اشاريو] = std::move(entries[پٺ_idx]);
		}

		entries.ڪڍو_پٺ();

		إذا (entries.خالي_جايون())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &ڪنجي, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون())
			أرجع -1;

		إذا (entries.سائيز() * hashtable_سائيز_trigger > hashtable.سائيز()) {
			do_rehash();
			hash = do_hash(ڪنجي);
		}

		أرجع do_lookup_اندروني(ڪنجي, hash);
	}

	صحيح do_lookup_اندروني(ثابت K &ڪنجي, Hasher::hash_t hash) ثابت
	{
		صحيح اشاريو = hashtable[hash];

		طالما (اشاريو >= 0 && !ops.cmp(entries[اشاريو].udata.پهريون, ڪنجي)) {
			اشاريو = entries[اشاريو].ايندڙ;
			do_assert(-1 <= اشاريو && اشاريو < صحيح(entries.سائيز()));
		}

		أرجع اشاريو;
	}

	صحيح do_lookup_no_rehash(ثابت K &ڪنجي, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالي_جايون())
			أرجع -1;

		أرجع do_lookup_اندروني(ڪنجي, hash);
	}

	صحيح do_وجهو(ثابت K &ڪنجي, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون()) {
			entries.emplace_پٺ(std::pair<K, T>(ڪنجي, T()), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٺ(std::pair<K, T>(ڪنجي, T()), hashtable[hash]);
			hashtable[hash] = entries.سائيز() - 1;
		}
		أرجع entries.سائيز() - 1;
	}

	صحيح do_وجهو(ثابت std::pair<K, T> &قدر, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون()) {
			entries.emplace_پٺ(قدر, -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٺ(قدر, hashtable[hash]);
			hashtable[hash] = entries.سائيز() - 1;
		}
		أرجع entries.سائيز() - 1;
	}

	صحيح do_وجهو(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون()) {
			entries.emplace_پٺ(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٺ(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.سائيز() - 1;
		}
		أرجع entries.سائيز() - 1;
	}

عام:
	class const_iterator
	{
		دوست class dict;
	محمي:
		ثابت dict *ptr;
		صحيح اشاريو;
		const_iterator(ثابت dict *ptr, صحيح اشاريو) : ptr(ptr), اشاريو(اشاريو) { }
	عام:
		تعريفنوع std::bidirectional_iterator_ٽيگ iterator_category;
		تعريفنوع std::pair<K, T> قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع ثابت std::pair<K, T>* اشارو;
		تعريفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاريو--; أرجع *this; }
		const_iterator عامل++(صحيح) { const_iterator tmp = *this; اشاريو--; أرجع tmp; }
		const_iterator عامل--() { اشاريو++; أرجع *this; }
		const_iterator عامل--(صحيح) { const_iterator tmp = *this; اشاريو++; أرجع tmp; }
		const_iterator عامل+=(صحيح amt) { اشاريو -= amt; أرجع *this; }
		bool عامل<(ثابت const_iterator &other) ثابت { أرجع اشاريو > other.اشاريو; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
		ثابت std::pair<K, T> &عامل*() ثابت { أرجع ptr->entries[اشاريو].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { أرجع &ptr->entries[اشاريو].udata; }
	};

	class iterator
	{
		دوست class dict;
	محمي:
		dict *ptr;
		صحيح اشاريو;
		iterator(dict *ptr, صحيح اشاريو) : ptr(ptr), اشاريو(اشاريو) { }
	عام:
		تعريفنوع std::forward_iterator_ٽيگ iterator_category;
		تعريفنوع std::pair<K, T> قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع std::pair<K, T>* اشارو;
		تعريفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator عامل++() { اشاريو--; أرجع *this; }
		iterator عامل+=(صحيح amt) { اشاريو -= amt; أرجع *this; }
		bool عامل<(ثابت iterator &other) ثابت { أرجع اشاريو > other.اشاريو; }
		bool عامل==(ثابت iterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
		bool عامل!=(ثابت iterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
		std::pair<K, T> &عامل*() { أرجع ptr->entries[اشاريو].udata; }
		std::pair<K, T> *عامل->() { أرجع &ptr->entries[اشاريو].udata; }
		ثابت std::pair<K, T> &عامل*() ثابت { أرجع ptr->entries[اشاريو].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { أرجع &ptr->entries[اشاريو].udata; }
		عامل const_iterator() ثابت { أرجع const_iterator(ptr, اشاريو); }
	};
	using ابتو_iterator = std::ابتو_iterator<const_iterator>;
	ابتو_iterator rbegin() ثابت {
		أرجع std::make_ابتو_iterator(پڇاڙي());
	}
	ابتو_iterator rend() ثابت {
		أرجع std::make_ابتو_iterator(begin());
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
		مٽا_سٽا(other);
	}

	dict &عامل=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	dict &عامل=(dict &&other) noexcept {
		clear();
		مٽا_سٽا(other);
		أرجع *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		لكل (خودڪار &it : list)
			وجهو(it);
	}

	سانچو<class داخلا_Iterator>
	dict(داخلا_Iterator پهريون, داخلا_Iterator آخري)
	{
		وجهو(پهريون, آخري);
	}

	سانچو<class داخلا_Iterator>
	فراغ وجهو(داخلا_Iterator پهريون, داخلا_Iterator آخري)
	{
		لكل (; پهريون != آخري; ++پهريون)
			وجهو(*پهريون);
	}

	std::pair<iterator, bool> وجهو(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(ڪنجي, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> وجهو(ثابت std::pair<K, T> &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر.پهريون);
		صحيح i = do_lookup(قدر.پهريون, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(قدر, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> وجهو(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.پهريون);
		صحيح i = do_lookup(rvalue.پهريون, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::forward<std::pair<K, T>>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &ڪنجي, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::make_pair(ڪنجي, قدر), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &ڪنجي, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::make_pair(ڪنجي, std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::make_pair(std::forward<K>(rkey), قدر), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحيح erase(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح اشاريو = do_lookup(ڪنجي, hash);
		أرجع do_erase(اشاريو, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->پهريون);
		do_erase(it.اشاريو, hash);
		أرجع ++it;
	}

	صحيح ڳڻپ(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح ڳڻپ(ثابت K &ڪنجي, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		أرجع i < 0 || i > it.اشاريو ? 0 : 1;
	}

	iterator ڳوليو(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i < 0)
			أرجع پڇاڙي();
		أرجع iterator(this, i);
	}

	const_iterator ڳوليو(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			أرجع پڇاڙي();
		أرجع const_iterator(this, i);
	}

	T& at(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i < 0)
			throw std::ٻاهريون_جو_range("dict::at()");
		أرجع entries[i].udata.ٻيو;
	}

	ثابت T& at(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			throw std::ٻاهريون_جو_range("dict::at()");
		أرجع entries[i].udata.ٻيو;
	}

	ثابت T& at(ثابت K &ڪنجي, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع entries[i].udata.ٻيو;
	}

	T& عامل[](ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i < 0)
			i = do_وجهو(std::pair<K, T>(ڪنجي, T()), hash);
		أرجع entries[i].udata.ٻيو;
	}

	سانچو<typename Compare = std::less<K>>
	فراغ ترتيب_ڏيو(Compare comp = Compare())
	{
		std::ترتيب_ڏيو(entries.begin(), entries.پڇاڙي(), [comp](ثابت داخلا_t &a, ثابت داخلا_t &b){ أرجع comp(b.udata.پهريون, a.udata.پهريون); });
		do_rehash();
	}

	فراغ مٽا_سٽا(dict &other)
	{
		hashtable.مٽا_سٽا(other.hashtable);
		entries.مٽا_سٽا(other.entries);
	}

	bool عامل==(ثابت dict &other) ثابت {
		إذا (سائيز() != other.سائيز())
			أرجع false;
		لكل (خودڪار &it : entries) {
			خودڪار oit = other.ڳوليو(it.udata.پهريون);
			إذا (oit == other.پڇاڙي() || !(oit->ٻيو == it.udata.ٻيو))
				أرجع false;
		}
		أرجع true;
	}

	bool عامل!=(ثابت dict &other) ثابت {
		أرجع !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودڪار &it : entries) {
			Hasher داخلا_hash;
			داخلا_hash.eat(it.udata.پهريون);
			داخلا_hash.eat(it.udata.ٻيو);
			comm.eat(داخلا_hash);
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(سائيز_t n) { entries.reserve(n); }
	سائيز_t سائيز() ثابت { أرجع entries.سائيز(); }
	bool خالي_جايون() ثابت { أرجع entries.خالي_جايون(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.سائيز())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.سائيز())-1-n); }
	iterator پڇاڙي() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.سائيز())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.سائيز())-1-n); }
	const_iterator پڇاڙي() ثابت { أرجع const_iterator(this, -1); }
};

سانچو<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل pool
{
	سانچو<typename, صحيح, typename> دوست class idict;

محمي:
	بنية داخلا_t
	{
		K udata;
		صحيح ايندڙ;

		داخلا_t() { }
		داخلا_t(ثابت K &udata, صحيح ايندڙ) : udata(udata), ايندڙ(ايندڙ) { }
		داخلا_t(K &&udata, صحيح ايندڙ) : udata(std::move(udata)), ايندڙ(ايندڙ) { }
	};

	std::ويڪٽر<صحيح> hashtable;
	std::ويڪٽر<داخلا_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#جيڪڏهن_وصف_ٿيل NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#جيڪڏهن_پورو

	Hasher::hash_t do_hash(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالي_جايون())
			hash = ops.hash(ڪنجي).yield() % (بنانشان صحيح)(hashtable.سائيز());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_سائيز(entries.صلاحيت() * hashtable_سائيز_ضرب_ڪندڙ), -1);

		لكل (صحيح i = 0; i < صحيح(entries.سائيز()); i++) {
			do_assert(-1 <= entries[i].ايندڙ && entries[i].ايندڙ < صحيح(entries.سائيز()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].ايندڙ = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح اشاريو, Hasher::hash_t hash)
	{
		do_assert(اشاريو < صحيح(entries.سائيز()));
		إذا (hashtable.خالي_جايون() || اشاريو < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		إذا (k == اشاريو) {
			hashtable[hash] = entries[اشاريو].ايندڙ;
		} وإلا {
			طالما (entries[k].ايندڙ != اشاريو) {
				k = entries[k].ايندڙ;
				do_assert(0 <= k && k < صحيح(entries.سائيز()));
			}
			entries[k].ايندڙ = entries[اشاريو].ايندڙ;
		}

		صحيح پٺ_idx = entries.سائيز()-1;

		إذا (اشاريو != پٺ_idx)
		{
			Hasher::hash_t پٺ_hash = do_hash(entries[پٺ_idx].udata);

			k = hashtable[پٺ_hash];
			إذا (k == پٺ_idx) {
				hashtable[پٺ_hash] = اشاريو;
			} وإلا {
				طالما (entries[k].ايندڙ != پٺ_idx) {
					k = entries[k].ايندڙ;
					do_assert(0 <= k && k < صحيح(entries.سائيز()));
				}
				entries[k].ايندڙ = اشاريو;
			}

			entries[اشاريو] = std::move(entries[پٺ_idx]);
		}

		entries.ڪڍو_پٺ();

		إذا (entries.خالي_جايون())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &ڪنجي, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون())
			أرجع -1;

		إذا (entries.سائيز() * hashtable_سائيز_trigger > hashtable.سائيز()) {
			do_rehash();
			hash = do_hash(ڪنجي);
		}

		أرجع do_lookup_اندروني(ڪنجي, hash);
	}

	صحيح do_lookup_اندروني(ثابت K &ڪنجي, Hasher::hash_t hash) ثابت
	{
		صحيح اشاريو = hashtable[hash];

		طالما (اشاريو >= 0 && !ops.cmp(entries[اشاريو].udata, ڪنجي)) {
			اشاريو = entries[اشاريو].ايندڙ;
			do_assert(-1 <= اشاريو && اشاريو < صحيح(entries.سائيز()));
		}

		أرجع اشاريو;
	}

	صحيح do_lookup_no_rehash(ثابت K &ڪنجي, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالي_جايون())
			أرجع -1;

		أرجع do_lookup_اندروني(ڪنجي, hash);
	}

	صحيح do_وجهو(ثابت K &قدر, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون()) {
			entries.emplace_پٺ(قدر, -1);
			do_rehash();
			hash = do_hash(قدر);
		} وإلا {
			entries.emplace_پٺ(قدر, hashtable[hash]);
			hashtable[hash] = entries.سائيز() - 1;
		}
		أرجع entries.سائيز() - 1;
	}

	صحيح do_وجهو(K &&rvalue, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالي_جايون()) {
			entries.emplace_پٺ(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.پٺ().udata);
		} وإلا {
			entries.emplace_پٺ(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.سائيز() - 1;
		}
		أرجع entries.سائيز() - 1;
	}

عام:
	class const_iterator
	{
		دوست class pool;
	محمي:
		ثابت pool *ptr;
		صحيح اشاريو;
		const_iterator(ثابت pool *ptr, صحيح اشاريو) : ptr(ptr), اشاريو(اشاريو) { }
	عام:
		تعريفنوع std::forward_iterator_ٽيگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارو;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاريو--; أرجع *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
		ثابت K &عامل*() ثابت { أرجع ptr->entries[اشاريو].udata; }
		ثابت K *عامل->() ثابت { أرجع &ptr->entries[اشاريو].udata; }
	};

	class iterator
	{
		دوست class pool;
	محمي:
		pool *ptr;
		صحيح اشاريو;
		iterator(pool *ptr, صحيح اشاريو) : ptr(ptr), اشاريو(اشاريو) { }
	عام:
		تعريفنوع std::forward_iterator_ٽيگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارو;
		تعريفنوع K& reference;
		iterator() { }
		iterator عامل++() { اشاريو--; أرجع *this; }
		bool عامل==(ثابت iterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
		bool عامل!=(ثابت iterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
		K &عامل*() { أرجع ptr->entries[اشاريو].udata; }
		K *عامل->() { أرجع &ptr->entries[اشاريو].udata; }
		ثابت K &عامل*() ثابت { أرجع ptr->entries[اشاريو].udata; }
		ثابت K *عامل->() ثابت { أرجع &ptr->entries[اشاريو].udata; }
		عامل const_iterator() ثابت { أرجع const_iterator(ptr, اشاريو); }
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
		مٽا_سٽا(other);
	}

	pool &عامل=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	pool &عامل=(pool &&other) noexcept {
		clear();
		مٽا_سٽا(other);
		أرجع *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		لكل (خودڪار &it : list)
			وجهو(it);
	}

	سانچو<class داخلا_Iterator>
	pool(داخلا_Iterator پهريون, داخلا_Iterator آخري)
	{
		وجهو(پهريون, آخري);
	}

	سانچو<class داخلا_Iterator>
	فراغ وجهو(داخلا_Iterator پهريون, داخلا_Iterator آخري)
	{
		لكل (; پهريون != آخري; ++پهريون)
			وجهو(*پهريون);
	}

	std::pair<iterator, bool> وجهو(ثابت K &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر);
		صحيح i = do_lookup(قدر, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(قدر, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> وجهو(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحيح i = do_lookup(rvalue, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_وجهو(std::forward<K>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	سانچو<typename... دليلون_3>
	std::pair<iterator, bool> emplace(دليلون_3&&... دليلون_2)
	{
		أرجع وجهو(K(std::forward<دليلون_3>(دليلون_2)...));
	}

	صحيح erase(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح اشاريو = do_lookup(ڪنجي, hash);
		أرجع do_erase(اشاريو, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.اشاريو, hash);
		أرجع ++it;
	}

	صحيح ڳڻپ(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح ڳڻپ(ثابت K &ڪنجي, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		أرجع i < 0 || i > it.اشاريو ? 0 : 1;
	}

	iterator ڳوليو(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		إذا (i < 0)
			أرجع پڇاڙي();
		أرجع iterator(this, i);
	}

	const_iterator ڳوليو(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			أرجع پڇاڙي();
		أرجع const_iterator(this, i);
	}

	bool عامل[](ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = do_hash(ڪنجي);
		صحيح i = do_lookup(ڪنجي, hash);
		أرجع i >= 0;
	}

	سانچو<typename Compare = std::less<K>>
	فراغ ترتيب_ڏيو(Compare comp = Compare())
	{
		std::ترتيب_ڏيو(entries.begin(), entries.پڇاڙي(), [comp](ثابت داخلا_t &a, ثابت داخلا_t &b){ أرجع comp(b.udata, a.udata); });
		do_rehash();
	}

	K ڪڍو()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		أرجع ret;
	}

	فراغ مٽا_سٽا(pool &other)
	{
		hashtable.مٽا_سٽا(other.hashtable);
		entries.مٽا_سٽا(other.entries);
	}

	bool عامل==(ثابت pool &other) ثابت {
		إذا (سائيز() != other.سائيز())
			أرجع false;
		لكل (خودڪار &it : entries)
			إذا (!other.ڳڻپ(it.udata))
				أرجع false;
		أرجع true;
	}

	bool عامل!=(ثابت pool &other) ثابت {
		أرجع !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودڪار &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(سائيز_t n) { entries.reserve(n); }
	سائيز_t سائيز() ثابت { أرجع entries.سائيز(); }
	bool خالي_جايون() ثابت { أرجع entries.خالي_جايون(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.سائيز())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.سائيز())-1-n); }
	iterator پڇاڙي() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.سائيز())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.سائيز())-1-n); }
	const_iterator پڇاڙي() ثابت { أرجع const_iterator(nullptr, -1); }
};

سانچو<typename K, صحيح وٿي, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل idict
{
	pool<K, OPS> database;

عام:
	class const_iterator
	{
		دوست class idict;
	محمي:
		ثابت idict &container;
		صحيح اشاريو;
		const_iterator(ثابت idict &container, صحيح اشاريو) : container(container), اشاريو(اشاريو) { }
	عام:
		تعريفنوع std::forward_iterator_ٽيگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارو;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاريو++; أرجع *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
		ثابت K &عامل*() ثابت { أرجع container[اشاريو]; }
		ثابت K *عامل->() ثابت { أرجع &container[اشاريو]; }
	};

	constexpr idict()
	{
	}

	صحيح عامل()(ثابت K &ڪنجي)
	{
		Hasher::hash_t hash = database.do_hash(ڪنجي);
		صحيح i = database.do_lookup(ڪنجي, hash);
		إذا (i < 0)
			i = database.do_وجهو(ڪنجي, hash);
		أرجع i + وٿي;
	}

	صحيح at(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = database.do_hash(ڪنجي);
		صحيح i = database.do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			throw std::ٻاهريون_جو_range("idict::at()");
		أرجع i + وٿي;
	}

	صحيح at(ثابت K &ڪنجي, صحيح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(ڪنجي);
		صحيح i = database.do_lookup_no_rehash(ڪنجي, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع i + وٿي;
	}

	صحيح ڳڻپ(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = database.do_hash(ڪنجي);
		صحيح i = database.do_lookup_no_rehash(ڪنجي, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح lookup(ثابت K &ڪنجي) ثابت
	{
		Hasher::hash_t hash = database.do_hash(ڪنجي);
		أرجع database.do_lookup_no_rehash(ڪنجي, hash);
	}

	فراغ expect(ثابت K &ڪنجي, صحيح i)
	{
		صحيح j = (*this)(ڪنجي);
		إذا (i != j)
			throw std::ٻاهريون_جو_range("idict::expect()");
	}

	ثابت K &عامل[](صحيح اشاريو) ثابت
	{
		أرجع database.entries.at(اشاريو - وٿي).udata;
	}

	فراغ مٽا_سٽا(idict &other)
	{
		database.مٽا_سٽا(other.database);
	}

	فراغ reserve(سائيز_t n) { database.reserve(n); }
	سائيز_t سائيز() ثابت { أرجع database.سائيز(); }
	bool خالي_جايون() ثابت { أرجع database.خالي_جايون(); }
	فراغ clear() { database.clear(); }

	const_iterator begin() ثابت { أرجع const_iterator(*this, وٿي); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(*this, n); }
	const_iterator پڇاڙي() ثابت { أرجع const_iterator(*this, وٿي + سائيز()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
سانچو<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_اڻ_استعمال_ٿيل mfp
{
	idict<K, 0, OPS> database;
	class Atomic_والد {
	عام:
		explicit Atomic_والد(صحيح p) : والد(p) {}
		Atomic_والد(ثابت Atomic_والد &other) : والد(other.get()) {}
		Atomic_والد &عامل=(ثابت Atomic_والد &other) { سيٽ(other.get()); أرجع *this; }
		صحيح get() ثابت { أرجع والد.ڀريو(std::يادگيري_ترتيب_relaxed); }
		فراغ سيٽ(صحيح p) { والد.store(p, std::يادگيري_ترتيب_relaxed); }
	خاص:
		std::atomic<صحيح> والد;
	};
	std::ويڪٽر<Atomic_والد> parents;

عام:
	تعريفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحيح عامل()(ثابت K &ڪنجي)
	{
		صحيح i = database(ڪنجي);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		إذا (parents.سائيز() < database.سائيز()) {
			parents.emplace_پٺ(-1);
		}
		أرجع i;
	}

	// Finds an element at given index
	ثابت K &عامل[](صحيح اشاريو) ثابت
	{
		أرجع database[اشاريو];
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

		طالما (true) {
			صحيح pp = parents[p].get();
			إذا (pp < 0)
				اكسر;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		طالما (k != p) {
			صحيح ايندڙ_k = parents[k].get();
			const_cast<Atomic_والد*>(&parents[k])->سيٽ(p);
			k = ايندڙ_k;
		}

		أرجع p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	فراغ imerge(صحيح i, صحيح j)
	{
		i = ifind(i);
		j = ifind(j);

		إذا (i != j)
			parents[i].سيٽ(j);
	}

	فراغ ipromote(صحيح i)
	{
		صحيح k = i;

		طالما (k != -1) {
			صحيح ايندڙ_k = parents[k].get();
			parents[k].سيٽ(i);
			k = ايندڙ_k;
		}

		parents[i].سيٽ(-1);
	}

	صحيح lookup(ثابت K &a)
	{
		أرجع ifind((*this)(a));
	}

	ثابت K &ڳوليو(ثابت K &a) ثابت
	{
		صحيح i = database.at(a, -1);
		إذا (i < 0)
			أرجع a;
		أرجع (*this)[ifind(i)];
	}

	فراغ ملايو(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	فراغ promote(ثابت K &a)
	{
		صحيح i = database.at(a, -1);
		إذا (i >= 0)
			ipromote(i);
	}

	فراغ مٽا_سٽا(mfp &other)
	{
		database.مٽا_سٽا(other.database);
		parents.مٽا_سٽا(other.parents);
	}

	فراغ reserve(سائيز_t n) { database.reserve(n); }
	سائيز_t سائيز() ثابت { أرجع database.سائيز(); }
	bool خالي_جايون() ثابت { أرجع database.خالي_جايون(); }
	فراغ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { أرجع database.begin(); }
	const_iterator element(صحيح n) ثابت { أرجع database.element(n); }
	const_iterator پڇاڙي() ثابت { أرجع database.پڇاڙي(); }
};

} /* namespace hashlib */

#جيڪڏهن_پورو
