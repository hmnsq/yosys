// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// -------------------------------------------------------
// Written by Claire Xenia Wolf <claire@yosyshq.com> in 2014
// -------------------------------------------------------

#اگر_غیر_تعریف_شدہ HASHLIB_H
#تعریف_کرو HASHLIB_H

#شامل <array>
#شامل <atomic>
#شامل <stdexcept>
#شامل <algorithm>
#شامل <optional>
#شامل <string>
#شامل <variant>
#شامل <vector>
#شامل <type_traits>
#شامل <stdint.h>

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

#اگر defined(__GNUC__) || defined(__clang__)
#  define HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل __attribute__((warn_غیر_مستعمل))
#ورنہ
#  define HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل
#اگر_ختم

ثابت صحیح hashtable_سائز_trigger = 2;
ثابت صحیح hashtable_سائز_ضربی_عدد = 3;

namespace legacy {
	اندرونی uint32_t djb2_جوڑو(uint32_t a, uint32_t b) {
		واپس ((a << 5) + a) + b;
	}
};

سانچہ<typename T>
ساخت hash_ops;

اندرونی غیرعلامتی صحیح mkhash_xorshift(غیرعلامتی صحیح a) {
	اگر (حجم(a) == 4) {
		a ^= a << 13;
		a ^= a >> 17;
		a ^= a << 5;
	} ورنہ اگر (حجم(a) == 8) {
		a ^= a << 13;
		a ^= a >> 7;
		a ^= a << 17;
	} ورنہ
		throw std::runtime_error("mkhash_xorshift() only implemented for 32 bit and 64 bit ints");
	واپس a;
}

class HasherDJB32 {
عوامی:
	using hash_t = uint32_t;

	HasherDJB32() {
		// traditionally 5381 is used as starting value for the djb2 hash
		state = 5381;
	}

نجی:
	uint32_t state;
	// The XOR version of DJB2
	[[nodiscard]]
	ساکن uint32_t djb2_xor(uint32_t a, uint32_t b) {
		uint32_t hash = ((a << 5) + a) ^ b;
		واپس hash;
	}
عوامی:
	خالی hash32(uint32_t i) {
		state = djb2_xor(i, state);
		state = mkhash_xorshift(state);
		واپس;
	}
	خالی hash64(uint64_t i) {
		state = djb2_xor((uint32_t)(i & 0xFFFFFFFFULL), state);
		state = djb2_xor((uint32_t)(i >> 32ULL), state);
		state = mkhash_xorshift(state);
		واپس;
	}
	[[nodiscard]]
	hash_t yield() ثابت {
		واپس (hash_t)state;
	}

	سانچہ<typename T>
	خالی eat(T&& t) {
		*this = hash_ops<std::remove_cv_t<std::remove_reference_t<T>>>::hash_into(std::forward<T>(t), *this);
	}

	سانچہ<typename T>
	خالی eat(ثابت T& t) {
		*this = hash_ops<T>::hash_into(t, *this);
	}

	[[deprecated]]
	خالی commutative_eat(hash_t t) {
		state ^= t;
	}

	خالی force(hash_t new_state) {
		state = new_state;
	}
};

using Hasher = HasherDJB32;

// Boilerplate compressor for trivially implementing
// top-level hash method with hash_into
#تعریف_کرو HASH_چوٹی_LOOP_FST [[nodiscard]] ساکن اندرونی Hasher hash
#تعریف_کرو HASH_چوٹی_LOOP_SND { \
	Hasher h; \
	h = hash_into(a, h); \
	واپس h; \
}

سانچہ<typename T>
ساخت hash_ops {
	ساکن اندرونی bool cmp(ثابت T &a, ثابت T &b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت T &a, Hasher h) {
		اگر constexpr (std::is_integral_v<T>) {
			static_assert(حجم(T) <= حجم(uint64_t));
			اگر (حجم(T) == حجم(uint64_t))
				h.hash64(a);
			ورنہ
				h.hash32(a);
			واپس h;
		} ورنہ اگر constexpr (std::is_enum_v<T>) {
			using u_type = std::underlying_type_t<T>;
			واپس hash_ops<u_type>::hash_into((u_type) a, h);
		} ورنہ اگر constexpr (std::is_اشارہ_گر_v<T>) {
			واپس hash_ops<uintptr_t>::hash_into((uintptr_t) a, h);
		} ورنہ اگر constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
			صحیح سائز = a.سائز();
			صحیح i = 0;
			جب (i + 8 < سائز) {
				uint64_t v;
				حافظہ_نقل(&v, a.ڈیٹا() + i, 8);
				h.hash64(v);
				i += 8;
			}
			uint64_t v = 0;
			حافظہ_نقل(&v, a.ڈیٹا() + i, سائز - i);
			h.hash64(v);
			واپس h;
		} ورنہ {
			واپس a.hash_into(h);
		}
	}
	HASH_چوٹی_LOOP_FST (ثابت T &a) HASH_چوٹی_LOOP_SND
};

سانچہ<typename P, typename Q> ساخت hash_ops<std::pair<P, Q>> {
	ساکن اندرونی bool cmp(ثابت std::pair<P, Q> &a, ثابت std::pair<P, Q> &b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت std::pair<P, Q> &a, Hasher h) {
		h = hash_ops<P>::hash_into(a.پہلا, h);
		h = hash_ops<Q>::hash_into(a.دوسرا, h);
		واپس h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::pair<P, Q> &a) HASH_چوٹی_LOOP_SND
	[[nodiscard]] ساکن اندرونی Hasher hash(ثابت P &p, ثابت Q &q) {
		Hasher h;
		h = hash_ops<P>::hash_into(p, h);
		h = hash_ops<Q>::hash_into(q, h);
		واپس h;
	}
};

سانچہ<typename... T> ساخت hash_ops<std::tuple<T...>> {
	ساکن اندرونی bool cmp(ثابت std::tuple<T...> &a, ثابت std::tuple<T...> &b) {
		واپس a == b;
	}
	سانچہ<سائز_t I = 0>
	ساکن اندرونی typename std::enable_if<I == حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &, Hasher h) {
		واپس h;
	}
	سانچہ<سائز_t I = 0>
	ساکن اندرونی typename std::enable_if<I != حجم...(T), Hasher>::type hash_into(ثابت std::tuple<T...> &a, Hasher h) {
		قسمتعریف hash_ops<typename std::tuple_element<I, std::tuple<T...>>::type> element_ops_t;
		h = hash_into<I+1>(a, h);
		h = element_ops_t::hash_into(std::get<I>(a), h);
		واپس h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::tuple<T...> &a) HASH_چوٹی_LOOP_SND
};

سانچہ<typename T> ساخت hash_ops<std::سمتیہ<T>> {
	ساکن اندرونی bool cmp(ثابت std::سمتیہ<T> &a, ثابت std::سمتیہ<T> &b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت std::سمتیہ<T> &a, Hasher h) {
		h.eat((uint32_t)a.سائز());
		برائے (خودکار k : a)
			h.eat(k);
		واپس h;
	}
	HASH_چوٹی_LOOP_FST (ثابت std::سمتیہ<T> &a) HASH_چوٹی_LOOP_SND
};

سانچہ<typename T, سائز_t N> ساخت hash_ops<std::array<T, N>> {
    ساکن اندرونی bool cmp(ثابت std::array<T, N> &a, ثابت std::array<T, N> &b) {
        واپس a == b;
    }
    [[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت std::array<T, N> &a, Hasher h) {
        برائے (ثابت خودکار& k : a)
            h = hash_ops<T>::hash_into(k, h);
        واپس h;
    }
	HASH_چوٹی_LOOP_FST (ثابت std::array<T, N> &a) HASH_چوٹی_LOOP_SND
};

ساخت hash_cstr_ops {
	ساکن اندرونی bool cmp(ثابت حرف *a, ثابت حرف *b) {
		واپس strcmp(a, b) == 0;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت حرف *a, Hasher h) {
		جب (*a)
			h.hash32(*(a++));
		واپس h;
	}
	HASH_چوٹی_LOOP_FST (ثابت حرف *a) HASH_چوٹی_LOOP_SND
};

سانچہ <> ساخت hash_ops<حرف*> : hash_cstr_ops {};

ساخت hash_ptr_ops {
	ساکن اندرونی bool cmp(ثابت خالی *a, ثابت خالی *b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت خالی *a, Hasher h) {
		واپس hash_ops<uintptr_t>::hash_into((uintptr_t)a, h);
	}
	HASH_چوٹی_LOOP_FST (ثابت خالی *a) HASH_چوٹی_LOOP_SND
};

ساخت hash_obj_ops {
	ساکن اندرونی bool cmp(ثابت خالی *a, ثابت خالی *b) {
		واپس a == b;
	}
	سانچہ<typename T>
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت T *a, Hasher h) {
		اگر (a)
			h = a->hash_into(h);
		ورنہ
			h.eat(0);
		واپس h;
	}
	سانچہ<typename T>
	HASH_چوٹی_LOOP_FST (ثابت T *a) HASH_چوٹی_LOOP_SND
};
/**
 * If you find yourself using this function, think hard
 * about if it's the right thing to do. Mixing finalized
 * hashes together with XORs or worse can destroy
 * desirable qualities of the hash function
 */
سانچہ<typename T>
[[nodiscard]]
Hasher::hash_t تسلسل_hash(ثابت T& obj) {
	واپس hash_ops<T>::hash(obj).yield();
}

/** Refer to docs/source/yosys_internals/hashing.rst */
سانچہ<typename T>
[[nodiscard]]
[[deprecated]]
اندرونی غیرعلامتی صحیح mkhash(ثابت T &v) {
	واپس (غیرعلامتی صحیح) تسلسل_hash<T>(v);
}

سانچہ<> ساخت hash_ops<std::monostate> {
	ساکن اندرونی bool cmp(std::monostate a, std::monostate b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(std::monostate, Hasher h) {
		واپس h;
	}
};

سانچہ<typename... T> ساخت hash_ops<std::variant<T...>> {
	ساکن اندرونی bool cmp(ثابت std::variant<T...> &a, ثابت std::variant<T...> &b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت std::variant<T...> &a, Hasher h) {
		std::دورہ([& h](ثابت خودکار &v) { h.eat(v); }, a);
		h.eat(a.اشاریہ());
		واپس h;
	}
};

سانچہ<typename T> ساخت hash_ops<std::optional<T>> {
	ساکن اندرونی bool cmp(ثابت std::optional<T> &a, ثابت std::optional<T> &b) {
		واپس a == b;
	}
	[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت std::optional<T> &a, Hasher h) {
		اگر(a.has_قدر())
			h.eat(*a);
		ورنہ
			h.eat(0);
		واپس h;
	}
};

اندرونی غیرعلامتی صحیح hashtable_سائز(غیرعلامتی صحیح min_سائز)
{
	// Primes as generated by https://oeis.org/A175953
	ساکن std::سمتیہ<غیرعلامتی صحیح> zero_and_some_مفرد_اعداد = {
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

	برائے (خودکار p : zero_and_some_مفرد_اعداد)
		اگر (p >= min_سائز) واپس p;

	اگر (حجم(غیرعلامتی صحیح) == 4)
		throw std::لمبائی_error("hash table exceeded maximum size.\nDesign is likely too large for yosys to handle, if possible try not to flatten the design.");

	برائے (خودکار p : zero_and_some_مفرد_اعداد)
		اگر (100129 * p > min_سائز) واپس 100129 * p;

	throw std::لمبائی_error("hash table exceeded maximum size.");
}

سانچہ<typename K, typename T, typename OPS = hash_ops<K>> class dict;
سانچہ<typename K, صحیح انحراف = 0, typename OPS = hash_ops<K>> class idict;
سانچہ<typename K, typename OPS = hash_ops<K>> class pool;
سانچہ<typename K, typename OPS = hash_ops<K>> class mfp;

// Computes the hash value of an unordered set of elements.
// See https://www.preprints.org/manuscript/201710.0192/v1/download.
// This is the Sum(4) algorithm from that paper, which has good collision resistance,
// much better than Sum(1) or Xor(1) (and somewhat better than Xor(4)).
class commutative_hash {
عوامی:
	commutative_hash() {
		buckets.fill(0);
	}
	سانچہ <typename T>
	خالی eat(ثابت T &obj) {
		eat(hash_ops<T>::hash(obj));
	}
	خالی eat(ثابت Hasher &h) {
		Hasher::hash_t v = h.yield();
		سائز_t اشاریہ = v & (buckets.سائز() - 1);
		buckets[اشاریہ] += v;
	}
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		برائے (خودکار b : buckets)
			h.eat(b);
		واپس h;
	}
نجی:
	std::array<Hasher::hash_t, 4> buckets;
};

سانچہ<typename K, typename T, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل dict {
	ساخت اندراج_t
	{
		std::pair<K, T> udata;
		صحیح اگلا;

		اندراج_t() { }
		اندراج_t(ثابت std::pair<K, T> &udata, صحیح اگلا) : udata(udata), اگلا(اگلا) { }
		اندراج_t(std::pair<K, T> &&udata, صحیح اگلا) : udata(std::move(udata)), اگلا(اگلا) { }
		bool عامل<(ثابت اندراج_t &other) ثابت { واپس udata.پہلا < other.udata.پہلا; }
	};

	std::سمتیہ<صحیح> hashtable;
	std::سمتیہ<اندراج_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#اگر_تعریف_شدہ NDEBUG
	ساکن اندرونی خالی do_assert(bool) { }
#ورنہ
	ساکن اندرونی خالی do_assert(bool cond) {
		اگر (!cond) throw std::runtime_error("dict<> assert failed.");
	}
#اگر_ختم

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		اگر (!hashtable.خالی_خانے())
			hash = ops.hash(کلید).yield() % (غیرعلامتی صحیح)(hashtable.سائز());
		واپس hash;
	}

	خالی do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_سائز(entries.استعداد() * hashtable_سائز_ضربی_عدد), -1);

		برائے (صحیح i = 0; i < صحیح(entries.سائز()); i++) {
			do_assert(-1 <= entries[i].اگلا && entries[i].اگلا < صحیح(entries.سائز()));
			Hasher::hash_t hash = do_hash(entries[i].udata.پہلا);
			entries[i].اگلا = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحیح do_erase(صحیح اشاریہ, Hasher::hash_t hash)
	{
		do_assert(اشاریہ < صحیح(entries.سائز()));
		اگر (hashtable.خالی_خانے() || اشاریہ < 0)
			واپس 0;

		صحیح k = hashtable[hash];
		do_assert(0 <= k && k < صحیح(entries.سائز()));

		اگر (k == اشاریہ) {
			hashtable[hash] = entries[اشاریہ].اگلا;
		} ورنہ {
			جب (entries[k].اگلا != اشاریہ) {
				k = entries[k].اگلا;
				do_assert(0 <= k && k < صحیح(entries.سائز()));
			}
			entries[k].اگلا = entries[اشاریہ].اگلا;
		}

		صحیح عقب_idx = entries.سائز()-1;

		اگر (اشاریہ != عقب_idx)
		{
			Hasher::hash_t عقب_hash = do_hash(entries[عقب_idx].udata.پہلا);

			k = hashtable[عقب_hash];
			do_assert(0 <= k && k < صحیح(entries.سائز()));

			اگر (k == عقب_idx) {
				hashtable[عقب_hash] = اشاریہ;
			} ورنہ {
				جب (entries[k].اگلا != عقب_idx) {
					k = entries[k].اگلا;
					do_assert(0 <= k && k < صحیح(entries.سائز()));
				}
				entries[k].اگلا = اشاریہ;
			}

			entries[اشاریہ] = std::move(entries[عقب_idx]);
		}

		entries.نکالو_عقب();

		اگر (entries.خالی_خانے())
			hashtable.clear();

		واپس 1;
	}

	صحیح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے())
			واپس -1;

		اگر (entries.سائز() * hashtable_سائز_trigger > hashtable.سائز()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		واپس do_lookup_اندرونی(کلید, hash);
	}

	صحیح do_lookup_اندرونی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحیح اشاریہ = hashtable[hash];

		جب (اشاریہ >= 0 && !ops.cmp(entries[اشاریہ].udata.پہلا, کلید)) {
			اشاریہ = entries[اشاریہ].اگلا;
			do_assert(-1 <= اشاریہ && اشاریہ < صحیح(entries.سائز()));
		}

		واپس اشاریہ;
	}

	صحیح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		اگر (hashtable.خالی_خانے())
			واپس -1;

		واپس do_lookup_اندرونی(کلید, hash);
	}

	صحیح do_داخل_کرو(ثابت K &کلید, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے()) {
			entries.emplace_عقب(std::pair<K, T>(کلید, T()), -1);
			do_rehash();
		} ورنہ {
			entries.emplace_عقب(std::pair<K, T>(کلید, T()), hashtable[hash]);
			hashtable[hash] = entries.سائز() - 1;
		}
		واپس entries.سائز() - 1;
	}

	صحیح do_داخل_کرو(ثابت std::pair<K, T> &قدر, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے()) {
			entries.emplace_عقب(قدر, -1);
			do_rehash();
		} ورنہ {
			entries.emplace_عقب(قدر, hashtable[hash]);
			hashtable[hash] = entries.سائز() - 1;
		}
		واپس entries.سائز() - 1;
	}

	صحیح do_داخل_کرو(std::pair<K, T> &&rvalue, ثابت Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے()) {
			entries.emplace_عقب(std::forward<std::pair<K, T>>(rvalue), -1);
			do_rehash();
		} ورنہ {
			entries.emplace_عقب(std::forward<std::pair<K, T>>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.سائز() - 1;
		}
		واپس entries.سائز() - 1;
	}

عوامی:
	class const_iterator
	{
		دوست class dict;
	محفوظ:
		ثابت dict *ptr;
		صحیح اشاریہ;
		const_iterator(ثابت dict *ptr, صحیح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عوامی:
		قسمتعریف std::bidirectional_iterator_لیبل iterator_category;
		قسمتعریف std::pair<K, T> قدر_type;
		قسمتعریف ptrdiff_t difference_type;
		قسمتعریف ثابت std::pair<K, T>* اشارہ_گر;
		قسمتعریف ثابت std::pair<K, T>& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ--; واپس *this; }
		const_iterator عامل++(صحیح) { const_iterator tmp = *this; اشاریہ--; واپس tmp; }
		const_iterator عامل--() { اشاریہ++; واپس *this; }
		const_iterator عامل--(صحیح) { const_iterator tmp = *this; اشاریہ++; واپس tmp; }
		const_iterator عامل+=(صحیح amt) { اشاریہ -= amt; واپس *this; }
		bool عامل<(ثابت const_iterator &other) ثابت { واپس اشاریہ > other.اشاریہ; }
		bool عامل==(ثابت const_iterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
		ثابت std::pair<K, T> &عامل*() ثابت { واپس ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { واپس &ptr->entries[اشاریہ].udata; }
	};

	class iterator
	{
		دوست class dict;
	محفوظ:
		dict *ptr;
		صحیح اشاریہ;
		iterator(dict *ptr, صحیح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عوامی:
		قسمتعریف std::forward_iterator_لیبل iterator_category;
		قسمتعریف std::pair<K, T> قدر_type;
		قسمتعریف ptrdiff_t difference_type;
		قسمتعریف std::pair<K, T>* اشارہ_گر;
		قسمتعریف std::pair<K, T>& reference;
		iterator() { }
		iterator عامل++() { اشاریہ--; واپس *this; }
		iterator عامل+=(صحیح amt) { اشاریہ -= amt; واپس *this; }
		bool عامل<(ثابت iterator &other) ثابت { واپس اشاریہ > other.اشاریہ; }
		bool عامل==(ثابت iterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت iterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
		std::pair<K, T> &عامل*() { واپس ptr->entries[اشاریہ].udata; }
		std::pair<K, T> *عامل->() { واپس &ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> &عامل*() ثابت { واپس ptr->entries[اشاریہ].udata; }
		ثابت std::pair<K, T> *عامل->() ثابت { واپس &ptr->entries[اشاریہ].udata; }
		عامل const_iterator() ثابت { واپس const_iterator(ptr, اشاریہ); }
	};
	using الٹ_iterator = std::الٹ_iterator<const_iterator>;
	الٹ_iterator rbegin() ثابت {
		واپس std::make_الٹ_iterator(انتہا());
	}
	الٹ_iterator rend() ثابت {
		واپس std::make_الٹ_iterator(begin());
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
		تبادلہ(other);
	}

	dict &عامل=(ثابت dict &other) {
		entries = other.entries;
		do_rehash();
		واپس *this;
	}

	dict &عامل=(dict &&other) noexcept {
		clear();
		تبادلہ(other);
		واپس *this;
	}

	dict(ثابت std::initializer_list<std::pair<K, T>> &list)
	{
		برائے (خودکار &it : list)
			داخل_کرو(it);
	}

	سانچہ<class داخلہ_Iterator>
	dict(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		داخل_کرو(پہلا, آخری);
	}

	سانچہ<class داخلہ_Iterator>
	خالی داخل_کرو(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		برائے (; پہلا != آخری; ++پہلا)
			داخل_کرو(*پہلا);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(کلید, hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت std::pair<K, T> &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر.پہلا);
		صحیح i = do_lookup(قدر.پہلا, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(قدر, hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(std::pair<K, T> &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue.پہلا);
		صحیح i = do_lookup(rvalue.پہلا, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::forward<std::pair<K, T>>(rvalue), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(کلید, قدر), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K ثابت &کلید, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(کلید, std::forward<T>(rvalue)), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T ثابت &قدر)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحیح i = do_lookup(rkey, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(std::forward<K>(rkey), قدر), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> emplace(K &&rkey, T &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rkey);
		صحیح i = do_lookup(rkey, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::make_pair(std::forward<K>(rkey), std::forward<T>(rvalue)), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	صحیح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح اشاریہ = do_lookup(کلید, hash);
		واپس do_erase(اشاریہ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(it->پہلا);
		do_erase(it.اشاریہ, hash);
		واپس ++it;
	}

	صحیح گنتی(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		واپس i < 0 ? 0 : 1;
	}

	صحیح گنتی(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		واپس i < 0 || i > it.اشاریہ ? 0 : 1;
	}

	iterator ڈھونڈو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			واپس انتہا();
		واپس iterator(this, i);
	}

	const_iterator ڈھونڈو(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			واپس انتہا();
		واپس const_iterator(this, i);
	}

	T& at(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			throw std::برآمد_کا_range("dict::at()");
		واپس entries[i].udata.دوسرا;
	}

	ثابت T& at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			throw std::برآمد_کا_range("dict::at()");
		واپس entries[i].udata.دوسرا;
	}

	ثابت T& at(ثابت K &کلید, ثابت T &defval) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			واپس defval;
		واپس entries[i].udata.دوسرا;
	}

	T& عامل[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			i = do_داخل_کرو(std::pair<K, T>(کلید, T()), hash);
		واپس entries[i].udata.دوسرا;
	}

	سانچہ<typename Compare = std::less<K>>
	خالی ترتیب_دو(Compare comp = Compare())
	{
		std::ترتیب_دو(entries.begin(), entries.انتہا(), [comp](ثابت اندراج_t &a, ثابت اندراج_t &b){ واپس comp(b.udata.پہلا, a.udata.پہلا); });
		do_rehash();
	}

	خالی تبادلہ(dict &other)
	{
		hashtable.تبادلہ(other.hashtable);
		entries.تبادلہ(other.entries);
	}

	bool عامل==(ثابت dict &other) ثابت {
		اگر (سائز() != other.سائز())
			واپس false;
		برائے (خودکار &it : entries) {
			خودکار oit = other.ڈھونڈو(it.udata.پہلا);
			اگر (oit == other.انتہا() || !(oit->دوسرا == it.udata.دوسرا))
				واپس false;
		}
		واپس true;
	}

	bool عامل!=(ثابت dict &other) ثابت {
		واپس !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		برائے (خودکار &it : entries) {
			Hasher اندراج_hash;
			اندراج_hash.eat(it.udata.پہلا);
			اندراج_hash.eat(it.udata.دوسرا);
			comm.eat(اندراج_hash);
		}
		واپس comm.hash_into(h);
	}

	خالی reserve(سائز_t n) { entries.reserve(n); }
	سائز_t سائز() ثابت { واپس entries.سائز(); }
	bool خالی_خانے() ثابت { واپس entries.خالی_خانے(); }
	خالی clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { واپس iterator(this, صحیح(entries.سائز())-1); }
	iterator element(صحیح n) { واپس iterator(this, صحیح(entries.سائز())-1-n); }
	iterator انتہا() { واپس iterator(nullptr, -1); }

	const_iterator begin() ثابت { واپس const_iterator(this, صحیح(entries.سائز())-1); }
	const_iterator element(صحیح n) ثابت { واپس const_iterator(this, صحیح(entries.سائز())-1-n); }
	const_iterator انتہا() ثابت { واپس const_iterator(this, -1); }
};

سانچہ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل pool
{
	سانچہ<typename, صحیح, typename> دوست class idict;

محفوظ:
	ساخت اندراج_t
	{
		K udata;
		صحیح اگلا;

		اندراج_t() { }
		اندراج_t(ثابت K &udata, صحیح اگلا) : udata(udata), اگلا(اگلا) { }
		اندراج_t(K &&udata, صحیح اگلا) : udata(std::move(udata)), اگلا(اگلا) { }
	};

	std::سمتیہ<صحیح> hashtable;
	std::سمتیہ<اندراج_t> entries;
	OPS ops;
	static_assert(std::is_nothrow_default_constructible_v<OPS>, "move ops are noexcept and default-construct OPS");

#اگر_تعریف_شدہ NDEBUG
	ساکن اندرونی خالی do_assert(bool) { }
#ورنہ
	ساکن اندرونی خالی do_assert(bool cond) {
		اگر (!cond) throw std::runtime_error("pool<> assert failed.");
	}
#اگر_ختم

	Hasher::hash_t do_hash(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = 0;
		اگر (!hashtable.خالی_خانے())
			hash = ops.hash(کلید).yield() % (غیرعلامتی صحیح)(hashtable.سائز());
		واپس hash;
	}

	خالی do_rehash()
	{
		hashtable.clear();
		hashtable.resize(hashtable_سائز(entries.استعداد() * hashtable_سائز_ضربی_عدد), -1);

		برائے (صحیح i = 0; i < صحیح(entries.سائز()); i++) {
			do_assert(-1 <= entries[i].اگلا && entries[i].اگلا < صحیح(entries.سائز()));
			Hasher::hash_t hash = do_hash(entries[i].udata);
			entries[i].اگلا = hashtable[hash];
			hashtable[hash] = i;
		}
	}

	صحیح do_erase(صحیح اشاریہ, Hasher::hash_t hash)
	{
		do_assert(اشاریہ < صحیح(entries.سائز()));
		اگر (hashtable.خالی_خانے() || اشاریہ < 0)
			واپس 0;

		صحیح k = hashtable[hash];
		اگر (k == اشاریہ) {
			hashtable[hash] = entries[اشاریہ].اگلا;
		} ورنہ {
			جب (entries[k].اگلا != اشاریہ) {
				k = entries[k].اگلا;
				do_assert(0 <= k && k < صحیح(entries.سائز()));
			}
			entries[k].اگلا = entries[اشاریہ].اگلا;
		}

		صحیح عقب_idx = entries.سائز()-1;

		اگر (اشاریہ != عقب_idx)
		{
			Hasher::hash_t عقب_hash = do_hash(entries[عقب_idx].udata);

			k = hashtable[عقب_hash];
			اگر (k == عقب_idx) {
				hashtable[عقب_hash] = اشاریہ;
			} ورنہ {
				جب (entries[k].اگلا != عقب_idx) {
					k = entries[k].اگلا;
					do_assert(0 <= k && k < صحیح(entries.سائز()));
				}
				entries[k].اگلا = اشاریہ;
			}

			entries[اشاریہ] = std::move(entries[عقب_idx]);
		}

		entries.نکالو_عقب();

		اگر (entries.خالی_خانے())
			hashtable.clear();

		واپس 1;
	}

	صحیح do_lookup(ثابت K &کلید, Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے())
			واپس -1;

		اگر (entries.سائز() * hashtable_سائز_trigger > hashtable.سائز()) {
			do_rehash();
			hash = do_hash(کلید);
		}

		واپس do_lookup_اندرونی(کلید, hash);
	}

	صحیح do_lookup_اندرونی(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		صحیح اشاریہ = hashtable[hash];

		جب (اشاریہ >= 0 && !ops.cmp(entries[اشاریہ].udata, کلید)) {
			اشاریہ = entries[اشاریہ].اگلا;
			do_assert(-1 <= اشاریہ && اشاریہ < صحیح(entries.سائز()));
		}

		واپس اشاریہ;
	}

	صحیح do_lookup_no_rehash(ثابت K &کلید, Hasher::hash_t hash) ثابت
	{
		اگر (hashtable.خالی_خانے())
			واپس -1;

		واپس do_lookup_اندرونی(کلید, hash);
	}

	صحیح do_داخل_کرو(ثابت K &قدر, Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے()) {
			entries.emplace_عقب(قدر, -1);
			do_rehash();
			hash = do_hash(قدر);
		} ورنہ {
			entries.emplace_عقب(قدر, hashtable[hash]);
			hashtable[hash] = entries.سائز() - 1;
		}
		واپس entries.سائز() - 1;
	}

	صحیح do_داخل_کرو(K &&rvalue, Hasher::hash_t &hash)
	{
		اگر (hashtable.خالی_خانے()) {
			entries.emplace_عقب(std::forward<K>(rvalue), -1);
			do_rehash();
			hash = do_hash(entries.عقب().udata);
		} ورنہ {
			entries.emplace_عقب(std::forward<K>(rvalue), hashtable[hash]);
			hashtable[hash] = entries.سائز() - 1;
		}
		واپس entries.سائز() - 1;
	}

عوامی:
	class const_iterator
	{
		دوست class pool;
	محفوظ:
		ثابت pool *ptr;
		صحیح اشاریہ;
		const_iterator(ثابت pool *ptr, صحیح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عوامی:
		قسمتعریف std::forward_iterator_لیبل iterator_category;
		قسمتعریف K قدر_type;
		قسمتعریف ptrdiff_t difference_type;
		قسمتعریف K* اشارہ_گر;
		قسمتعریف K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ--; واپس *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
		ثابت K &عامل*() ثابت { واپس ptr->entries[اشاریہ].udata; }
		ثابت K *عامل->() ثابت { واپس &ptr->entries[اشاریہ].udata; }
	};

	class iterator
	{
		دوست class pool;
	محفوظ:
		pool *ptr;
		صحیح اشاریہ;
		iterator(pool *ptr, صحیح اشاریہ) : ptr(ptr), اشاریہ(اشاریہ) { }
	عوامی:
		قسمتعریف std::forward_iterator_لیبل iterator_category;
		قسمتعریف K قدر_type;
		قسمتعریف ptrdiff_t difference_type;
		قسمتعریف K* اشارہ_گر;
		قسمتعریف K& reference;
		iterator() { }
		iterator عامل++() { اشاریہ--; واپس *this; }
		bool عامل==(ثابت iterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت iterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
		K &عامل*() { واپس ptr->entries[اشاریہ].udata; }
		K *عامل->() { واپس &ptr->entries[اشاریہ].udata; }
		ثابت K &عامل*() ثابت { واپس ptr->entries[اشاریہ].udata; }
		ثابت K *عامل->() ثابت { واپس &ptr->entries[اشاریہ].udata; }
		عامل const_iterator() ثابت { واپس const_iterator(ptr, اشاریہ); }
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
		تبادلہ(other);
	}

	pool &عامل=(ثابت pool &other) {
		entries = other.entries;
		do_rehash();
		واپس *this;
	}

	pool &عامل=(pool &&other) noexcept {
		clear();
		تبادلہ(other);
		واپس *this;
	}

	pool(ثابت std::initializer_list<K> &list)
	{
		برائے (خودکار &it : list)
			داخل_کرو(it);
	}

	سانچہ<class داخلہ_Iterator>
	pool(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		داخل_کرو(پہلا, آخری);
	}

	سانچہ<class داخلہ_Iterator>
	خالی داخل_کرو(داخلہ_Iterator پہلا, داخلہ_Iterator آخری)
	{
		برائے (; پہلا != آخری; ++پہلا)
			داخل_کرو(*پہلا);
	}

	std::pair<iterator, bool> داخل_کرو(ثابت K &قدر)
	{
		Hasher::hash_t hash = do_hash(قدر);
		صحیح i = do_lookup(قدر, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(قدر, hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	std::pair<iterator, bool> داخل_کرو(K &&rvalue)
	{
		Hasher::hash_t hash = do_hash(rvalue);
		صحیح i = do_lookup(rvalue, hash);
		اگر (i >= 0)
			واپس std::pair<iterator, bool>(iterator(this, i), false);
		i = do_داخل_کرو(std::forward<K>(rvalue), hash);
		واپس std::pair<iterator, bool>(iterator(this, i), true);
	}

	سانچہ<typename... دلائل_3>
	std::pair<iterator, bool> emplace(دلائل_3&&... دلائل_2)
	{
		واپس داخل_کرو(K(std::forward<دلائل_3>(دلائل_2)...));
	}

	صحیح erase(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح اشاریہ = do_lookup(کلید, hash);
		واپس do_erase(اشاریہ, hash);
	}

	iterator erase(iterator it)
	{
		Hasher::hash_t hash = do_hash(*it);
		do_erase(it.اشاریہ, hash);
		واپس ++it;
	}

	صحیح گنتی(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		واپس i < 0 ? 0 : 1;
	}

	صحیح گنتی(ثابت K &کلید, const_iterator it) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		واپس i < 0 || i > it.اشاریہ ? 0 : 1;
	}

	iterator ڈھونڈو(ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		اگر (i < 0)
			واپس انتہا();
		واپس iterator(this, i);
	}

	const_iterator ڈھونڈو(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			واپس انتہا();
		واپس const_iterator(this, i);
	}

	bool عامل[](ثابت K &کلید)
	{
		Hasher::hash_t hash = do_hash(کلید);
		صحیح i = do_lookup(کلید, hash);
		واپس i >= 0;
	}

	سانچہ<typename Compare = std::less<K>>
	خالی ترتیب_دو(Compare comp = Compare())
	{
		std::ترتیب_دو(entries.begin(), entries.انتہا(), [comp](ثابت اندراج_t &a, ثابت اندراج_t &b){ واپس comp(b.udata, a.udata); });
		do_rehash();
	}

	K نکالو()
	{
		iterator it = begin();
		K ret = *it;
		erase(it);
		واپس ret;
	}

	خالی تبادلہ(pool &other)
	{
		hashtable.تبادلہ(other.hashtable);
		entries.تبادلہ(other.entries);
	}

	bool عامل==(ثابت pool &other) ثابت {
		اگر (سائز() != other.سائز())
			واپس false;
		برائے (خودکار &it : entries)
			اگر (!other.گنتی(it.udata))
				واپس false;
		واپس true;
	}

	bool عامل!=(ثابت pool &other) ثابت {
		واپس !عامل==(other);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		commutative_hash comm;
		برائے (خودکار &it : entries) {
			comm.eat(ops.hash(it.udata));
		}
		واپس comm.hash_into(h);
	}

	خالی reserve(سائز_t n) { entries.reserve(n); }
	سائز_t سائز() ثابت { واپس entries.سائز(); }
	bool خالی_خانے() ثابت { واپس entries.خالی_خانے(); }
	خالی clear() { hashtable.clear(); entries.clear(); }

	iterator begin() { واپس iterator(this, صحیح(entries.سائز())-1); }
	iterator element(صحیح n) { واپس iterator(this, صحیح(entries.سائز())-1-n); }
	iterator انتہا() { واپس iterator(nullptr, -1); }

	const_iterator begin() ثابت { واپس const_iterator(this, صحیح(entries.سائز())-1); }
	const_iterator element(صحیح n) ثابت { واپس const_iterator(this, صحیح(entries.سائز())-1-n); }
	const_iterator انتہا() ثابت { واپس const_iterator(nullptr, -1); }
};

سانچہ<typename K, صحیح انحراف, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل idict
{
	pool<K, OPS> database;

عوامی:
	class const_iterator
	{
		دوست class idict;
	محفوظ:
		ثابت idict &container;
		صحیح اشاریہ;
		const_iterator(ثابت idict &container, صحیح اشاریہ) : container(container), اشاریہ(اشاریہ) { }
	عوامی:
		قسمتعریف std::forward_iterator_لیبل iterator_category;
		قسمتعریف K قدر_type;
		قسمتعریف ptrdiff_t difference_type;
		قسمتعریف K* اشارہ_گر;
		قسمتعریف K& reference;
		const_iterator() { }
		const_iterator عامل++() { اشاریہ++; واپس *this; }
		bool عامل==(ثابت const_iterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
		bool عامل!=(ثابت const_iterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
		ثابت K &عامل*() ثابت { واپس container[اشاریہ]; }
		ثابت K *عامل->() ثابت { واپس &container[اشاریہ]; }
	};

	constexpr idict()
	{
	}

	صحیح عامل()(ثابت K &کلید)
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup(کلید, hash);
		اگر (i < 0)
			i = database.do_داخل_کرو(کلید, hash);
		واپس i + انحراف;
	}

	صحیح at(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			throw std::برآمد_کا_range("idict::at()");
		واپس i + انحراف;
	}

	صحیح at(ثابت K &کلید, صحیح defval) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		اگر (i < 0)
			واپس defval;
		واپس i + انحراف;
	}

	صحیح گنتی(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		صحیح i = database.do_lookup_no_rehash(کلید, hash);
		واپس i < 0 ? 0 : 1;
	}

	صحیح lookup(ثابت K &کلید) ثابت
	{
		Hasher::hash_t hash = database.do_hash(کلید);
		واپس database.do_lookup_no_rehash(کلید, hash);
	}

	خالی expect(ثابت K &کلید, صحیح i)
	{
		صحیح j = (*this)(کلید);
		اگر (i != j)
			throw std::برآمد_کا_range("idict::expect()");
	}

	ثابت K &عامل[](صحیح اشاریہ) ثابت
	{
		واپس database.entries.at(اشاریہ - انحراف).udata;
	}

	خالی تبادلہ(idict &other)
	{
		database.تبادلہ(other.database);
	}

	خالی reserve(سائز_t n) { database.reserve(n); }
	سائز_t سائز() ثابت { واپس database.سائز(); }
	bool خالی_خانے() ثابت { واپس database.خالی_خانے(); }
	خالی clear() { database.clear(); }

	const_iterator begin() ثابت { واپس const_iterator(*this, انحراف); }
	const_iterator element(صحیح n) ثابت { واپس const_iterator(*this, n); }
	const_iterator انتہا() ثابت { واپس const_iterator(*this, انحراف + سائز()); }
};

/**
 * Union-find data structure with a promotion method
 * mfp stands for "merge, find, promote"
 * i-prefixed methods operate on indices in parents
*/
سانچہ<typename K, typename OPS>
class HASHLIB_ATTRIBUTE_WARN_غیر_مستعمل mfp
{
	idict<K, 0, OPS> database;
	class Atomic_والد {
	عوامی:
		explicit Atomic_والد(صحیح p) : والد(p) {}
		Atomic_والد(ثابت Atomic_والد &other) : والد(other.get()) {}
		Atomic_والد &عامل=(ثابت Atomic_والد &other) { سیٹ(other.get()); واپس *this; }
		صحیح get() ثابت { واپس والد.بھرو(std::حافظہ_ترتیب_relaxed); }
		خالی سیٹ(صحیح p) { والد.store(p, std::حافظہ_ترتیب_relaxed); }
	نجی:
		std::atomic<صحیح> والد;
	};
	std::سمتیہ<Atomic_والد> parents;

عوامی:
	قسمتعریف typename idict<K, 0>::const_iterator const_iterator;

	constexpr mfp()
	{
	}

	// Finds a given element's index. If it isn't in the data structure,
	// it is added as its own set
	صحیح عامل()(ثابت K &کلید)
	{
		صحیح i = database(کلید);
		// If the lookup caused the database to grow,
		// also add a corresponding entry in parents initialized to -1 (no parent)
		اگر (parents.سائز() < database.سائز()) {
			parents.emplace_عقب(-1);
		}
		واپس i;
	}

	// Finds an element at given index
	ثابت K &عامل[](صحیح اشاریہ) ثابت
	{
		واپس database[اشاریہ];
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
	صحیح ifind(صحیح i) ثابت
	{
		صحیح p = i, k = i;

		جب (true) {
			صحیح pp = parents[p].get();
			اگر (pp < 0)
				توڑو;
			p = pp;
		}
		// p is now the representative of i
		// Now we traverse from i up to the representative again
		// and make p the parent of all the nodes along the way.
		// This is a side effect and doesn't affect the return value.
		// It speeds up future find operations
		جب (k != p) {
			صحیح اگلا_k = parents[k].get();
			const_cast<Atomic_والد*>(&parents[k])->سیٹ(p);
			k = اگلا_k;
		}

		واپس p;
	}

	// Merge sets if the given indices belong to different sets.
	// Makes ifind(j) the root of the merged set.
	خالی imerge(صحیح i, صحیح j)
	{
		i = ifind(i);
		j = ifind(j);

		اگر (i != j)
			parents[i].سیٹ(j);
	}

	خالی ipromote(صحیح i)
	{
		صحیح k = i;

		جب (k != -1) {
			صحیح اگلا_k = parents[k].get();
			parents[k].سیٹ(i);
			k = اگلا_k;
		}

		parents[i].سیٹ(-1);
	}

	صحیح lookup(ثابت K &a)
	{
		واپس ifind((*this)(a));
	}

	ثابت K &ڈھونڈو(ثابت K &a) ثابت
	{
		صحیح i = database.at(a, -1);
		اگر (i < 0)
			واپس a;
		واپس (*this)[ifind(i)];
	}

	خالی ملاؤ(ثابت K &a, ثابت K &b)
	{
		imerge((*this)(a), (*this)(b));
	}

	خالی promote(ثابت K &a)
	{
		صحیح i = database.at(a, -1);
		اگر (i >= 0)
			ipromote(i);
	}

	خالی تبادلہ(mfp &other)
	{
		database.تبادلہ(other.database);
		parents.تبادلہ(other.parents);
	}

	خالی reserve(سائز_t n) { database.reserve(n); }
	سائز_t سائز() ثابت { واپس database.سائز(); }
	bool خالی_خانے() ثابت { واپس database.خالی_خانے(); }
	خالی clear() { database.clear(); parents.clear(); }

	const_iterator begin() ثابت { واپس database.begin(); }
	const_iterator element(صحیح n) ثابت { واپس database.element(n); }
	const_iterator انتہا() ثابت { واپس database.انتہا(); }
};

} /* namespace hashlib */

#اگر_ختم
