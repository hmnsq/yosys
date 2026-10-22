// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#اگر_تعریف_نشده HASHLIB_H
#تعریف_کن HASHLIB_H

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

#تعریف_کن YS_HASHING_نسخه 1

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
#  define HASHLIB_ATTRIBUTE_WARN_استفادهنشده __attribute__((warn_استفادهنشده))
#وإلا
#  define HASHLIB_ATTRIBUTE_WARN_استفادهنشده
#پایان_اگر

ثابت صحيح hashtable_اندازه_trigger = 2;
ثابت صحيح hashtable_اندازه_ضریب = 3;

namespace legacy {
	مضمن uint32_t djb2_جمع_کن(uint32_t a, uint32_t b) {
		أرجع ((a << 5) + a) + b;
	}
};

قالب<typename T>
بنية hash_ops;

مضمن بیعلامت صحيح mkhash_xorshift(بیعلامت صحيح a) {
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

	قالب<typename T>
	فراغ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	قالب<typename T>
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
#تعریف_کن HASH_بالاترین_LOOP_FST [[nodiscard]] ساكن مضمن Hasher hash
#تعریف_کن HASH_بالاترین_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	أرجع h; \
}

قالب<typename T>
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
		} وإلا إذا constexpr (std::is_اشاره_گر_v<T>) {
			أرجع hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} وإلا إذا constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحيح اندازه = a.اندازه();
			صحيح i = 0;
			طالما (i + 8 < اندازه) {
				uint64_t v;
				کپی_حافظه(&v, a.داده() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			کپی_حافظه(&v, a.داده() + i, اندازه - i);
			h.hash64(v);
			أرجع h;
		} وإلا {
			أرجع a.hash_into(h);
		}
	}
	HASH_بالاترین_LOOP_FST (ثابت T &a) HASH_بالاترین_LOOP_SND
};

قالب<typename P, typename Q> بنية hash_ops<std::pair<P, Q>> {
	ساكن مضمن bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.اولی, h);
		h = hash_ops<Q>::hash_into(a.دومی, h);
		أرجع h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_بالاترین_LOOP_SND
	[[nodiscard]] ساكن مضمن Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		أرجع h;
	}
};

قالب<typename... T> بنية hash_ops<std::tuple<T...>> {
	ساكن مضمن bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		أرجع a == b;
	}
	قالب<اندازه_t I = 0>
	ساكن مضمن typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		أرجع h;
	}
	قالب<اندازه_t I = 0>
	ساكن مضمن typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعريفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		أرجع h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::tuple<T...> &a) HASH_بالاترین_LOOP_SND
};

قالب<typename T> بنية hash_ops<std::وکتور<T>> {
	ساكن مضمن bool cmp(ثابت std::وکتور<T> &a, ثابت std::وکتور<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::وکتور<T> &a, Hasher h) {
		h.eat((uint32_t)a.اندازه());
		لكل (خودکار k : a)
			h.eat(k);
		أرجع h;
	}
	HASH_بالاترین_LOOP_FST (ثابت std::وکتور<T> &a) HASH_بالاترین_LOOP_SND
};

قالب<typename T, اندازه_t N> بنية hash_ops<std::array<T, N>> {
    ساكن مضمن bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        أرجع a == b;
    }
    [[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        لكل (ثابت خودکار& k : a)
            h = hash_ops<T>::hash_into(k, h);
        أرجع h;
    }
	HASH_بالاترین_LOOP_FST (ثابت std::array<T, N> &a) HASH_بالاترین_LOOP_SND
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
	HASH_بالاترین_LOOP_FST (ثابت حرف *a) HASH_بالاترین_LOOP_SND
};

قالب <> بنية hash_ops<حرف*> : hash_cstr_ops {};

بنية hash_ptr_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت فراغ *a, Hasher h) {
		أرجع hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_بالاترین_LOOP_FST (ثابت فراغ *a) HASH_بالاترین_LOOP_SND
};

بنية hash_obj_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	قالب<typename T>
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T *a, Hasher h) {
		إذا (a)
			h = a->hash_into(h);
		وإلا
			h.eat(0);
		أرجع h;
	}
	قالب<typename T>
	HASH_بالاترین_LOOP_FST (ثابت T *a) HASH_بالاترین_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
قالب<typename T>
[[nodiscard]]
Hasher::hash_t پیاپی_hash(ثابت T& obj) {
	أرجع hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
قالب<typename T>
[[nodiscard]]
[[deprecated]]
مضمن بیعلامت صحيح mkhash(ثابت T &v) {
	أرجع (بیعلامت صحيح) پیاپی_hash<T>(v);
}

قالب<> بنية hash_ops<std::monostate> {
	ساكن مضمن bool cmp(std::monostate a, std::monostate b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(std::monostate, Hasher h) {
		أرجع h;
	}
};

قالب<typename... T> بنية hash_ops<std::variant<T...>> {
	ساكن مضمن bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::بازدید([& h](ثابت خودکار &v) { h.eat(v); }, a);
		h.eat(a.شاخص());
		أرجع h;
	}
};

قالب<typename T> بنية hash_ops<std::optional<T>> {
	ساكن مضمن bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		إذا(a.has_مقدار())
			h.eat(*a);
		وإلا
			h.eat(0);
		أرجع h;
	}
};

مضمن بیعلامت صحيح hashtable_اندازه(بیعلامت صحيح min_اندازه)
{
	// Primes as generated by https://oeis.org/A175953
	ساكن std::وکتور<بیعلامت صحيح> zero_and_some_اعداد_اول = {
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

	لكل (خودکار p : zero_and_some_اعداد_اول)
		إذا (p >= min_اندازه) أرجع p;

	إذا (حجم(بیعلامت صحيح) == 4)
		throw std::درازی_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	لكل (خودکار p : zero_and_some_اعداد_اول)
		إذا (100129 * p > min_اندازه) أرجع 100129 * p;

	throw std::درازی_error("hash table exceeded maximum size.");
}

قالب<typename K, typename T, typename OPS = hash_ops<K>> class dict;
قالب<typename K, صحيح جابه_جایی = 0, typename OPS = hash_ops<K>> class idict;
قالب<typename K, typename OPS = hash_ops<K>> class pool;
قالب<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عام:
	commutative_hash() {
		buckets.fill(0);
	}
	قالب <typename T>
	فراغ eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	فراغ eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		اندازه_t شاخص = v & (buckets.اندازه() - 1);
		buckets[شاخص] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		لكل (خودکار b : buckets)
			h.eat(b);
		أرجع h;
	}
خاص:
	std::array<Hasher::hash_t, 4> buckets;
};

قالب<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_استفادهنشده dict {
	بنية مدخل_t
	{
		std::pair<K, T> udata;
		صحيح بعدی;

		مدخل_t() { }
		مدخل_t(ثابت std::pair<K, T> &udata, صحيح بعدی) : udata(udata), بعدی(بعدی) { }
		مدخل_t(std::pair<K, T> &&udata, صحيح بعدی) : udata(std::move(udata)), بعدی(بعدی) { }
		bool عملگر<(ثابت مدخل_t &other) ثابت { أرجع udata.اولی < other.udata.اولی; }
	};

	std::وکتور<صحيح> hashtable;
	std::وکتور<مدخل_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#اگر_تعریف_شده NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#پایان_اگر

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالی())
			hash = ops.hash(کلید).yield() % (بیعلامت صحيح)(hashtable.اندازه());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_اندازه(entries.ظرفیت() * hashtable_اندازه_ضریب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.اندازه()); i++) {
			do_assert(-1 <= entries[i].بعدی && entries[i].بعدی < صحيح(entries.اندازه()));
			Hasher::hash_t hash = do_hash(entries[i].udata.اولی);
			entries[i].بعدی = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح شاخص, Hasher::hash_t hash)
	{
		do_assert(شاخص < صحيح(entries.اندازه()));
		إذا (hashtable.خالی() || شاخص < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		do_assert(0 <= k && k < صحيح(entries.اندازه()));

		إذا (k == شاخص) {
			hashtable[hash] = entries[شاخص].بعدی;
		} وإلا {
			طالما (entries[k].بعدی != شاخص) {
				k = entries[k].بعدی;
				do_assert(0 <= k && k < صحيح(entries.اندازه()));
			}
			entries[k].بعدی = entries[شاخص].بعدی;
		}

		صحيح پشت_idx = entries.اندازه()-1;

		إذا (شاخص != پشت_idx)
		{
			Hasher::hash_t پشت_hash = do_hash(entries[پشت_idx].udata.اولی);

			k = hashtable[پشت_hash];
			do_assert(0 <= k && k < صحيح(entries.اندازه()));

			إذا (k == پشت_idx) {
				hashtable[پشت_hash] = شاخص;
			} وإلا {
				طالما (entries[k].بعدی != پشت_idx) {
					k = entries[k].بعدی;
					do_assert(0 <= k && k < صحيح(entries.اندازه()));
				}
				entries[k].بعدی = شاخص;
			}

			entries[شاخص] = std::move(entries[پشت_idx]);
		}

		entries.برداشت_پشت();

		إذا (entries.خالی())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی())
			أرجع -1;

		إذا (entries.اندازه() * hashtable_اندازه_trigger > hashtable.اندازه()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		أرجع do_lookup_داخلی(کلید, hash);
	}

	صحيح do_lookup_داخلی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحيح شاخص = hashtable[hash];

		طالما (شاخص >= 0 && !ops.cmp(entries[شاخص].udata.اولی, کلید)) {
			شاخص = entries[شاخص].بعدی;
			do_assert(-1 <= شاخص && شاخص < صحيح(entries.اندازه()));
		}

		أرجع شاخص;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالی())
			أرجع -1;

		أرجع do_lookup_داخلی(کلید, hash);
	}

	صحيح do_درج(ثابت K &کلید, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی()) {
			entries.emplace_پشت(std::pair<K, T>(کلید, T()), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پشت(std::pair<K, T>(کلید, T()), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_درج(ثابت std::pair<K, T> &مقدار, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی()) {
			entries.emplace_پشت(مقدار, -1);
			do_rehash();
		} وإلا {
			entries.emplace_پشت(مقدار, hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_درج(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی()) {
			entries.emplace_پشت(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پشت(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

عام:
	class const_iterator
	{
		دوست class dict;
	محمي:
		ثابت dict *ptr;
		صحيح شاخص;
		const_iterator(ثابت dict *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::bidirectional_iterator_برچسب iterator_category;
		تعريفنوع std::pair<K, T> مقدار_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع ثابت std::pair<K, T>* اشاره_گر;
		تعريفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator عملگر++() { شاخص--; أرجع *this; }
		const_iterator عملگر++(صحيح) { const_iterator tmp = *this; شاخص--; أرجع tmp; }
		const_iterator عملگر--() { شاخص++; أرجع *this; }
		const_iterator عملگر--(صحيح) { const_iterator tmp = *this; شاخص++; أرجع tmp; }
		const_iterator عملگر+=(صحيح amt) { شاخص -= amt; أرجع *this; }
		bool عملگر<(ثابت const_iterator &other) ثابت { أرجع شاخص > other.شاخص; }
		bool عملگر==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool عملگر!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت std::pair<K, T> &عملگر*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> *عملگر->() ثابت { أرجع &ptr->entries[شاخص].udata; }
	};

	class iterator
	{
		دوست class dict;
	محمي:
		dict *ptr;
		صحيح شاخص;
		iterator(dict *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_برچسب iterator_category;
		تعريفنوع std::pair<K, T> مقدار_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع std::pair<K, T>* اشاره_گر;
		تعريفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator عملگر++() { شاخص--; أرجع *this; }
		iterator عملگر+=(صحيح amt) { شاخص -= amt; أرجع *this; }
		bool عملگر<(ثابت iterator &other) ثابت { أرجع شاخص > other.شاخص; }
		bool عملگر==(ثابت iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool عملگر!=(ثابت iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		std::pair<K, T> &عملگر*() { أرجع ptr->entries[شاخص].udata; }
		std::pair<K, T> *عملگر->() { أرجع &ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> &عملگر*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت std::pair<K, T> *عملگر->() ثابت { أرجع &ptr->entries[شاخص].udata; }
		عملگر const_iterator() ثابت { أرجع const_iterator(ptr, شاخص); }
	};
	using معکوس_iterator = std::معکوس_iterator<const_iterator>;
	معکوس_iterator rbegin() ثابت {
		أرجع std::make_معکوس_iterator(انتها());
	}
	معکوس_iterator rend() ثابت {
		أرجع std::make_معکوس_iterator(begin());
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
		جابجا(other);
	}

	dict &عملگر=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	dict &عملگر=(dict &&other) noexcept {
		clear();
		جابجا(other);
		أرجع *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		لكل (خودکار &it : list)
			درج(it);
	}

	قالب<class درون_داد_Iterator>
	dict(درون_داد_Iterator اولی, درون_داد_Iterator آخرین)
	{
		درج(اولی, آخرین);
	}

	قالب<class درون_داد_Iterator>
	فراغ درج(درون_داد_Iterator اولی, درون_داد_Iterator آخرین)
	{
		لكل (; اولی != آخرین; ++اولی)
			درج(*اولی);
	}

	std::pair<iterator, bool> درج(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(کلید, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(ثابت std::pair<K, T> &مقدار)
	{
		Hasher::hash_t hash = do_hash(مقدار.اولی);
		صحيح i = do_lookup(مقدار.اولی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(مقدار, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.اولی);
		صحيح i = do_lookup(rvalue.اولی, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::forward<std::pair<K, T>>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T ثابت &مقدار)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(کلید, مقدار), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(کلید, std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &مقدار)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(std::forward<K>(rkey), مقدار), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحيح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح شاخص = do_lookup(کلید, hash);
		أرجع do_erase(شاخص, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->اولی);
		do_erase(it.شاخص, hash);
		أرجع ++it;
	}

	صحيح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح شمار(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 || i > it.شاخص ? 0 : 1;
	}

	iterator پیدا_کن(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			أرجع انتها();
		أرجع iterator(this, i);
	}

	const_iterator پیدا_کن(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع انتها();
		أرجع const_iterator(this, i);
	}

	T& at(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			throw std::بیرون_از_range("dict::at()");
		أرجع entries[i].udata.دومی;
	}

	ثابت T& at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			throw std::بیرون_از_range("dict::at()");
		أرجع entries[i].udata.دومی;
	}

	ثابت T& at(ثابت K &کلید, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع entries[i].udata.دومی;
	}

	T& عملگر[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			i = do_درج(std::pair<K, T>(کلید, T()), hash);
		أرجع entries[i].udata.دومی;
	}

	قالب<typename Compare = std::less<K>>
	فراغ مرتب_سازی(Compare comp = Compare())
	{
		std::مرتب_سازی(entries.begin(), entries.انتها(), [comp](ثابت مدخل_t &a, ثابت مدخل_t &b){ أرجع comp(b.udata.اولی, a.udata.اولی); });
		do_rehash();
	}

	فراغ جابجا(dict &other)
	{
		hashtable.جابجا(other.hashtable);
		entries.جابجا(other.entries);
	}

	bool عملگر==(ثابت dict &other) ثابت {
		إذا (اندازه() != other.اندازه())
			أرجع false;
		لكل (خودکار &it : entries) {
			خودکار oit = other.پیدا_کن(it.udata.اولی);
			إذا (oit == other.انتها() || !(oit->دومی == it.udata.دومی))
				أرجع false;
		}
		أرجع true;
	}

	bool عملگر!=(ثابت dict &other) ثابت {
		أرجع !عملگر==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودکار &it : entries) {
			Hasher مدخل_hash;
			مدخل_hash.eat(it.udata.اولی);
			مدخل_hash.eat(it.udata.دومی);
			comm.eat(مدخل_hash);
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(اندازه_t n) { entries.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع entries.اندازه(); }
	bool خالی() ثابت { أرجع entries.خالی(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.اندازه())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.اندازه())-1-n); }
	iterator انتها() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1-n); }
	const_iterator انتها() ثابت { أرجع const_iterator(this, -1); }
};

قالب<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_استفادهنشده pool
{
	قالب<typename, صحيح, typename> دوست class idict;

محمي:
	بنية مدخل_t
	{
		K udata;
		صحيح بعدی;

		مدخل_t() { }
		مدخل_t(ثابت K &udata, صحيح بعدی) : udata(udata), بعدی(بعدی) { }
		مدخل_t(K &&udata, صحيح بعدی) : udata(std::move(udata)), بعدی(بعدی) { }
	};

	std::وکتور<صحيح> hashtable;
	std::وکتور<مدخل_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#اگر_تعریف_شده NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#پایان_اگر

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالی())
			hash = ops.hash(کلید).yield() % (بیعلامت صحيح)(hashtable.اندازه());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_اندازه(entries.ظرفیت() * hashtable_اندازه_ضریب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.اندازه()); i++) {
			do_assert(-1 <= entries[i].بعدی && entries[i].بعدی < صحيح(entries.اندازه()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].بعدی = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح شاخص, Hasher::hash_t hash)
	{
		do_assert(شاخص < صحيح(entries.اندازه()));
		إذا (hashtable.خالی() || شاخص < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		إذا (k == شاخص) {
			hashtable[hash] = entries[شاخص].بعدی;
		} وإلا {
			طالما (entries[k].بعدی != شاخص) {
				k = entries[k].بعدی;
				do_assert(0 <= k && k < صحيح(entries.اندازه()));
			}
			entries[k].بعدی = entries[شاخص].بعدی;
		}

		صحيح پشت_idx = entries.اندازه()-1;

		إذا (شاخص != پشت_idx)
		{
			Hasher::hash_t پشت_hash = do_hash(entries[پشت_idx].udata);

			k = hashtable[پشت_hash];
			إذا (k == پشت_idx) {
				hashtable[پشت_hash] = شاخص;
			} وإلا {
				طالما (entries[k].بعدی != پشت_idx) {
					k = entries[k].بعدی;
					do_assert(0 <= k && k < صحيح(entries.اندازه()));
				}
				entries[k].بعدی = شاخص;
			}

			entries[شاخص] = std::move(entries[پشت_idx]);
		}

		entries.برداشت_پشت();

		إذا (entries.خالی())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی())
			أرجع -1;

		إذا (entries.اندازه() * hashtable_اندازه_trigger > hashtable.اندازه()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		أرجع do_lookup_داخلی(کلید, hash);
	}

	صحيح do_lookup_داخلی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحيح شاخص = hashtable[hash];

		طالما (شاخص >= 0 && !ops.cmp(entries[شاخص].udata, کلید)) {
			شاخص = entries[شاخص].بعدی;
			do_assert(-1 <= شاخص && شاخص < صحيح(entries.اندازه()));
		}

		أرجع شاخص;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالی())
			أرجع -1;

		أرجع do_lookup_داخلی(کلید, hash);
	}

	صحيح do_درج(ثابت K &مقدار, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی()) {
			entries.emplace_پشت(مقدار, -1);
			do_rehash();
			hash = do_hash(مقدار);
		} وإلا {
			entries.emplace_پشت(مقدار, hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

	صحيح do_درج(K &&rvalue, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی()) {
			entries.emplace_پشت(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.پشت().udata);
		} وإلا {
			entries.emplace_پشت(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.اندازه() - 1;
		}
		أرجع entries.اندازه() - 1;
	}

عام:
	class const_iterator
	{
		دوست class pool;
	محمي:
		ثابت pool *ptr;
		صحيح شاخص;
		const_iterator(ثابت pool *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_برچسب iterator_category;
		تعريفنوع K مقدار_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_گر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عملگر++() { شاخص--; أرجع *this; }
		bool عملگر==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool عملگر!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت K &عملگر*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت K *عملگر->() ثابت { أرجع &ptr->entries[شاخص].udata; }
	};

	class iterator
	{
		دوست class pool;
	محمي:
		pool *ptr;
		صحيح شاخص;
		iterator(pool *ptr, صحيح شاخص) : ptr(ptr), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_برچسب iterator_category;
		تعريفنوع K مقدار_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_گر;
		تعريفنوع K& reference;
		iterator() { }
		iterator عملگر++() { شاخص--; أرجع *this; }
		bool عملگر==(ثابت iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool عملگر!=(ثابت iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		K &عملگر*() { أرجع ptr->entries[شاخص].udata; }
		K *عملگر->() { أرجع &ptr->entries[شاخص].udata; }
		ثابت K &عملگر*() ثابت { أرجع ptr->entries[شاخص].udata; }
		ثابت K *عملگر->() ثابت { أرجع &ptr->entries[شاخص].udata; }
		عملگر const_iterator() ثابت { أرجع const_iterator(ptr, شاخص); }
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
		جابجا(other);
	}

	pool &عملگر=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	pool &عملگر=(pool &&other) noexcept {
		clear();
		جابجا(other);
		أرجع *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		لكل (خودکار &it : list)
			درج(it);
	}

	قالب<class درون_داد_Iterator>
	pool(درون_داد_Iterator اولی, درون_داد_Iterator آخرین)
	{
		درج(اولی, آخرین);
	}

	قالب<class درون_داد_Iterator>
	فراغ درج(درون_داد_Iterator اولی, درون_داد_Iterator آخرین)
	{
		لكل (; اولی != آخرین; ++اولی)
			درج(*اولی);
	}

	std::pair<iterator, bool> درج(ثابت K &مقدار)
	{
		Hasher::hash_t hash = do_hash(مقدار);
		صحيح i = do_lookup(مقدار, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(مقدار, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> درج(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحيح i = do_lookup(rvalue, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_درج(std::forward<K>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	قالب<typename... آرگومانها_3>
	std::pair<iterator, bool> emplace(آرگومانها_3&&... آرگومانها_2)
	{
		أرجع درج(K(std::forward<آرگومانها_3>(آرگومانها_2)...));
	}

	صحيح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح شاخص = do_lookup(کلید, hash);
		أرجع do_erase(شاخص, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.شاخص, hash);
		أرجع ++it;
	}

	صحيح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح شمار(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 || i > it.شاخص ? 0 : 1;
	}

	iterator پیدا_کن(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			أرجع انتها();
		أرجع iterator(this, i);
	}

	const_iterator پیدا_کن(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع انتها();
		أرجع const_iterator(this, i);
	}

	bool عملگر[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		أرجع i >= 0;
	}

	قالب<typename Compare = std::less<K>>
	فراغ مرتب_سازی(Compare comp = Compare())
	{
		std::مرتب_سازی(entries.begin(), entries.انتها(), [comp](ثابت مدخل_t &a, ثابت مدخل_t &b){ أرجع comp(b.udata, a.udata); });
		do_rehash();
	}

	K برداشت()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		أرجع ret;
	}

	فراغ جابجا(pool &other)
	{
		hashtable.جابجا(other.hashtable);
		entries.جابجا(other.entries);
	}

	bool عملگر==(ثابت pool &other) ثابت {
		إذا (اندازه() != other.اندازه())
			أرجع false;
		لكل (خودکار &it : entries)
			إذا (!other.شمار(it.udata))
				أرجع false;
		أرجع true;
	}

	bool عملگر!=(ثابت pool &other) ثابت {
		أرجع !عملگر==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودکار &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(اندازه_t n) { entries.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع entries.اندازه(); }
	bool خالی() ثابت { أرجع entries.خالی(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.اندازه())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.اندازه())-1-n); }
	iterator انتها() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.اندازه())-1-n); }
	const_iterator انتها() ثابت { أرجع const_iterator(nullptr, -1); }
};

قالب<typename K, صحيح جابه_جایی, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_استفادهنشده idict
{
	pool<K, OPS> database;

عام:
	class const_iterator
	{
		دوست class idict;
	محمي:
		ثابت idict &container;
		صحيح شاخص;
		const_iterator(ثابت idict &container, صحيح شاخص) : container(container), شاخص(شاخص) { }
	عام:
		تعريفنوع std::forward_iterator_برچسب iterator_category;
		تعريفنوع K مقدار_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشاره_گر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عملگر++() { شاخص++; أرجع *this; }
		bool عملگر==(ثابت const_iterator &other) ثابت { أرجع شاخص == other.شاخص; }
		bool عملگر!=(ثابت const_iterator &other) ثابت { أرجع شاخص != other.شاخص; }
		ثابت K &عملگر*() ثابت { أرجع container[شاخص]; }
		ثابت K *عملگر->() ثابت { أرجع &container[شاخص]; }
	};

	constexpr idict()
	{
	}

	صحيح عملگر()(ثابت K &کلید)
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup(کلید, hash);
		إذا (i < 0)
			i = database.do_درج(کلید, hash);
		أرجع i + جابه_جایی;
	}

	صحيح at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			throw std::بیرون_از_range("idict::at()");
		أرجع i + جابه_جایی;
	}

	صحيح at(ثابت K &کلید, صحيح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع i + جابه_جایی;
	}

	صحيح شمار(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح lookup(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		أرجع database.do_lookup_no_rehash(کلید, hash);
	}

	فراغ expect(ثابت K &کلید, صحيح i)
	{
		صحيح j = (*this)(کلید);
		إذا (i != j)
			throw std::بیرون_از_range("idict::expect()");
	}

	ثابت K &عملگر[](صحيح شاخص) ثابت
	{
		أرجع database.entries.at(شاخص - جابه_جایی).udata;
	}

	فراغ جابجا(idict &other)
	{
		database.جابجا(other.database);
	}

	فراغ reserve(اندازه_t n) { database.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع database.اندازه(); }
	bool خالی() ثابت { أرجع database.خالی(); }
	فراغ clear() { database.clear(); }

	const_iterator begin() ثابت { أرجع const_iterator(*this, جابه_جایی); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(*this, n); }
	const_iterator انتها() ثابت { أرجع const_iterator(*this, جابه_جایی + اندازه()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
قالب<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_استفادهنشده mfp
{
	idict<K, 0, OPS> database;
	class Atomic_والد {
	عام:
		explicit Atomic_والد(صحيح p) : والد(p) {}
		Atomic_والد(ثابت Atomic_والد &other) : والد(other.get()) {}
		Atomic_والد &عملگر=(ثابت Atomic_والد &other) { مجموعه(other.get()); أرجع *this; }
		صحيح get() ثابت { أرجع والد.بارگیری(std::حافظه_ترتیب_relaxed); }
		فراغ مجموعه(صحيح p) { والد.store(p, std::حافظه_ترتیب_relaxed); }
	خاص:
		std::atomic<صحيح> والد;
	};
	std::وکتور<Atomic_والد> parents;

عام:
	تعريفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحيح عملگر()(ثابت K &کلید)
	{
		صحيح i = database(کلید);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		إذا (parents.اندازه() < database.اندازه()) {
			parents.emplace_پشت(-1);
		}
		أرجع i;
	}

	// Finds an element at given index
	ثابت K &عملگر[](صحيح شاخص) ثابت
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
			صحيح بعدی_k = parents[k].get();
			const_cast<Atomic_والد*>(&parents[k])->مجموعه(p);
			k = بعدی_k;
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
			parents[i].مجموعه(j);
	}

	فراغ ipromote(صحيح i)
	{
		صحيح k = i;

		طالما (k != -1) {
			صحيح بعدی_k = parents[k].get();
			parents[k].مجموعه(i);
			k = بعدی_k;
		}

		parents[i].مجموعه(-1);
	}

	صحيح lookup(ثابت K &a)
	{
		أرجع ifind((*this)(a));
	}

	ثابت K &پیدا_کن(ثابت K &a) ثابت
	{
		صحيح i = database.at(a, -1);
		إذا (i < 0)
			أرجع a;
		أرجع (*this)[ifind(i)];
	}

	فراغ ادغام(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	فراغ promote(ثابت K &a)
	{
		صحيح i = database.at(a, -1);
		إذا (i >= 0)
			ipromote(i);
	}

	فراغ جابجا(mfp &other)
	{
		database.جابجا(other.database);
		parents.جابجا(other.parents);
	}

	فراغ reserve(اندازه_t n) { database.reserve(n); }
	اندازه_t اندازه() ثابت { أرجع database.اندازه(); }
	bool خالی() ثابت { أرجع database.خالی(); }
	فراغ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { أرجع database.begin(); }
	const_iterator element(صحيح n) ثابت { أرجع database.element(n); }
	const_iterator انتها() ثابت { أرجع database.انتها(); }
};

} /* namespace hashlib */

#پایان_اگر
