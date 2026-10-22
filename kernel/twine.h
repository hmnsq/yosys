#جيڪڏهن_وصف_نه_ٿيل YOSYS_TWINE_H
#وصف_ڪريو YOSYS_TWINE_H

#ضمن "kernel/hashcons.h"
#ضمن "kernel/yosys_common.h"

#ضمن <algorithm>
#ضمن <bit>
#ضمن <cstdint>
#ضمن <cstring>
#ضمن <deque>
#ضمن <limits>
#ضمن <span>
#ضمن <string>
#ضمن <string_view>
#ضمن <unordered_set>
#ضمن <variant>
#ضمن <vector>

YOSYS_NAMESPACE_BEGIN

بنية TwineSpec;
بنية TwinePool;
بنية IdString;

/**
 * A twine is a data structure designed to deduplicate prefixes.
 * The key idea is that a twine is a suffix node or a leaf node.
 * A leaf node holds a string,
 * while a suffix node appends a string to a twine.
 *
 * Twines are used here to implement IdString, the Yosys interned
 * string type. They're extended to the needs of Yosys, specifically
 * with efficient static NEW_ID prefixes,
 * and IdString holding a special bit marking the publicity of a name.
 * When public, a backslash is prepended when printing an IdString.
 *
 * An IdString references a TwineNode in a per-Design TwinePool
 * by indexing into it. It is constructed by interning a TwineSpec
 * into a TwinePool.
 *
 * A general IdString can't be constructed, converted to a string, or printed
 * without a pointer to the TwinePool.
 * Compile-time allocated indices defined in kernel/constids.inc
 * live in a global read-only pool, so they're an exception to that.
 */

بنية NullIdString {
	constexpr عامل IdString() ثابت;
	constexpr bool عامل==(IdString ref) ثابت;
};

بنية IdString {
خاص:
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TWINE_PROVENANCE
	using بٽ_t = uint64_t;
#وإلا
	using بٽ_t = uint32_t;
#جيڪڏهن_پورو
	بٽ_t قدر;

	ساكن constexpr صحيح TWINE_ويڪر = std::numeric_limits<بٽ_t>::digits;
	ساكن constexpr صحيح TWINE_عوامي_سرڪايو = TWINE_ويڪر - 1;
	ساكن constexpr بٽ_t TWINE_عوامي_ٻٽو_انگ = بٽ_t{1} << TWINE_عوامي_سرڪايو;
	ساكن constexpr بٽ_t TWINE_NULL_VAL = ~بٽ_t{0};
	ساكن constexpr صحيح TWINE_SERIAL_سرڪايو = TWINE_ويڪر == 64 ? 32 : TWINE_عوامي_سرڪايو;
	ساكن constexpr بٽ_t TWINE_اشاريو_نقاب = (بٽ_t{1} << TWINE_SERIAL_سرڪايو) - 1;
	ساكن constexpr بٽ_t TWINE_SERIAL_نقاب = بٽ_t(~(TWINE_اشاريو_نقاب | TWINE_عوامي_ٻٽو_انگ));

عام:
	ساكن constexpr NullIdString Null{};

	ساكن constexpr سائيز_t MAX_اشاريو = TWINE_اشاريو_نقاب;
	ساكن constexpr سائيز_t MAX_SERIAL = TWINE_SERIAL_نقاب >> TWINE_SERIAL_سرڪايو;

	constexpr سائيز_t raw() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? قدر : (قدر & ~TWINE_SERIAL_نقاب);
	}

	// raw() with the pool serial kept
	constexpr سائيز_t بٽ() ثابت { أرجع قدر; }

	constexpr سائيز_t ترتيب_ڪنجي() ثابت { أرجع std::rotl((بٽ_t) eq_ڪنجي(), 1); }

	constexpr سائيز_t serial() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? 0 : ((قدر & TWINE_SERIAL_نقاب) >> TWINE_SERIAL_سرڪايو);
	}

	constexpr IdString stamped(سائيز_t pool_serial) ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this
			: IdString((قدر & ~TWINE_SERIAL_نقاب)
					| (بٽ_t(pool_serial << TWINE_SERIAL_سرڪايو) & TWINE_SERIAL_نقاب));
	}

	constexpr IdString() : قدر(TWINE_NULL_VAL) {}
	explicit constexpr IdString(سائيز_t val) : قدر((بٽ_t) val) {}

	constexpr سائيز_t eq_ڪنجي() ثابت { أرجع قدر & ~TWINE_SERIAL_نقاب; }

	constexpr bool عامل==(ثابت IdString &rhs) ثابت { أرجع eq_ڪنجي() == rhs.eq_ڪنجي(); }
	constexpr bool عامل!=(ثابت IdString &rhs) ثابت { أرجع eq_ڪنجي() != rhs.eq_ڪنجي(); }
	constexpr std::strong_ordering عامل<=>(ثابت IdString &rhs) ثابت {
		أرجع ترتيب_ڪنجي() <=> rhs.ترتيب_ڪنجي();
	}

	سانچو <typename... دليلون_3>
	constexpr bool اندريون(ثابت دليلون_3&... دليلون_2) ثابت {
		أرجع (اندريون_one(دليلون_2) || ...);
	}
	bool اندريون(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	سانچو<typename T> بنية compare_ptr_by_name {
		bool عامل()(ثابت T *a, ثابت T *b) ثابت {
			أرجع (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool خالي_جايون() ثابت { أرجع قدر == TWINE_NULL_VAL; }
	constexpr فراغ clear() { قدر = TWINE_NULL_VAL; }
	constexpr bool is_عوامي() ثابت { أرجع قدر != TWINE_NULL_VAL && (قدر & TWINE_عوامي_ٻٽو_انگ); }

	constexpr IdString untag() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this : IdString(قدر & ~TWINE_عوامي_ٻٽو_انگ);
	}
	constexpr IdString ٽيگ(bool pub) ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this : IdString(pub ? (قدر | TWINE_عوامي_ٻٽو_انگ) : (قدر & ~TWINE_عوامي_ٻٽو_انگ));
	}

	std::string handle_token() ثابت;
	ساكن سائيز_t handle_token_اڳياڙي(std::string_view token, bool &is_عوامي_2);

خاص:
	constexpr bool اندريون_one(IdString rhs) ثابت { أرجع *this == rhs; }
	bool اندريون_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::عامل IdString() ثابت { أرجع IdString(); }
constexpr bool NullIdString::عامل==(IdString ref) ثابت { أرجع ref.خالي_جايون(); }

namespace hashlib {
	سانچو<>
	بنية hash_ops<IdString> {
		ساكن مضمن bool cmp(IdString a, IdString b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ڪنجي());
			أرجع h;
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ڪنجي());
			أرجع h;
		}
	};
}

مضمن bool IdString::اندريون_one(ثابت pool<IdString> &rhs) ثابت { أرجع rhs.ڳڻپ(*this) != 0; }
مضمن bool IdString::اندريون(ثابت pool<IdString> &rhs) ثابت { أرجع اندريون_one(rhs); }


تعداد : ننڍو {
#وصف_ڪريو X(N) IDX_##N,
#ضمن "kernel/constids.inc"
#وصف_رد X
	STATIC_TWINE_پڇاڙي
};

بنية ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#وصف_ڪريو X(N) ساكن constexpr IdString N = IdString(IDX_##N).ٽيگ((#N)[0] != '$');
#ضمن "kernel/constids.inc"
#وصف_رد X

	ساكن constexpr ثابت حرف* static_نالا[] = {
#وصف_ڪريو X(N) #N,
#ضمن "kernel/constids.inc"
#وصف_رد X
	};

	ساكن constexpr IdString lookup(std::string_view name)
	{
		صحيح هيٺيون = 0, مٿاهون = STATIC_TWINE_پڇاڙي;
		طالما (مٿاهون - هيٺيون >= 2) {
			صحيح mid = (هيٺيون + مٿاهون) / 2;
			إذا (name < static_نالا[mid])
				مٿاهون = mid;
			وإلا
				هيٺيون = mid;
		}

		إذا (name != static_نالا[هيٺيون])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		أرجع IdString(هيٺيون).ٽيگ(name[0] != '$');
	}

	ساكن constexpr bool is_static(IdString ref) {
		أرجع ref.untag().raw() < STATIC_TWINE_پڇاڙي;
	}

	ساكن constexpr IdString static_ref(سائيز_t raw) {
		إذا (!is_static(IdString(raw)))
			throw "not a static twine id";
		أرجع IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ساكن std::string str(IdString ref);
	ساكن std::string unescaped_str(IdString ref);
};

سانچو<سائيز_t Raw> مضمن constexpr IdString constid = ID::static_ref(Raw);

#وصف_ڪريو ID(id) (YOSYS_NAMESPACE_اڳياڙي constid<YOSYS_NAMESPACE_اڳياڙي ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
بنية TwineSpec {
	// deduplicates shared prefixes
	بنية Suffix {
		IdString اڳياڙي_2;
		std::string پڇ;
		خودڪار عامل<=>(ثابت Suffix&) ثابت = افتراضي;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	بنية AutoSuffix {
		ثابت std::string *اڳياڙي_2;
		std::string پڇ;
		خودڪار عامل<=>(ثابت AutoSuffix&) ثابت = افتراضي;
	};

	// "leaf", regular deduplicated string
	بنية پن {
		std::string s;
		خودڪار عامل<=>(ثابت پن&) ثابت = افتراضي;
	};

	std::variant<پن, Suffix, AutoSuffix> معلومات;

	TwineSpec(پن v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

بنية SmallString {
	ساكن constexpr uint32_t INLINE_CAP = 8;
	ساكن constexpr سائيز_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = افتراضي;
	explicit SmallString(std::string_view content);
	SmallString(ثابت SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &عامل=(ثابت SmallString &other);
	SmallString &عامل=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ثابت { أرجع {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

خاص:
	فراغ store(std::string_view content);
	فراغ release();

	اتحاد {
		حرف inl_[INLINE_CAP];
		حرف *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
بنية Twine_ڳنڍ {
	ساكن constexpr uint32_t NO_اڳياڙي = ~uint32_t{0};
	ساكن constexpr uint32_t DEAD = NO_اڳياڙي - 1;

	بنية ڪنجي_2 {
		uint32_t اڳياڙي_2;
		std::string_view متن;
	};

	ساكن constexpr uint32_t extend_hash(uint32_t hash, std::string_view پڇ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		لكل (حرف c : پڇ)
			hash = ((hash << 5) + hash) + static_cast<بنانشان حرف>(c);
		أرجع hash;
	}

	Twine_ڳنڍ() = افتراضي;
	explicit Twine_ڳنڍ(std::string_view content)
		: متن_2(content), اڳياڙي_3(NO_اڳياڙي), hash_(extend_hash(0, content)) {}
	Twine_ڳنڍ(uint32_t اڳياڙي_2, std::string_view پڇ, uint32_t hash)
		: متن_2(پڇ), اڳياڙي_3(اڳياڙي_2), hash_(hash) {}
	Twine_ڳنڍ(ثابت Twine_ڳنڍ &other) = افتراضي;
	Twine_ڳنڍ(Twine_ڳنڍ &&other) noexcept;
	Twine_ڳنڍ &عامل=(ثابت Twine_ڳنڍ &other) = افتراضي;
	Twine_ڳنڍ &عامل=(Twine_ڳنڍ &&other) noexcept;
	~Twine_ڳنڍ() = افتراضي;

	تعداد class قسم_2 { Dead, پن, Suffix };

	constexpr bool is_dead() ثابت { أرجع اڳياڙي_3 == DEAD; }
	constexpr bool is_پن() ثابت { أرجع اڳياڙي_3 == NO_اڳياڙي; }
	constexpr bool is_suffix() ثابت { أرجع اڳياڙي_3 < DEAD; }

	constexpr قسم_2 قسم() ثابت {
		أرجع اڳياڙي_3 == DEAD ? قسم_2::Dead
			: اڳياڙي_3 == NO_اڳياڙي ? قسم_2::پن
			: قسم_2::Suffix;
	}

	std::string_view متن() ثابت { أرجع متن_2.view(); }
	IdString اڳياڙي_2() ثابت { أرجع IdString(اڳياڙي_3); }
	uint32_t hash() ثابت { أرجع hash_; }
	ڪنجي_2 ڪنجي() ثابت { أرجع {اڳياڙي_3, متن()}; }

	bool عامل==(ثابت Twine_ڳنڍ &other) ثابت
		{ أرجع اڳياڙي_3 == other.اڳياڙي_3 && متن() == other.متن(); }
	bool عامل==(ثابت ڪنجي_2 &other) ثابت
		{ أرجع اڳياڙي_3 == other.اڳياڙي_2 && متن() == other.متن; }

خاص:
	SmallString متن_2;
	uint32_t اڳياڙي_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_ڳنڍ) == (حجم(فراغ *) == 8 ? 24 : 20));

بنية StaticTwines {
	ساكن فراغ init();
	ساكن ثابت Twine_ڳنڍ &ڳنڍ(سائيز_t idx);
	ساكن bool ready();

خاص:
	ساكن std::ويڪٽر<Twine_ڳنڍ> nodes_;
};

خارجي int64_t twine_gc_ns;
خارجي صحيح twine_gc_ڳڻپ;

بنية TwinePool : HashConsPool<TwinePool, Twine_ڳنڍ, IdString> {
	ساكن constexpr سائيز_t STATIC_ڳڻپ = STATIC_TWINE_پڇاڙي;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& عامل=(ثابت TwinePool& other);
	TwinePool& عامل=(TwinePool&& other);

	ثابت Twine_ڳنڍ& عامل[](IdString ref) ثابت;

	ساكن ثابت Twine_ڳنڍ& static_ڳنڍ(سائيز_t idx);

	ساكن فراغ check_ready();
	ساكن uint32_t hash_ڳنڍ(ثابت Twine_ڳنڍ& t) { أرجع t.hash(); }

	سانچو<typename F>
	ساكن فراغ for_each_ٻار(ثابت Twine_ڳنڍ& t, F&& f) {
		إذا (t.is_suffix())
			f(t.اڳياڙي_2());
	}

	سائيز_t gc(pool<IdString>& roots);

	فراغ dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	سائيز_t str_سائيز(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view اڳياڙي_2) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحيح compare_by_name(IdString a, IdString b) ثابت;
	IdString اڳياڙي_جو(IdString ref) ثابت;

	IdString ڳوليو(std::string_view name) ثابت;
	IdString ڳوليو(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString جوڙيو(TwineSpec t);
	IdString جوڙيو(IdString اڳياڙي_2, std::string_view پڇ);
	// Infers publicity from first character
	IdString جوڙيو(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString ڳوليو_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	فراغ check_owned(IdString ref) ثابت;

خاص:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	فراغ append_str(IdString ref, std::string& ٻاهريون) ثابت;
	IdString auto_اڳياڙي(ثابت std::string *اڳياڙي_2);
	IdString جوڙيو_inner(Twine_ڳنڍ t);
	IdString intern(uint32_t اڳياڙي_2, std::string_view متن);
	uint32_t content_hash(uint32_t اڳياڙي_2, std::string_view متن) ثابت;
	IdString ڳوليو_content(uint32_t اڳياڙي_2, std::string_view متن, uint32_t hash) ثابت;
	IdString ڳوليو_content(uint32_t اڳياڙي_2, std::string_view متن) ثابت;
	ساكن سائيز_t ايندڙ_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	سائيز_t serial_;
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
