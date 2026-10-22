// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#که_نه_تعریف_شوی HASHLIB_H
#تعریف_کړه HASHLIB_H

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

#تعریف_کړه YS_HASHING_نسخه 1

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
#  define HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی __attribute__((warn_ناکارول_شوی))
#وإلا
#  define HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی
#د_که_پای

ثابت صحيح hashtable_اندازه_trigger = 2;
ثابت صحيح hashtable_اندازه_ضریب = 3;

namespace legacy {
	مضمن uint32_t djb2_جمع_کړه(uint32_t a, uint32_t b) {
		أرجع ((a << 5) + a) + b;
	}
};

کينډۍ<typename T>
بنية hash_ops;

مضمن بینښه صحيح mkhash_xorshift(بینښه صحيح a) {
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

	کينډۍ<typename T>
	فراغ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	کينډۍ<typename T>
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
#تعریف_کړه HASH_سر_LOOP_FST [[nodiscard]] ساكن مضمن Hasher hash
#تعریف_کړه HASH_سر_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	أرجع h; \
}

کينډۍ<typename T>
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
		} وإلا إذا constexpr (std::is_اشاره_ګر_v<T>) {
			أرجع hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} وإلا إذا constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحيح اندازه = a.اندازه();
			صحيح i = 0;
			طالما (i + 8 < اندازه) {
				uint64_t v;
				حافظه_نقل(&v, a.معلومات() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			حافظه_نقل(&v, a.معلومات() + i, اندازه - i);
			h.hash64(v);
			أرجع h;
		} وإلا {
			أرجع a.hash_into(h);
		}
	}
	HASH_سر_LOOP_FST (ثابت T &a) HASH_سر_LOOP_SND
};

کينډۍ<typename P, typename Q> بنية hash_ops<std::pair<P, Q>> {
	ساكن مضمن bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.لومړی, h);
		h = hash_ops<Q>::hash_into(a.دوهم, h);
		أرجع h;
	}
	HASH_سر_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_سر_LOOP_SND
	[[nodiscard]] ساكن مضمن Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		أرجع h;
	}
};

کينډۍ<typename... T> بنية hash_ops<std::tuple<T...>> {
	ساكن مضمن bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		أرجع a == b;
	}
	کينډۍ<اندازه_t I = 0>
	ساكن مضمن typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		أرجع h;
	}
	کينډۍ<اندازه_t I = 0>
	ساكن مضمن typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعريفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		أرجع h;
	}
	HASH_سر_LOOP_FST (ثابت std::tuple<T...> &a) HASH_سر_LOOP_SND
};

کينډۍ<typename T> بنية hash_ops<std::ویکټور<T>> {
	ساكن مضمن bool cmp(ثابت std::ویکټور<T> &a, ثابت std::ویکټور<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::ویکټور<T> &a, Hasher h) {
		h.eat((uint32_t)a.اندازه());
		لكل (خپلکاری k : a)
			h.eat(k);
		أرجع h;
	}
	HASH_سر_LOOP_FST (ثابت std::ویکټور<T> &a) HASH_سر_LOOP_SND
};

کينډۍ<typename T, اندازه_t N> بنية hash_ops<std::array<T, N>> {
    ساكن مضمن bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        أرجع a == b;
    }
    [[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        لكل (ثابت خپلکاری& k : a)
            h = hash_ops<T>::hash_into(k, h);
        أرجع h;
    }
	HASH_سر_LOOP_FST (ثابت std::array<T, N> &a) HASH_سر_LOOP_SND
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
	HASH_سر_LOOP_FST (ثابت حرف *a) HASH_سر_LOOP_SND
};

کينډۍ <> بنية hash_ops<حرف*> : hash_cstr_ops {};

بنية hash_ptr_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت فراغ *a, Hasher h) {
		أرجع hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_سر_LOOP_FST (ثابت فراغ *a) HASH_سر_LOOP_SND
};

بنية hash_obj_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	کينډۍ<typename T>
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T *a, Hasher h) {
		إذا (a)
			h = a->hash_into(h);
		وإلا
			h.eat(0);
		أرجع h;
	}
	کينډۍ<typename T>
	HASH_سر_LOOP_FST (ثابت T *a) HASH_سر_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
کينډۍ<typename T>
[[nodiscard]]
Hasher::hash_t پرلپسې_hash(ثابت T& obj) {
	أرجع hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
کينډۍ<typename T>
[[nodiscard]]
[[deprecated]]
مضمن بینښه صحيح mkhash(ثابت T &v) {
	أرجع (بینښه صحيح) پرلپسې_hash<T>(v);
}

کينډۍ<> بنية hash_ops<std::monostate> {
	ساكن مضمن bool cmp(std::monostate a, std::monostate b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(std::monostate, Hasher h) {
		أرجع h;
	}
};

کينډۍ<typename... T> بنية hash_ops<std::variant<T...>> {
	ساكن مضمن bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::لیدنه([& h](ثابت خپلکاری &v) { h.eat(v); }, a);
		h.eat(a.شاخص());
		أرجع h;
	}
};

کينډۍ<typename T> بنية hash_ops<std::optional<T>> {
	ساكن مضمن bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		إذا(a.has_ارزښت())
			h.eat(*a);
		وإلا
			h.eat(0);
		أرجع h;
	}
};

مضمن بینښه صحيح hashtable_اندازه(بینښه صحيح min_اندازه)
{
	// Primes as generated by https://oeis.org/A175953
	ساكن std::ویکټور<بینښه صحيح> zero_and_some_لومړني_عددونه = {
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

	لكل (خپلکاری p : zero_and_some_لومړني_عددونه)
		إذا (p >= min_اندازه) أرجع p;

	إذا (حجم(بینښه صحيح) == 4)
		throw std::اوږدتیا_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	لكل (خپلکاری p : zero_and_some_لومړني_عددونه)
		إذا (100129 * p > min_اندازه) أرجع 100129 * p;

	throw std::اوږدتیا_error("hash table exceeded maximum size.");
}

کينډۍ<typename K, typename T, typename OPS = hash_ops<K>> class dict;
کينډۍ<typename K, صحيح دننه_واټن = 0, typename OPS = hash_ops<K>> class idict;
کينډۍ<typename K, typename OPS = hash_ops<K>> class pool;
کينډۍ<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عام:
	commutative_hash() {
		buckets.fill(0);
	}
	کينډۍ <typename T>
	فراغ eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	فراغ eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		اندازه_t شاخص = v & (buckets.اندازه() - 1);
		buckets[شاخص] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		لكل (خپلکاری b : buckets)
			h.eat(b);
		أرجع h;
	}
خاص:
	std::array<Hasher::hash_t, 4> buckets;
};

کينډۍ<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی dict {
	بنية ثبت_t
	{
		std::pair<K, T> udata;
		صحيح بل;

		ثبت_t() { }
		ثبت_t(ثابت std::pair<K, T> &udata, صحيح بل) : udata(udata), بل(بل) { }
		ثبت_t(std::pair<K, T> &&udata, صحيح بل) : udata(std::move(udata)), بل(بل) { }
		bool چلوونکی<(ثابت ثبت_t &other) ثابت { أرجع udata.لومړی < other.udata.لومړی; }
	};

	std::ویکټور<صحيح> hashtable;
	std::ویکټور<ثبت_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#که_تعریف_شوی NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#د_که_پای

	Hasher::hash_t do_hash(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.تشه())
			hash = ops.hash(کلی).yield() % (بینښه صحيح)(hashtable.اندازه());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_اندازه(entries.ظرفیت() * hashtable_اندازه_ضریب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.اندازه()); i++) {
			do_assert(-1 <= entries[i].بل && entries[i].بل < صحيح(entries.اندازه()));
			Hasher::hash_t hash = do_hash(entries[i].udata.لومړی);
			entries[i].بل = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح شاخص, Hasher::hash_t hash)
	{
		do_assert(شاخص < صحيح(entries.اندازه()));
		إذا (hashtable.تشه() || شاخص < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		do_assert(0 <= k && k < صحيح(entries.اندازه()));

		إذا (k == شاخص) {
			hashtable[hash] = entries[شاخص].بل;
		} وإلا {
			طالما (entries[k].بل != شاخص) {
				k = entries[k].بل;
				do_assert(0 <= k && k < صحيح(entries.اندازه()));
			}
			entries[k].بل = entries[شاخص].بل;
		}

		صحيح شا_idx = entries.اندازه()-1;

		إذا (شاخص != شا_idx)
		{
			Hasher::hash_t شا_hash = do_hash(entries[شا_idx].udata.لومړی);

			k = hashtable[شا_hash];
			do_assert(0 <= k && k < صحيح(entries.اندازه()));

			إذا (k == شا_idx) {
				hashtable[شا_hash] = شاخص;
			} وإلا {
				طالما (entries[k].بل != شا_idx) {
					k = entries[k].بل;
					do_assert(0 <= k && k < صحيح(entries.اندازه()));
				}
				entries[k].بل = شاخص;
			}

			entries[شاخص] = std::move(entries[شا_idx]);
		}

		entries.راوباسه_شا();

		إذا (entries.تشه())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلی, Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه())
			أرجع -1;

		إذا (entries.اندازه() * hashtable_اندازه_trigger > hashtable.اندازه()) {
			do_rehash();
			hash = do_hash(کلی);
		}

		أرجع do_lookup_دننی(کلی, hash);
	}

	صحيح do_lookup_دننی(ثابت K &کلی, Hasher::hash_t hash) ثابت
	{
		صحيح شاخص = hashtable[hash];

		طالما (شاخص >= 0 && !ops.cmp(entries[شاخص].udata.لومړی, کلی)) {
			شاخص = entries[شاخص].بل;
			do_assert(-1 <= شاخص && شاخص < صحيح(entries.اندازه()));
		}

		أرجع شاخص;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلی, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.تشه())
			أرجع -1;

		أرجع do_lookup_دننی(کلی, hash);
	}

	صحيح do_ورننباسه(ثابت K &کلی, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه()) {
			entries.emplace_شا(std::pair<K, T>(کلی, T()), -1);
			do_rehash();
		} وإلا {
			entries.emplace_شا(std::pair<K, T>(کلی, T()), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_ورننباسه(ثابت std::pair<K, T> &ارزښت, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه()) {
			entries.emplace_شا(ارزښت, -1);
			do_rehash();
		} وإلا {
			entries.emplace_شا(ارزښت, hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_ورننباسه(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه()) {
			entries.emplace_شا(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} وإلا {
			entries.emplace_شا(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

عام:
	class const_iterator
	{
		ملګری class dict;
	محمي:
		ثابت dict *ptr;
		صحيح شاخص;
		const_iterator(ثابت dict *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::bidirectional_iterator_نښان iterator_category;
		تعريفنوع std::pair<K, T> ارزښت_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع ثابت std::pair<K, T>* اشاره_ګر;
		تعريفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator چلوونکی++() { شاخص--; أرجع *this; }
		const_iterator چلوونکی++(صحيح) { const_iterator tmp = *this; شاخص--; أرجع tmp; }
		const_iterator چلوونکی--() { شاخص++; أرجع *this; }
		const_iterator چلوونکی--(صحيح) { const_iterator tmp = *this; شاخص++; أرجع tmp; }
		const_iterator چلوونکی+=(صحيح amt) { شاخص -= amt; أرجع *this; }
		bool چلوونکی<(ثابت const_iterator &other) ثابت { أرجع شاخص > other.شاخص; }
		bool چلوونکی==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool چلوونکی!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت std::pair<K, T> &چلوونکی*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> *چلوونکی->() ثابت { أرجع &ptr->entries[شاخص].udata; }
	};

	class iterator
	{
		ملګری class dict;
	محمي:
		dict *ptr;
		صحيح شاخص;
		iterator(dict *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_نښان iterator_category;
		تعريفنوع std::pair<K, T> ارزښت_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع std::pair<K, T>* اشاره_ګر;
		تعريفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator چلوونکی++() { شاخص--; أرجع *this; }
		iterator چلوونکی+=(صحيح amt) { شاخص -= amt; أرجع *this; }
		bool چلوونکی<(ثابت iterator &other) ثابت { أرجع شاخص > other.شاخص; }
		bool چلوونکی==(ثابت iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool چلوونکی!=(ثابت iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		std::pair<K, T> &چلوونکی*() { أرجع ptr->entries[شاخص].udata; }
		std::pair<K, T> *چلوونکی->() { أرجع &ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> &چلوونکی*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> *چلوونکی->() ثابت { أرجع &ptr->entries[شاخص].udata; }
		چلوونکی const_iterator() ثابت { أرجع const_iterator(ptr, شاخص); }
	};
	using سرچپه_iterator = std::سرچپه_iterator<const_iterator>;
	سرچپه_iterator rbegin() ثابت {
		أرجع std::make_سرچپه_iterator(پای_ټکی());
	}
	سرچپه_iterator rend() ثابت {
		أرجع std::make_سرچپه_iterator(begin());
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
		بدلون(other);
	}

	dict &چلوونکی=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	dict &چلوونکی=(dict &&other) noexcept {
		clear();
		بدلون(other);
		أرجع *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		لكل (خپلکاری &it : list)
			ورننباسه(it);
	}

	کينډۍ<class ننوت_Iterator>
	dict(ننوت_Iterator لومړی, ننوت_Iterator وروستی)
	{
		ورننباسه(لومړی, وروستی);
	}

	کينډۍ<class ننوت_Iterator>
	فراغ ورننباسه(ننوت_Iterator لومړی, ننوت_Iterator وروستی)
	{
		لكل (; لومړی != وروستی; ++لومړی)
			ورننباسه(*لومړی);
	}

	std::pair<iterator, bool> ورننباسه(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(کلی, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ورننباسه(ثابت std::pair<K, T> &ارزښت)
	{
		Hasher::hash_t hash = do_hash(ارزښت.لومړی);
		صحيح i = do_lookup(ارزښت.لومړی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(ارزښت, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ورننباسه(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.لومړی);
		صحيح i = do_lookup(rvalue.لومړی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::forward<std::pair<K, T>>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلی, T ثابت &ارزښت)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::make_pair(کلی, ارزښت), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلی, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::make_pair(کلی, std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &ارزښت)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::make_pair(std::forward<K>(rkey), ارزښت), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحيح erase(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح شاخص = do_lookup(کلی, hash);
		أرجع do_erase(شاخص, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->لومړی);
		do_erase(it.شاخص, hash);
		أرجع ++it;
	}

	صحيح شمېر(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح شمېر(ثابت K &کلی, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		أرجع i < 0 || i > it.شاخص ? 0 : 1;
	}

	iterator پیدا_کړه(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i < 0)
			أرجع پای_ټکی();
		أرجع iterator(this, i);
	}

	const_iterator پیدا_کړه(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			أرجع پای_ټکی();
		أرجع const_iterator(this, i);
	}

	T& at(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i < 0)
			throw std::بهر_د_range("dict::at()");
		أرجع entries[i].udata.دوهم;
	}

	ثابت T& at(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			throw std::بهر_د_range("dict::at()");
		أرجع entries[i].udata.دوهم;
	}

	ثابت T& at(ثابت K &کلی, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع entries[i].udata.دوهم;
	}

	T& چلوونکی[](ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i < 0)
			i = do_ورننباسه(std::pair<K, T>(کلی, T()), hash);
		أرجع entries[i].udata.دوهم;
	}

	کينډۍ<typename Compare = std::less<K>>
	فراغ ترتیبول(Compare comp = Compare())
	{
		std::ترتیبول(entries.begin(), entries.پای_ټکی(), [comp](ثابت ثبت_t &a, ثابت ثبت_t &b){ أرجع comp(b.udata.لومړی, a.udata.لومړی); });
		do_rehash();
	}

	فراغ بدلون(dict &other)
	{
		hashtable.بدلون(other.hashtable);
		entries.بدلون(other.entries);
	}

	bool چلوونکی==(ثابت dict &other) ثابت {
		إذا (اندازه() != other.اندازه())
			أرجع false;
		لكل (خپلکاری &it : entries) {
			خپلکاری oit = other.پیدا_کړه(it.udata.لومړی);
			إذا (oit == other.پای_ټکی() || !(oit->دوهم == it.udata.دوهم))
				أرجع false;
		}
		أرجع true;
	}

	bool چلوونکی!=(ثابت dict &other) ثابت {
		أرجع !چلوونکی==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خپلکاری &it : entries) {
			Hasher ثبت_hash;
			ثبت_hash.eat(it.udata.لومړی);
			ثبت_hash.eat(it.udata.دوهم);
			comm.eat(ثبت_hash);
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(اندازه_t n) { entries.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع entries.اندازه(); }
	bool تشه() ثابت { أرجع entries.تشه(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.اندازه())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.اندازه())-1-n); }
	iterator پای_ټکی() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1-n); }
	const_iterator پای_ټکی() ثابت { أرجع const_iterator(this, -1); }
};

کينډۍ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی pool
{
	کينډۍ<typename, صحيح, typename> ملګری class idict;

محمي:
	بنية ثبت_t
	{
		K udata;
		صحيح بل;

		ثبت_t() { }
		ثبت_t(ثابت K &udata, صحيح بل) : udata(udata), بل(بل) { }
		ثبت_t(K &&udata, صحيح بل) : udata(std::move(udata)), بل(بل) { }
	};

	std::ویکټور<صحيح> hashtable;
	std::ویکټور<ثبت_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#که_تعریف_شوی NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#د_که_پای

	Hasher::hash_t do_hash(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.تشه())
			hash = ops.hash(کلی).yield() % (بینښه صحيح)(hashtable.اندازه());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_اندازه(entries.ظرفیت() * hashtable_اندازه_ضریب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.اندازه()); i++) {
			do_assert(-1 <= entries[i].بل && entries[i].بل < صحيح(entries.اندازه()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].بل = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح شاخص, Hasher::hash_t hash)
	{
		do_assert(شاخص < صحيح(entries.اندازه()));
		إذا (hashtable.تشه() || شاخص < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		إذا (k == شاخص) {
			hashtable[hash] = entries[شاخص].بل;
		} وإلا {
			طالما (entries[k].بل != شاخص) {
				k = entries[k].بل;
				do_assert(0 <= k && k < صحيح(entries.اندازه()));
			}
			entries[k].بل = entries[شاخص].بل;
		}

		صحيح شا_idx = entries.اندازه()-1;

		إذا (شاخص != شا_idx)
		{
			Hasher::hash_t شا_hash = do_hash(entries[شا_idx].udata);

			k = hashtable[شا_hash];
			إذا (k == شا_idx) {
				hashtable[شا_hash] = شاخص;
			} وإلا {
				طالما (entries[k].بل != شا_idx) {
					k = entries[k].بل;
					do_assert(0 <= k && k < صحيح(entries.اندازه()));
				}
				entries[k].بل = شاخص;
			}

			entries[شاخص] = std::move(entries[شا_idx]);
		}

		entries.راوباسه_شا();

		إذا (entries.تشه())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلی, Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه())
			أرجع -1;

		إذا (entries.اندازه() * hashtable_اندازه_trigger > hashtable.اندازه()) {
			do_rehash();
			hash = do_hash(کلی);
		}

		أرجع do_lookup_دننی(کلی, hash);
	}

	صحيح do_lookup_دننی(ثابت K &کلی, Hasher::hash_t hash) ثابت
	{
		صحيح شاخص = hashtable[hash];

		طالما (شاخص >= 0 && !ops.cmp(entries[شاخص].udata, کلی)) {
			شاخص = entries[شاخص].بل;
			do_assert(-1 <= شاخص && شاخص < صحيح(entries.اندازه()));
		}

		أرجع شاخص;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلی, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.تشه())
			أرجع -1;

		أرجع do_lookup_دننی(کلی, hash);
	}

	صحيح do_ورننباسه(ثابت K &ارزښت, Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه()) {
			entries.emplace_شا(ارزښت, -1);
			do_rehash();
			hash = do_hash(ارزښت);
		} وإلا {
			entries.emplace_شا(ارزښت, hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_ورننباسه(K &&rvalue, Hasher::hash_t &hash)
	{
		إذا (hashtable.تشه()) {
			entries.emplace_شا(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.شا().udata);
		} وإلا {
			entries.emplace_شا(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

عام:
	class const_iterator
	{
		ملګری class pool;
	محمي:
		ثابت pool *ptr;
		صحيح شاخص;
		const_iterator(ثابت pool *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_نښان iterator_category;
		تعريفنوع K ارزښت_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_ګر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator چلوونکی++() { شاخص--; أرجع *this; }
		bool چلوونکی==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool چلوونکی!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت K &چلوونکی*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت K *چلوونکی->() ثابت { أرجع &ptr->entries[شاخص].udata; }
	};

	class iterator
	{
		ملګری class pool;
	محمي:
		pool *ptr;
		صحيح شاخص;
		iterator(pool *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_نښان iterator_category;
		تعريفنوع K ارزښت_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_ګر;
		تعريفنوع K& reference;
		iterator() { }
		iterator چلوونکی++() { شاخص--; أرجع *this; }
		bool چلوونکی==(ثابت iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool چلوونکی!=(ثابت iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		K &چلوونکی*() { أرجع ptr->entries[شاخص].udata; }
		K *چلوونکی->() { أرجع &ptr->entries[شاخص].udata; }
		ثابت K &چلوونکی*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت K *چلوونکی->() ثابت { أرجع &ptr->entries[شاخص].udata; }
		چلوونکی const_iterator() ثابت { أرجع const_iterator(ptr, شاخص); }
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
		بدلون(other);
	}

	pool &چلوونکی=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	pool &چلوونکی=(pool &&other) noexcept {
		clear();
		بدلون(other);
		أرجع *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		لكل (خپلکاری &it : list)
			ورننباسه(it);
	}

	کينډۍ<class ننوت_Iterator>
	pool(ننوت_Iterator لومړی, ننوت_Iterator وروستی)
	{
		ورننباسه(لومړی, وروستی);
	}

	کينډۍ<class ننوت_Iterator>
	فراغ ورننباسه(ننوت_Iterator لومړی, ننوت_Iterator وروستی)
	{
		لكل (; لومړی != وروستی; ++لومړی)
			ورننباسه(*لومړی);
	}

	std::pair<iterator, bool> ورننباسه(ثابت K &ارزښت)
	{
		Hasher::hash_t hash = do_hash(ارزښت);
		صحيح i = do_lookup(ارزښت, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(ارزښت, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> ورننباسه(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحيح i = do_lookup(rvalue, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_ورننباسه(std::forward<K>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	کينډۍ<typename... دلیلونه_3>
	std::pair<iterator, bool> emplace(دلیلونه_3&&... دلیلونه_2)
	{
		أرجع ورننباسه(K(std::forward<دلیلونه_3>(دلیلونه_2)...));
	}

	صحيح erase(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح شاخص = do_lookup(کلی, hash);
		أرجع do_erase(شاخص, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.شاخص, hash);
		أرجع ++it;
	}

	صحيح شمېر(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح شمېر(ثابت K &کلی, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		أرجع i < 0 || i > it.شاخص ? 0 : 1;
	}

	iterator پیدا_کړه(ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		إذا (i < 0)
			أرجع پای_ټکی();
		أرجع iterator(this, i);
	}

	const_iterator پیدا_کړه(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			أرجع پای_ټکی();
		أرجع const_iterator(this, i);
	}

	bool چلوونکی[](ثابت K &کلی)
	{
		Hasher::hash_t hash = do_hash(کلی);
		صحيح i = do_lookup(کلی, hash);
		أرجع i >= 0;
	}

	کينډۍ<typename Compare = std::less<K>>
	فراغ ترتیبول(Compare comp = Compare())
	{
		std::ترتیبول(entries.begin(), entries.پای_ټکی(), [comp](ثابت ثبت_t &a, ثابت ثبت_t &b){ أرجع comp(b.udata, a.udata); });
		do_rehash();
	}

	K راوباسه()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		أرجع ret;
	}

	فراغ بدلون(pool &other)
	{
		hashtable.بدلون(other.hashtable);
		entries.بدلون(other.entries);
	}

	bool چلوونکی==(ثابت pool &other) ثابت {
		إذا (اندازه() != other.اندازه())
			أرجع false;
		لكل (خپلکاری &it : entries)
			إذا (!other.شمېر(it.udata))
				أرجع false;
		أرجع true;
	}

	bool چلوونکی!=(ثابت pool &other) ثابت {
		أرجع !چلوونکی==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خپلکاری &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(اندازه_t n) { entries.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع entries.اندازه(); }
	bool تشه() ثابت { أرجع entries.تشه(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.اندازه())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.اندازه())-1-n); }
	iterator پای_ټکی() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1-n); }
	const_iterator پای_ټکی() ثابت { أرجع const_iterator(nullptr, -1); }
};

کينډۍ<typename K, صحيح دننه_واټن, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی idict
{
	pool<K, OPS> database;

عام:
	class const_iterator
	{
		ملګری class idict;
	محمي:
		ثابت idict &container;
		صحيح شاخص;
		const_iterator(ثابت idict &container, صحيح شاخص) : container(container), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_نښان iterator_category;
		تعريفنوع K ارزښت_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_ګر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator چلوونکی++() { شاخص++; أرجع *this; }
		bool چلوونکی==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool چلوونکی!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت K &چلوونکی*() ثابت { أرجع container[شاخص]; }
		ثابت K *چلوونکی->() ثابت { أرجع &container[شاخص]; }
	};

	constexpr idict()
	{
	}

	صحيح چلوونکی()(ثابت K &کلی)
	{
		Hasher::hash_t hash = database.do_hash(کلی);
		صحيح i = database.do_lookup(کلی, hash);
		إذا (i < 0)
			i = database.do_ورننباسه(کلی, hash);
		أرجع i + دننه_واټن;
	}

	صحيح at(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلی);
		صحيح i = database.do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			throw std::بهر_د_range("idict::at()");
		أرجع i + دننه_واټن;
	}

	صحيح at(ثابت K &کلی, صحيح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلی);
		صحيح i = database.do_lookup_no_rehash(کلی, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع i + دننه_واټن;
	}

	صحيح شمېر(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلی);
		صحيح i = database.do_lookup_no_rehash(کلی, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح lookup(ثابت K &کلی) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلی);
		أرجع database.do_lookup_no_rehash(کلی, hash);
	}

	فراغ expect(ثابت K &کلی, صحيح i)
	{
		صحيح j = (*this)(کلی);
		إذا (i != j)
			throw std::بهر_د_range("idict::expect()");
	}

	ثابت K &چلوونکی[](صحيح شاخص) ثابت
	{
		أرجع database.entries.at(شاخص - دننه_واټن).udata;
	}

	فراغ بدلون(idict &other)
	{
		database.بدلون(other.database);
	}

	فراغ reserve(اندازه_t n) { database.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع database.اندازه(); }
	bool تشه() ثابت { أرجع database.تشه(); }
	فراغ clear() { database.clear(); }

	const_iterator begin() ثابت { أرجع const_iterator(*this, دننه_واټن); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(*this, n); }
	const_iterator پای_ټکی() ثابت { أرجع const_iterator(*this, دننه_واټن + اندازه()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
کينډۍ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_ناکارول_شوی mfp
{
	idict<K, 0, OPS> database;
	class Atomic_پلار {
	عام:
		explicit Atomic_پلار(صحيح p) : پلار(p) {}
		Atomic_پلار(ثابت Atomic_پلار &other) : پلار(other.get()) {}
		Atomic_پلار &چلوونکی=(ثابت Atomic_پلار &other) { سېټ(other.get()); أرجع *this; }
		صحيح get() ثابت { أرجع پلار.بارول(std::حافظه_ترتیب_relaxed); }
		فراغ سېټ(صحيح p) { پلار.store(p, std::حافظه_ترتیب_relaxed); }
	خاص:
		std::atomic<صحيح> پلار;
	};
	std::ویکټور<Atomic_پلار> parents;

عام:
	تعريفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحيح چلوونکی()(ثابت K &کلی)
	{
		صحيح i = database(کلی);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		إذا (parents.اندازه() < database.اندازه()) {
			parents.emplace_شا(-1);
		}
		أرجع i;
	}

	// Finds an element at given index
	ثابت K &چلوونکی[](صحيح شاخص) ثابت
	{
		أرجع database[شاخص];
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
			صحيح بل_k = parents[k].get();
			const_cast<Atomic_پلار*>(&parents[k])->سېټ(p);
			k = بل_k;
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
			parents[i].سېټ(j);
	}

	فراغ ipromote(صحيح i)
	{
		صحيح k = i;

		طالما (k != -1) {
			صحيح بل_k = parents[k].get();
			parents[k].سېټ(i);
			k = بل_k;
		}

		parents[i].سېټ(-1);
	}

	صحيح lookup(ثابت K &a)
	{
		أرجع ifind((*this)(a));
	}

	ثابت K &پیدا_کړه(ثابت K &a) ثابت
	{
		صحيح i = database.at(a, -1);
		إذا (i < 0)
			أرجع a;
		أرجع (*this)[ifind(i)];
	}

	فراغ یوځای(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	فراغ promote(ثابت K &a)
	{
		صحيح i = database.at(a, -1);
		إذا (i >= 0)
			ipromote(i);
	}

	فراغ بدلون(mfp &other)
	{
		database.بدلون(other.database);
		parents.بدلون(other.parents);
	}

	فراغ reserve(اندازه_t n) { database.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع database.اندازه(); }
	bool تشه() ثابت { أرجع database.تشه(); }
	فراغ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { أرجع database.begin(); }
	const_iterator element(صحيح n) ثابت { أرجع database.element(n); }
	const_iterator پای_ټکی() ثابت { أرجع database.پای_ټکی(); }
};

} /* namespace hashlib */

#د_که_پای
