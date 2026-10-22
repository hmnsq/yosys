// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#جے_بنا_تعریف HASHLIB_H
#تعریف_کرو HASHLIB_H

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

#تعریف_کرو YS_HASHING_نسخہ 1

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
#  define HASHLIB_ATTRIBUTE_WARN_بن_ورتیا __attribute__((warn_بن_ورتیا))
#وإلا
#  define HASHLIB_ATTRIBUTE_WARN_بن_ورتیا
#جے_انت

ثابت صحيح hashtable_ناپ_trigger = 2;
ثابت صحيح hashtable_ناپ_ضارب = 3;

namespace legacy {
	مضمن uint32_t djb2_جوڑو(uint32_t a, uint32_t b) {
		أرجع ((a << 5) + a) + b;
	}
};

سانچا<typename T>
بنية hash_ops;

مضمن بنانشانی صحيح mkhash_xorshift(بنانشانی صحيح a) {
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

	سانچا<typename T>
	فراغ eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	سانچا<typename T>
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
#تعریف_کرو HASH_چوٹی_LOOP_FST [[nodiscard]] ساكن مضمن Hasher hash
#تعریف_کرو HASH_چوٹی_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	أرجع h; \
}

سانچا<typename T>
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
		} وإلا إذا constexpr (std::is_اشارہ_گر_v<T>) {
			أرجع hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} وإلا إذا constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحيح ناپ = a.ناپ();
			صحيح i = 0;
			طالما (i + 8 < ناپ) {
				uint64_t v;
				حافظہ_نقل(&v, a.انکڑے() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			حافظہ_نقل(&v, a.انکڑے() + i, ناپ - i);
			h.hash64(v);
			أرجع h;
		} وإلا {
			أرجع a.hash_into(h);
		}
	}
	HASH_چوٹی_LOOP_FST (ثابت T &a) HASH_چوٹی_LOOP_SND
};

سانچا<typename P, typename Q> بنية hash_ops<std::pair<P, Q>> {
	ساكن مضمن bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.پہلا, h);
		h = hash_ops<Q>::hash_into(a.دوجا, h);
		أرجع h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_چوٹی_LOOP_SND
	[[nodiscard]] ساكن مضمن Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		أرجع h;
	}
};

سانچا<typename... T> بنية hash_ops<std::tuple<T...>> {
	ساكن مضمن bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		أرجع a == b;
	}
	سانچا<ناپ_t I = 0>
	ساكن مضمن typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		أرجع h;
	}
	سانچا<ناپ_t I = 0>
	ساكن مضمن typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		تعريفنوع hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		أرجع h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::tuple<T...> &a) HASH_چوٹی_LOOP_SND
};

سانچا<typename T> بنية hash_ops<std::سمتیہ<T>> {
	ساكن مضمن bool cmp(ثابت std::سمتیہ<T> &a, ثابت std::سمتیہ<T> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::سمتیہ<T> &a, Hasher h) {
		h.eat((uint32_t)a.ناپ());
		لكل (خودکار k : a)
			h.eat(k);
		أرجع h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::سمتیہ<T> &a) HASH_چوٹی_LOOP_SND
};

سانچا<typename T, ناپ_t N> بنية hash_ops<std::array<T, N>> {
    ساكن مضمن bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        أرجع a == b;
    }
    [[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        لكل (ثابت خودکار& k : a)
            h = hash_ops<T>::hash_into(k, h);
        أرجع h;
    }
	HASH_چوٹی_LOOP_FST (ثابت std::array<T, N> &a) HASH_چوٹی_LOOP_SND
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
	HASH_چوٹی_LOOP_FST (ثابت حرف *a) HASH_چوٹی_LOOP_SND
};

سانچا <> بنية hash_ops<حرف*> : hash_cstr_ops {};

بنية hash_ptr_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت فراغ *a, Hasher h) {
		أرجع hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_چوٹی_LOOP_FST (ثابت فراغ *a) HASH_چوٹی_LOOP_SND
};

بنية hash_obj_ops {
	ساكن مضمن bool cmp(ثابت فراغ *a, ثابت فراغ *b) {
		أرجع a == b;
	}
	سانچا<typename T>
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T *a, Hasher h) {
		إذا (a)
			h = a->hash_into(h);
		وإلا
			h.eat(0);
		أرجع h;
	}
	سانچا<typename T>
	HASH_چوٹی_LOOP_FST (ثابت T *a) HASH_چوٹی_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
سانچا<typename T>
[[nodiscard]]
Hasher::hash_t لگاتار_hash(ثابت T& obj) {
	أرجع hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
سانچا<typename T>
[[nodiscard]]
[[deprecated]]
مضمن بنانشانی صحيح mkhash(ثابت T &v) {
	أرجع (بنانشانی صحيح) لگاتار_hash<T>(v);
}

سانچا<> بنية hash_ops<std::monostate> {
	ساكن مضمن bool cmp(std::monostate a, std::monostate b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(std::monostate, Hasher h) {
		أرجع h;
	}
};

سانچا<typename... T> بنية hash_ops<std::variant<T...>> {
	ساكن مضمن bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		أرجع a == b;
	}
	[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::ویکھو([& h](ثابت خودکار &v) { h.eat(v); }, a);
		h.eat(a.اشاریہ());
		أرجع h;
	}
};

سانچا<typename T> بنية hash_ops<std::optional<T>> {
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

مضمن بنانشانی صحيح hashtable_ناپ(بنانشانی صحيح min_ناپ)
{
	// Primes as generated by https://oeis.org/A175953
	ساكن std::سمتیہ<بنانشانی صحيح> zero_and_some_مفرد_عدد = {
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

	لكل (خودکار p : zero_and_some_مفرد_عدد)
		إذا (p >= min_ناپ) أرجع p;

	إذا (حجم(بنانشانی صحيح) == 4)
		throw std::لمائی_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	لكل (خودکار p : zero_and_some_مفرد_عدد)
		إذا (100129 * p > min_ناپ) أرجع 100129 * p;

	throw std::لمائی_error("hash table exceeded maximum size.");
}

سانچا<typename K, typename T, typename OPS = hash_ops<K>> class dict;
سانچا<typename K, صحيح وتھ = 0, typename OPS = hash_ops<K>> class idict;
سانچا<typename K, typename OPS = hash_ops<K>> class pool;
سانچا<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عام:
	commutative_hash() {
		buckets.fill(0);
	}
	سانچا <typename T>
	فراغ eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	فراغ eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		ناپ_t اشاریہ = v & (buckets.ناپ() - 1);
		buckets[اشاریہ] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		لكل (خودکار b : buckets)
			h.eat(b);
		أرجع h;
	}
خاص:
	std::array<Hasher::hash_t, 4> buckets;
};

سانچا<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_بن_ورتیا dict {
	بنية اندراج_t
	{
		std::pair<K, T> udata;
		صحيح اگلا;

		اندراج_t() { }
		اندراج_t(ثابت std::pair<K, T> &udata, صحيح اگلا) : udata(udata), اگلا(اگلا) { }
		اندراج_t(std::pair<K, T> &&udata, صحيح اگلا) : udata(std::move(udata)), اگلا(اگلا) { }
		bool عامل<(ثابت اندراج_t &other) ثابت { أرجع udata.پہلا < other.udata.پہلا; }
	};

	std::سمتیہ<صحيح> hashtable;
	std::سمتیہ<اندراج_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#جے_تعریف_شدہ NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#جے_انت

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالی_تھاواں())
			hash = ops.hash(کلید).yield() % (بنانشانی صحيح)(hashtable.ناپ());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ناپ(entries.سمرتھا() * hashtable_ناپ_ضارب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.ناپ()); i++) {
			do_assert(-1 <= entries[i].اگلا && entries[i].اگلا < صحيح(entries.ناپ()));
			Hasher::hash_t hash = do_hash(entries[i].udata.پہلا);
			entries[i].اگلا = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح اشاریہ, Hasher::hash_t hash)
	{
		do_assert(اشاریہ < صحيح(entries.ناپ()));
		إذا (hashtable.خالی_تھاواں() || اشاریہ < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		do_assert(0 <= k && k < صحيح(entries.ناپ()));

		إذا (k == اشاریہ) {
			hashtable[hash] = entries[اشاریہ].اگلا;
		} وإلا {
			طالما (entries[k].اگلا != اشاریہ) {
				k = entries[k].اگلا;
				do_assert(0 <= k && k < صحيح(entries.ناپ()));
			}
			entries[k].اگلا = entries[اشاریہ].اگلا;
		}

		صحيح پٹھ_idx = entries.ناپ()-1;

		إذا (اشاریہ != پٹھ_idx)
		{
			Hasher::hash_t پٹھ_hash = do_hash(entries[پٹھ_idx].udata.پہلا);

			k = hashtable[پٹھ_hash];
			do_assert(0 <= k && k < صحيح(entries.ناپ()));

			إذا (k == پٹھ_idx) {
				hashtable[پٹھ_hash] = اشاریہ;
			} وإلا {
				طالما (entries[k].اگلا != پٹھ_idx) {
					k = entries[k].اگلا;
					do_assert(0 <= k && k < صحيح(entries.ناپ()));
				}
				entries[k].اگلا = اشاریہ;
			}

			entries[اشاریہ] = std::move(entries[پٹھ_idx]);
		}

		entries.نکالو_پٹھ();

		إذا (entries.خالی_تھاواں())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں())
			أرجع -1;

		إذا (entries.ناپ() * hashtable_ناپ_trigger > hashtable.ناپ()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		أرجع do_lookup_اندرونی(کلید, hash);
	}

	صحيح do_lookup_اندرونی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحيح اشاریہ = hashtable[hash];

		طالما (اشاریہ >= 0 && !ops.cmp(entries[اشاریہ].udata.پہلا, کلید)) {
			اشاریہ = entries[اشاریہ].اگلا;
			do_assert(-1 <= اشاریہ && اشاریہ < صحيح(entries.ناپ()));
		}

		أرجع اشاریہ;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالی_تھاواں())
			أرجع -1;

		أرجع do_lookup_اندرونی(کلید, hash);
	}

	صحيح do_داخل_کرو(ثابت K &کلید, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں()) {
			entries.emplace_پٹھ(std::pair<K, T>(کلید, T()), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٹھ(std::pair<K, T>(کلید, T()), hashtable[hash]);
			hashtable[hash] = entries.ناپ() - 1;
		}
		أرجع entries.ناپ() - 1;
	}

	صحيح do_داخل_کرو(ثابت std::pair<K, T> &قدر, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں()) {
			entries.emplace_پٹھ(قدر, -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٹھ(قدر, hashtable[hash]);
			hashtable[hash] = entries.ناپ() - 1;
		}
		أرجع entries.ناپ() - 1;
	}

	صحيح do_داخل_کرو(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں()) {
			entries.emplace_پٹھ(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} وإلا {
			entries.emplace_پٹھ(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ناپ() - 1;
		}
		أرجع entries.ناپ() - 1;
	}

عام:
	class const_iterator
	{
		یار class dict;
	محمي:
		ثابت dict *ptr;
		صحيح اشاریہ;
		const_iterator(ثابت dict *ptr, صحيح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عام:
		تعريفنوع std::bidirectional_iterator_ٹیگ iterator_category;
		تعريفنوع std::pair<K, T> قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع ثابت std::pair<K, T>* اشارہ_گر;
		تعريفنوع ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ--; أرجع *this; }
		const_iterator عامل++(صحيح) { const_iterator tmp = *this; اشاریہ--; أرجع tmp; }
		const_iterator عامل--() { اشاریہ++; أرجع *this; }
		const_iterator عامل--(صحيح) { const_iterator tmp = *this; اشاریہ++; أرجع tmp; }
		const_iterator عامل+=(صحيح amt) { اشاریہ -= amt; أرجع *this; }
		bool عامل<(ثابت const_iterator &other) ثابت { أرجع اشاریہ > other.اشاریہ; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
		ثابت std::pair<K, T> &عامل*() ثابت { أرجع ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { أرجع &ptr->entries[اشاریہ].udata; }
	};

	class iterator
	{
		یار class dict;
	محمي:
		dict *ptr;
		صحيح اشاریہ;
		iterator(dict *ptr, صحيح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عام:
		تعريفنوع std::forward_iterator_ٹیگ iterator_category;
		تعريفنوع std::pair<K, T> قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع std::pair<K, T>* اشارہ_گر;
		تعريفنوع std::pair<K, T>& reference;
		iterator() { }
		iterator عامل++() { اشاریہ--; أرجع *this; }
		iterator عامل+=(صحيح amt) { اشاریہ -= amt; أرجع *this; }
		bool عامل<(ثابت iterator &other) ثابت { أرجع اشاریہ > other.اشاریہ; }
		bool عامل==(ثابت iterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت iterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
		std::pair<K, T> &عامل*() { أرجع ptr->entries[اشاریہ].udata; }
		std::pair<K, T> *عامل->() { أرجع &ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> &عامل*() ثابت { أرجع ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { أرجع &ptr->entries[اشاریہ].udata; }
		عامل const_iterator() ثابت { أرجع const_iterator(ptr, اشاریہ); }
	};
	using الٹ_iterator = std::الٹ_iterator<const_iterator>;
	الٹ_iterator rbegin() ثابت {
		أرجع std::make_الٹ_iterator(سرا());
	}
	الٹ_iterator rend() ثابت {
		أرجع std::make_الٹ_iterator(begin());
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
		وٹاندرا(other);
	}

	dict &عامل=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	dict &عامل=(dict &&other) noexcept {
		clear();
		وٹاندرا(other);
		أرجع *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		لكل (خودکار &it : list)
			داخل_کرو(it);
	}

	سانچا<class داخلہ_Iterator>
	dict(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		داخل_کرو(پہلا, آخری);
	}

	سانچا<class داخلہ_Iterator>
	فراغ داخل_کرو(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		لكل (; پہلا != آخری; ++پہلا)
			داخل_کرو(*پہلا);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(کلید, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت std::pair<K, T> &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر.پہلا);
		صحيح i = do_lookup(قدر.پہلا, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(قدر, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.پہلا);
		صحيح i = do_lookup(rvalue.پہلا, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::forward<std::pair<K, T>>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(کلید, قدر), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(کلید, std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(std::forward<K>(rkey), قدر), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحيح i = do_lookup(rkey, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحيح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح اشاریہ = do_lookup(کلید, hash);
		أرجع do_erase(اشاریہ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->پہلا);
		do_erase(it.اشاریہ, hash);
		أرجع ++it;
	}

	صحيح گنتی(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح گنتی(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 || i > it.اشاریہ ? 0 : 1;
	}

	iterator لبھو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			أرجع سرا();
		أرجع iterator(this, i);
	}

	const_iterator لبھو(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع سرا();
		أرجع const_iterator(this, i);
	}

	T& at(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			throw std::باہرلا_دا_range("dict::at()");
		أرجع entries[i].udata.دوجا;
	}

	ثابت T& at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			throw std::باہرلا_دا_range("dict::at()");
		أرجع entries[i].udata.دوجا;
	}

	ثابت T& at(ثابت K &کلید, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع entries[i].udata.دوجا;
	}

	T& عامل[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			i = do_داخل_کرو(std::pair<K, T>(کلید, T()), hash);
		أرجع entries[i].udata.دوجا;
	}

	سانچا<typename Compare = std::less<K>>
	فراغ ترتیب_دیو(Compare comp = Compare())
	{
		std::ترتیب_دیو(entries.begin(), entries.سرا(), [comp](ثابت اندراج_t &a, ثابت اندراج_t &b){ أرجع comp(b.udata.پہلا, a.udata.پہلا); });
		do_rehash();
	}

	فراغ وٹاندرا(dict &other)
	{
		hashtable.وٹاندرا(other.hashtable);
		entries.وٹاندرا(other.entries);
	}

	bool عامل==(ثابت dict &other) ثابت {
		إذا (ناپ() != other.ناپ())
			أرجع false;
		لكل (خودکار &it : entries) {
			خودکار oit = other.لبھو(it.udata.پہلا);
			إذا (oit == other.سرا() || !(oit->دوجا == it.udata.دوجا))
				أرجع false;
		}
		أرجع true;
	}

	bool عامل!=(ثابت dict &other) ثابت {
		أرجع !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودکار &it : entries) {
			Hasher اندراج_hash;
			اندراج_hash.eat(it.udata.پہلا);
			اندراج_hash.eat(it.udata.دوجا);
			comm.eat(اندراج_hash);
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(ناپ_t n) { entries.reserve(n); }
	ناپ_t ناپ() ثابت { أرجع entries.ناپ(); }
	bool خالی_تھاواں() ثابت { أرجع entries.خالی_تھاواں(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.ناپ())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.ناپ())-1-n); }
	iterator سرا() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.ناپ())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.ناپ())-1-n); }
	const_iterator سرا() ثابت { أرجع const_iterator(this, -1); }
};

سانچا<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_بن_ورتیا pool
{
	سانچا<typename, صحيح, typename> یار class idict;

محمي:
	بنية اندراج_t
	{
		K udata;
		صحيح اگلا;

		اندراج_t() { }
		اندراج_t(ثابت K &udata, صحيح اگلا) : udata(udata), اگلا(اگلا) { }
		اندراج_t(K &&udata, صحيح اگلا) : udata(std::move(udata)), اگلا(اگلا) { }
	};

	std::سمتیہ<صحيح> hashtable;
	std::سمتیہ<اندراج_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#جے_تعریف_شدہ NDEBUG
	ساكن مضمن فراغ do_assert(bool) { }
#وإلا
	ساكن مضمن فراغ do_assert(bool cond) {
		إذا (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#جے_انت

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		إذا (!hashtable.خالی_تھاواں())
			hash = ops.hash(کلید).yield() % (بنانشانی صحيح)(hashtable.ناپ());
		أرجع hash;
	}

	فراغ do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_ناپ(entries.سمرتھا() * hashtable_ناپ_ضارب), -1);

		لكل (صحيح i = 0; i < صحيح(entries.ناپ()); i++) {
			do_assert(-1 <= entries[i].اگلا && entries[i].اگلا < صحيح(entries.ناپ()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].اگلا = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحيح do_erase(صحيح اشاریہ, Hasher::hash_t hash)
	{
		do_assert(اشاریہ < صحيح(entries.ناپ()));
		إذا (hashtable.خالی_تھاواں() || اشاریہ < 0)
			أرجع 0;

		صحيح k = hashtable[hash];
		إذا (k == اشاریہ) {
			hashtable[hash] = entries[اشاریہ].اگلا;
		} وإلا {
			طالما (entries[k].اگلا != اشاریہ) {
				k = entries[k].اگلا;
				do_assert(0 <= k && k < صحيح(entries.ناپ()));
			}
			entries[k].اگلا = entries[اشاریہ].اگلا;
		}

		صحيح پٹھ_idx = entries.ناپ()-1;

		إذا (اشاریہ != پٹھ_idx)
		{
			Hasher::hash_t پٹھ_hash = do_hash(entries[پٹھ_idx].udata);

			k = hashtable[پٹھ_hash];
			إذا (k == پٹھ_idx) {
				hashtable[پٹھ_hash] = اشاریہ;
			} وإلا {
				طالما (entries[k].اگلا != پٹھ_idx) {
					k = entries[k].اگلا;
					do_assert(0 <= k && k < صحيح(entries.ناپ()));
				}
				entries[k].اگلا = اشاریہ;
			}

			entries[اشاریہ] = std::move(entries[پٹھ_idx]);
		}

		entries.نکالو_پٹھ();

		إذا (entries.خالی_تھاواں())
			hashtable.clear();

		أرجع 1;
	}

	صحيح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں())
			أرجع -1;

		إذا (entries.ناپ() * hashtable_ناپ_trigger > hashtable.ناپ()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		أرجع do_lookup_اندرونی(کلید, hash);
	}

	صحيح do_lookup_اندرونی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحيح اشاریہ = hashtable[hash];

		طالما (اشاریہ >= 0 && !ops.cmp(entries[اشاریہ].udata, کلید)) {
			اشاریہ = entries[اشاریہ].اگلا;
			do_assert(-1 <= اشاریہ && اشاریہ < صحيح(entries.ناپ()));
		}

		أرجع اشاریہ;
	}

	صحيح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		إذا (hashtable.خالی_تھاواں())
			أرجع -1;

		أرجع do_lookup_اندرونی(کلید, hash);
	}

	صحيح do_داخل_کرو(ثابت K &قدر, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں()) {
			entries.emplace_پٹھ(قدر, -1);
			do_rehash();
			hash = do_hash(قدر);
		} وإلا {
			entries.emplace_پٹھ(قدر, hashtable[hash]);
			hashtable[hash] = entries.ناپ() - 1;
		}
		أرجع entries.ناپ() - 1;
	}

	صحيح do_داخل_کرو(K &&rvalue, Hasher::hash_t &hash)
	{
		إذا (hashtable.خالی_تھاواں()) {
			entries.emplace_پٹھ(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.پٹھ().udata);
		} وإلا {
			entries.emplace_پٹھ(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.ناپ() - 1;
		}
		أرجع entries.ناپ() - 1;
	}

عام:
	class const_iterator
	{
		یار class pool;
	محمي:
		ثابت pool *ptr;
		صحيح اشاریہ;
		const_iterator(ثابت pool *ptr, صحيح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عام:
		تعريفنوع std::forward_iterator_ٹیگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارہ_گر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ--; أرجع *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
		ثابت K &عامل*() ثابت { أرجع ptr->entries[اشاریہ].udata; }
		ثابت K *عامل->() ثابت { أرجع &ptr->entries[اشاریہ].udata; }
	};

	class iterator
	{
		یار class pool;
	محمي:
		pool *ptr;
		صحيح اشاریہ;
		iterator(pool *ptr, صحيح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عام:
		تعريفنوع std::forward_iterator_ٹیگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارہ_گر;
		تعريفنوع K& reference;
		iterator() { }
		iterator عامل++() { اشاریہ--; أرجع *this; }
		bool عامل==(ثابت iterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت iterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
		K &عامل*() { أرجع ptr->entries[اشاریہ].udata; }
		K *عامل->() { أرجع &ptr->entries[اشاریہ].udata; }
		ثابت K &عامل*() ثابت { أرجع ptr->entries[اشاریہ].udata; }
		ثابت K *عامل->() ثابت { أرجع &ptr->entries[اشاریہ].udata; }
		عامل const_iterator() ثابت { أرجع const_iterator(ptr, اشاریہ); }
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
		وٹاندرا(other);
	}

	pool &عامل=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		أرجع *this;
	}

	pool &عامل=(pool &&other) noexcept {
		clear();
		وٹاندرا(other);
		أرجع *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		لكل (خودکار &it : list)
			داخل_کرو(it);
	}

	سانچا<class داخلہ_Iterator>
	pool(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		داخل_کرو(پہلا, آخری);
	}

	سانچا<class داخلہ_Iterator>
	فراغ داخل_کرو(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		لكل (; پہلا != آخری; ++پہلا)
			داخل_کرو(*پہلا);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت K &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر);
		صحيح i = do_lookup(قدر, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(قدر, hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحيح i = do_lookup(rvalue, hash);
		إذا (i >= 0)
			أرجع std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::forward<K>(rvalue), hash);
		أرجع std::pair<iterator, bool>(iterator(this, i), true);
	}

	سانچا<typename... دلیلاں_3>
	std::pair<iterator, bool> emplace(دلیلاں_3&&... دلیلاں_2)
	{
		أرجع داخل_کرو(K(std::forward<دلیلاں_3>(دلیلاں_2)...));
	}

	صحيح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح اشاریہ = do_lookup(کلید, hash);
		أرجع do_erase(اشاریہ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.اشاریہ, hash);
		أرجع ++it;
	}

	صحيح گنتی(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 ? 0 : 1;
	}

	صحيح گنتی(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		أرجع i < 0 || i > it.اشاریہ ? 0 : 1;
	}

	iterator لبھو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		إذا (i < 0)
			أرجع سرا();
		أرجع iterator(this, i);
	}

	const_iterator لبھو(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع سرا();
		أرجع const_iterator(this, i);
	}

	bool عامل[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحيح i = do_lookup(کلید, hash);
		أرجع i >= 0;
	}

	سانچا<typename Compare = std::less<K>>
	فراغ ترتیب_دیو(Compare comp = Compare())
	{
		std::ترتیب_دیو(entries.begin(), entries.سرا(), [comp](ثابت اندراج_t &a, ثابت اندراج_t &b){ أرجع comp(b.udata, a.udata); });
		do_rehash();
	}

	K نکالو()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		أرجع ret;
	}

	فراغ وٹاندرا(pool &other)
	{
		hashtable.وٹاندرا(other.hashtable);
		entries.وٹاندرا(other.entries);
	}

	bool عامل==(ثابت pool &other) ثابت {
		إذا (ناپ() != other.ناپ())
			أرجع false;
		لكل (خودکار &it : entries)
			إذا (!other.گنتی(it.udata))
				أرجع false;
		أرجع true;
	}

	bool عامل!=(ثابت pool &other) ثابت {
		أرجع !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		لكل (خودکار &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		أرجع comm.hash_into(h);
	}

	فراغ reserve(ناپ_t n) { entries.reserve(n); }
	ناپ_t ناپ() ثابت { أرجع entries.ناپ(); }
	bool خالی_تھاواں() ثابت { أرجع entries.خالی_تھاواں(); }
	فراغ clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { أرجع iterator(this, صحيح(entries.ناپ())-1); }
	iterator element(صحيح n) { أرجع iterator(this, صحيح(entries.ناپ())-1-n); }
	iterator سرا() { أرجع iterator(nullptr, -1); }

	const_iterator begin() ثابت { أرجع const_iterator(this, صحيح(entries.ناپ())-1); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(this, صحيح(entries.ناپ())-1-n); }
	const_iterator سرا() ثابت { أرجع const_iterator(nullptr, -1); }
};

سانچا<typename K, صحيح وتھ, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_بن_ورتیا idict
{
	pool<K, OPS> database;

عام:
	class const_iterator
	{
		یار class idict;
	محمي:
		ثابت idict &container;
		صحيح اشاریہ;
		const_iterator(ثابت idict &container, صحيح اشاریہ) : container(container), اشاریہ(اشاریہ) { }
	عام:
		تعريفنوع std::forward_iterator_ٹیگ iterator_category;
		تعريفنوع K قدر_type;
		تعريفنوع ptrdiff_t difference_type;
		تعريفنوع K* اشارہ_گر;
		تعريفنوع K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ++; أرجع *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
		ثابت K &عامل*() ثابت { أرجع container[اشاریہ]; }
		ثابت K *عامل->() ثابت { أرجع &container[اشاریہ]; }
	};

	constexpr idict()
	{
	}

	صحيح عامل()(ثابت K &کلید)
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup(کلید, hash);
		إذا (i < 0)
			i = database.do_داخل_کرو(کلید, hash);
		أرجع i + وتھ;
	}

	صحيح at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			throw std::باہرلا_دا_range("idict::at()");
		أرجع i + وتھ;
	}

	صحيح at(ثابت K &کلید, صحيح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحيح i = database.do_lookup_no_rehash(کلید, hash);
		إذا (i < 0)
			أرجع defval;
		أرجع i + وتھ;
	}

	صحيح گنتی(ثابت K &کلید) ثابت
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
			throw std::باہرلا_دا_range("idict::expect()");
	}

	ثابت K &عامل[](صحيح اشاریہ) ثابت
	{
		أرجع database.entries.at(اشاریہ - وتھ).udata;
	}

	فراغ وٹاندرا(idict &other)
	{
		database.وٹاندرا(other.database);
	}

	فراغ reserve(ناپ_t n) { database.reserve(n); }
	ناپ_t ناپ() ثابت { أرجع database.ناپ(); }
	bool خالی_تھاواں() ثابت { أرجع database.خالی_تھاواں(); }
	فراغ clear() { database.clear(); }

	const_iterator begin() ثابت { أرجع const_iterator(*this, وتھ); }
	const_iterator element(صحيح n) ثابت { أرجع const_iterator(*this, n); }
	const_iterator سرا() ثابت { أرجع const_iterator(*this, وتھ + ناپ()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
سانچا<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_بن_ورتیا mfp
{
	idict<K, 0, OPS> database;
	class Atomic_ماپا {
	عام:
		explicit Atomic_ماپا(صحيح p) : ماپا(p) {}
		Atomic_ماپا(ثابت Atomic_ماپا &other) : ماپا(other.get()) {}
		Atomic_ماپا &عامل=(ثابت Atomic_ماپا &other) { سیٹ(other.get()); أرجع *this; }
		صحيح get() ثابت { أرجع ماپا.لدو(std::حافظہ_ترتیب_relaxed); }
		فراغ سیٹ(صحيح p) { ماپا.store(p, std::حافظہ_ترتیب_relaxed); }
	خاص:
		std::atomic<صحيح> ماپا;
	};
	std::سمتیہ<Atomic_ماپا> parents;

عام:
	تعريفنوع typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحيح عامل()(ثابت K &کلید)
	{
		صحيح i = database(کلید);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		إذا (parents.ناپ() < database.ناپ()) {
			parents.emplace_پٹھ(-1);
		}
		أرجع i;
	}

	// Finds an element at given index
	ثابت K &عامل[](صحيح اشاریہ) ثابت
	{
		أرجع database[اشاریہ];
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
			صحيح اگلا_k = parents[k].get();
			const_cast<Atomic_ماپا*>(&parents[k])->سیٹ(p);
			k = اگلا_k;
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
			parents[i].سیٹ(j);
	}

	فراغ ipromote(صحيح i)
	{
		صحيح k = i;

		طالما (k != -1) {
			صحيح اگلا_k = parents[k].get();
			parents[k].سیٹ(i);
			k = اگلا_k;
		}

		parents[i].سیٹ(-1);
	}

	صحيح lookup(ثابت K &a)
	{
		أرجع ifind((*this)(a));
	}

	ثابت K &لبھو(ثابت K &a) ثابت
	{
		صحيح i = database.at(a, -1);
		إذا (i < 0)
			أرجع a;
		أرجع (*this)[ifind(i)];
	}

	فراغ ملاؤ(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	فراغ promote(ثابت K &a)
	{
		صحيح i = database.at(a, -1);
		إذا (i >= 0)
			ipromote(i);
	}

	فراغ وٹاندرا(mfp &other)
	{
		database.وٹاندرا(other.database);
		parents.وٹاندرا(other.parents);
	}

	فراغ reserve(ناپ_t n) { database.reserve(n); }
	ناپ_t ناپ() ثابت { أرجع database.ناپ(); }
	bool خالی_تھاواں() ثابت { أرجع database.خالی_تھاواں(); }
	فراغ clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { أرجع database.begin(); }
	const_iterator element(صحيح n) ثابت { أرجع database.element(n); }
	const_iterator سرا() ثابت { أرجع database.سرا(); }
};

} /* namespace hashlib */

#جے_انت
