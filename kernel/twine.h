#اگر_تعریف_نشده YOSYS_TWINE_H
#تعریف_کن YOSYS_TWINE_H

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
	constexpr عملگر IdString() ثابت;
	constexpr bool عملگر==(IdString ref) ثابت;
};

بنية IdString {
خاص:
#اگر_تعریف_شده YOSYS_ENABLE_TWINE_PROVENANCE
	using بیتها_t = uint64_t;
#وإلا
	using بیتها_t = uint32_t;
#پایان_اگر
	بیتها_t مقدار;

	ساكن constexpr صحيح TWINE_عرض = std::numeric_limits<بیتها_t>::digits;
	ساكن constexpr صحيح TWINE_عمومی_لغزش = TWINE_عرض - 1;
	ساكن constexpr بیتها_t TWINE_عمومی_بیت = بیتها_t{1} << TWINE_عمومی_لغزش;
	ساكن constexpr بیتها_t TWINE_NULL_VAL = ~بیتها_t{0};
	ساكن constexpr صحيح TWINE_SERIAL_لغزش = TWINE_عرض == 64 ? 32 : TWINE_عمومی_لغزش;
	ساكن constexpr بیتها_t TWINE_شاخص_ماسک = (بیتها_t{1} << TWINE_SERIAL_لغزش) - 1;
	ساكن constexpr بیتها_t TWINE_SERIAL_ماسک = بیتها_t(~(TWINE_شاخص_ماسک | TWINE_عمومی_بیت));

عام:
	ساكن constexpr NullIdString Null{};

	ساكن constexpr اندازه_t MAX_شاخص = TWINE_شاخص_ماسک;
	ساكن constexpr اندازه_t MAX_SERIAL = TWINE_SERIAL_ماسک >> TWINE_SERIAL_لغزش;

	constexpr اندازه_t raw() ثابت {
		أرجع مقدار == TWINE_NULL_VAL ? مقدار : (مقدار & ~TWINE_SERIAL_ماسک);
	}

	// raw() with the pool serial kept
	constexpr اندازه_t بیتها() ثابت { أرجع مقدار; }

	constexpr اندازه_t ترتیب_کلید() ثابت { أرجع std::rotl((بیتها_t) eq_کلید(), 1); }

	constexpr اندازه_t serial() ثابت {
		أرجع مقدار == TWINE_NULL_VAL ? 0 : ((مقدار & TWINE_SERIAL_ماسک) >> TWINE_SERIAL_لغزش);
	}

	constexpr IdString stamped(اندازه_t pool_serial) ثابت {
		أرجع مقدار == TWINE_NULL_VAL ? *this
			: IdString((مقدار & ~TWINE_SERIAL_ماسک)
					| (بیتها_t(pool_serial << TWINE_SERIAL_لغزش) & TWINE_SERIAL_ماسک));
	}

	constexpr IdString() : مقدار(TWINE_NULL_VAL) {}
	explicit constexpr IdString(اندازه_t val) : مقدار((بیتها_t) val) {}

	constexpr اندازه_t eq_کلید() ثابت { أرجع مقدار & ~TWINE_SERIAL_ماسک; }

	constexpr bool عملگر==(ثابت IdString &rhs) ثابت { أرجع eq_کلید() == rhs.eq_کلید(); }
	constexpr bool عملگر!=(ثابت IdString &rhs) ثابت { أرجع eq_کلید() != rhs.eq_کلید(); }
	constexpr std::strong_ordering عملگر<=>(ثابت IdString &rhs) ثابت {
		أرجع ترتیب_کلید() <=> rhs.ترتیب_کلید();
	}

	قالب <typename... آرگومانها_3>
	constexpr bool درون(ثابت آرگومانها_3&... آرگومانها_2) ثابت {
		أرجع (درون_one(آرگومانها_2) || ...);
	}
	bool درون(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	قالب<typename T> بنية compare_ptr_by_name {
		bool عملگر()(ثابت T *a, ثابت T *b) ثابت {
			أرجع (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool خالی() ثابت { أرجع مقدار == TWINE_NULL_VAL; }
	constexpr فراغ clear() { مقدار = TWINE_NULL_VAL; }
	constexpr bool is_عمومی() ثابت { أرجع مقدار != TWINE_NULL_VAL && (مقدار & TWINE_عمومی_بیت); }

	constexpr IdString untag() ثابت {
		أرجع مقدار == TWINE_NULL_VAL ? *this : IdString(مقدار & ~TWINE_عمومی_بیت);
	}
	constexpr IdString برچسب(bool pub) ثابت {
		أرجع مقدار == TWINE_NULL_VAL ? *this : IdString(pub ? (مقدار | TWINE_عمومی_بیت) : (مقدار & ~TWINE_عمومی_بیت));
	}

	std::string handle_token() ثابت;
	ساكن اندازه_t handle_token_پیشوند(std::string_view token, bool &is_عمومی_2);

خاص:
	constexpr bool درون_one(IdString rhs) ثابت { أرجع *this == rhs; }
	bool درون_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::عملگر IdString() ثابت { أرجع IdString(); }
constexpr bool NullIdString::عملگر==(IdString ref) ثابت { أرجع ref.خالی(); }

namespace hashlib {
	قالب<>
	بنية hash_ops<IdString> {
		ساكن مضمن bool cmp(IdString a, IdString b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_کلید());
			أرجع h;
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_کلید());
			أرجع h;
		}
	};
}

مضمن bool IdString::درون_one(ثابت pool<IdString> &rhs) ثابت { أرجع rhs.شمار(*this) != 0; }
مضمن bool IdString::درون(ثابت pool<IdString> &rhs) ثابت { أرجع درون_one(rhs); }


تعداد : کوتاه {
#تعریف_کن X(N) IDX_##N,
#ضمن "kernel/constids.inc"
#لغو_تعریف X
	STATIC_TWINE_انتها
};

بنية ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#تعریف_کن X(N) ساكن constexpr IdString N = IdString(IDX_##N).برچسب((#N)[0] != '$');
#ضمن "kernel/constids.inc"
#لغو_تعریف X

	ساكن constexpr ثابت حرف* static_نامها[] = {
#تعریف_کن X(N) #N,
#ضمن "kernel/constids.inc"
#لغو_تعریف X
	};

	ساكن constexpr IdString lookup(std::string_view name)
	{
		صحيح کم = 0, بالا = STATIC_TWINE_انتها;
		طالما (بالا - کم >= 2) {
			صحيح mid = (کم + بالا) / 2;
			إذا (name < static_نامها[mid])
				بالا = mid;
			وإلا
				کم = mid;
		}

		إذا (name != static_نامها[کم])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		أرجع IdString(کم).برچسب(name[0] != '$');
	}

	ساكن constexpr bool is_static(IdString ref) {
		أرجع ref.untag().raw() < STATIC_TWINE_انتها;
	}

	ساكن constexpr IdString static_ref(اندازه_t raw) {
		إذا (!is_static(IdString(raw)))
			throw "not a static twine id";
		أرجع IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ساكن std::string str(IdString ref);
	ساكن std::string unescaped_str(IdString ref);
};

قالب<اندازه_t Raw> مضمن constexpr IdString constid = ID::static_ref(Raw);

#تعریف_کن ID(id) (YOSYS_NAMESPACE_پیشوند constid<YOSYS_NAMESPACE_پیشوند ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
بنية TwineSpec {
	// deduplicates shared prefixes
	بنية Suffix {
		IdString پیشوند_2;
		std::string دم;
		خودکار عملگر<=>(ثابت Suffix&) ثابت = افتراضي;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	بنية AutoSuffix {
		ثابت std::string *پیشوند_2;
		std::string دم;
		خودکار عملگر<=>(ثابت AutoSuffix&) ثابت = افتراضي;
	};

	// "leaf", regular deduplicated string
	بنية برگ {
		std::string s;
		خودکار عملگر<=>(ثابت برگ&) ثابت = افتراضي;
	};

	std::variant<برگ, Suffix, AutoSuffix> داده;

	TwineSpec(برگ v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

بنية SmallString {
	ساكن constexpr uint32_t INLINE_CAP = 8;
	ساكن constexpr اندازه_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = افتراضي;
	explicit SmallString(std::string_view content);
	SmallString(ثابت SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &عملگر=(ثابت SmallString &other);
	SmallString &عملگر=(SmallString &&other) noexcept;
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
بنية Twine_گره {
	ساكن constexpr uint32_t NO_پیشوند = ~uint32_t{0};
	ساكن constexpr uint32_t DEAD = NO_پیشوند - 1;

	بنية کلید_2 {
		uint32_t پیشوند_2;
		std::string_view متن;
	};

	ساكن constexpr uint32_t extend_hash(uint32_t hash, std::string_view دم) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		لكل (حرف c : دم)
			hash = ((hash << 5) + hash) + static_cast<بیعلامت حرف>(c);
		أرجع hash;
	}

	Twine_گره() = افتراضي;
	explicit Twine_گره(std::string_view content)
		: متن_2(content), پیشوند_3(NO_پیشوند), hash_(extend_hash(0, content)) {}
	Twine_گره(uint32_t پیشوند_2, std::string_view دم, uint32_t hash)
		: متن_2(دم), پیشوند_3(پیشوند_2), hash_(hash) {}
	Twine_گره(ثابت Twine_گره &other) = افتراضي;
	Twine_گره(Twine_گره &&other) noexcept;
	Twine_گره &عملگر=(ثابت Twine_گره &other) = افتراضي;
	Twine_گره &عملگر=(Twine_گره &&other) noexcept;
	~Twine_گره() = افتراضي;

	تعداد class نوع_2 { Dead, برگ, Suffix };

	constexpr bool is_dead() ثابت { أرجع پیشوند_3 == DEAD; }
	constexpr bool is_برگ() ثابت { أرجع پیشوند_3 == NO_پیشوند; }
	constexpr bool is_suffix() ثابت { أرجع پیشوند_3 < DEAD; }

	constexpr نوع_2 نوع() ثابت {
		أرجع پیشوند_3 == DEAD ? نوع_2::Dead
			: پیشوند_3 == NO_پیشوند ? نوع_2::برگ
			: نوع_2::Suffix;
	}

	std::string_view متن() ثابت { أرجع متن_2.view(); }
	IdString پیشوند_2() ثابت { أرجع IdString(پیشوند_3); }
	uint32_t hash() ثابت { أرجع hash_; }
	کلید_2 کلید() ثابت { أرجع {پیشوند_3, متن()}; }

	bool عملگر==(ثابت Twine_گره &other) ثابت
		{ أرجع پیشوند_3 == other.پیشوند_3 && متن() == other.متن(); }
	bool عملگر==(ثابت کلید_2 &other) ثابت
		{ أرجع پیشوند_3 == other.پیشوند_2 && متن() == other.متن; }

خاص:
	SmallString متن_2;
	uint32_t پیشوند_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_گره) == (حجم(فراغ *) == 8 ? 24 : 20));

بنية StaticTwines {
	ساكن فراغ init();
	ساكن ثابت Twine_گره &گره(اندازه_t idx);
	ساكن bool ready();

خاص:
	ساكن std::وکتور<Twine_گره> nodes_;
};

خارجي int64_t twine_gc_ns;
خارجي صحيح twine_gc_شمار;

بنية TwinePool : HashConsPool<TwinePool, Twine_گره, IdString> {
	ساكن constexpr اندازه_t STATIC_شمار = STATIC_TWINE_انتها;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& عملگر=(ثابت TwinePool& other);
	TwinePool& عملگر=(TwinePool&& other);

	ثابت Twine_گره& عملگر[](IdString ref) ثابت;

	ساكن ثابت Twine_گره& static_گره(اندازه_t idx);

	ساكن فراغ check_ready();
	ساكن uint32_t hash_گره(ثابت Twine_گره& t) { أرجع t.hash(); }

	قالب<typename F>
	ساكن فراغ for_each_فرزند(ثابت Twine_گره& t, F&& f) {
		إذا (t.is_suffix())
			f(t.پیشوند_2());
	}

	اندازه_t gc(pool<IdString>& roots);

	فراغ dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	اندازه_t str_اندازه(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view پیشوند_2) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحيح compare_by_name(IdString a, IdString b) ثابت;
	IdString پیشوند_از(IdString ref) ثابت;

	IdString پیدا_کن(std::string_view name) ثابت;
	IdString پیدا_کن(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString جمع_کن(TwineSpec t);
	IdString جمع_کن(IdString پیشوند_2, std::string_view دم);
	// Infers publicity from first character
	IdString جمع_کن(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString پیدا_کن_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	فراغ check_owned(IdString ref) ثابت;

خاص:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	فراغ append_str(IdString ref, std::string& بیرون) ثابت;
	IdString auto_پیشوند(ثابت std::string *پیشوند_2);
	IdString جمع_کن_inner(Twine_گره t);
	IdString intern(uint32_t پیشوند_2, std::string_view متن);
	uint32_t content_hash(uint32_t پیشوند_2, std::string_view متن) ثابت;
	IdString پیدا_کن_content(uint32_t پیشوند_2, std::string_view متن, uint32_t hash) ثابت;
	IdString پیدا_کن_content(uint32_t پیشوند_2, std::string_view متن) ثابت;
	ساكن اندازه_t بعدی_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	اندازه_t serial_;
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
