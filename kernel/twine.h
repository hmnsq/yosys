#جے_بنا_تعریف YOSYS_TWINE_H
#تعریف_کرو YOSYS_TWINE_H

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
#جے_تعریف_شدہ YOSYS_ENABLE_TWINE_PROVENANCE
	using بٹ_t = uint64_t;
#وإلا
	using بٹ_t = uint32_t;
#جے_انت
	بٹ_t قدر;

	ساكن constexpr صحيح TWINE_چوڑائی = std::numeric_limits<بٹ_t>::digits;
	ساكن constexpr صحيح TWINE_عوامی_کھسکاؤ = TWINE_چوڑائی - 1;
	ساكن constexpr بٹ_t TWINE_عوامی_ثنائی_ہندسہ = بٹ_t{1} << TWINE_عوامی_کھسکاؤ;
	ساكن constexpr بٹ_t TWINE_NULL_VAL = ~بٹ_t{0};
	ساكن constexpr صحيح TWINE_SERIAL_کھسکاؤ = TWINE_چوڑائی == 64 ? 32 : TWINE_عوامی_کھسکاؤ;
	ساكن constexpr بٹ_t TWINE_اشاریہ_نقاب = (بٹ_t{1} << TWINE_SERIAL_کھسکاؤ) - 1;
	ساكن constexpr بٹ_t TWINE_SERIAL_نقاب = بٹ_t(~(TWINE_اشاریہ_نقاب | TWINE_عوامی_ثنائی_ہندسہ));

عام:
	ساكن constexpr NullIdString Null{};

	ساكن constexpr ناپ_t MAX_اشاریہ = TWINE_اشاریہ_نقاب;
	ساكن constexpr ناپ_t MAX_SERIAL = TWINE_SERIAL_نقاب >> TWINE_SERIAL_کھسکاؤ;

	constexpr ناپ_t raw() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? قدر : (قدر & ~TWINE_SERIAL_نقاب);
	}

	// raw() with the pool serial kept
	constexpr ناپ_t بٹ() ثابت { أرجع قدر; }

	constexpr ناپ_t ترتیب_کلید() ثابت { أرجع std::rotl((بٹ_t) eq_کلید(), 1); }

	constexpr ناپ_t serial() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? 0 : ((قدر & TWINE_SERIAL_نقاب) >> TWINE_SERIAL_کھسکاؤ);
	}

	constexpr IdString stamped(ناپ_t pool_serial) ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this
			: IdString((قدر & ~TWINE_SERIAL_نقاب)
					| (بٹ_t(pool_serial << TWINE_SERIAL_کھسکاؤ) & TWINE_SERIAL_نقاب));
	}

	constexpr IdString() : قدر(TWINE_NULL_VAL) {}
	explicit constexpr IdString(ناپ_t val) : قدر((بٹ_t) val) {}

	constexpr ناپ_t eq_کلید() ثابت { أرجع قدر & ~TWINE_SERIAL_نقاب; }

	constexpr bool عامل==(ثابت IdString &rhs) ثابت { أرجع eq_کلید() == rhs.eq_کلید(); }
	constexpr bool عامل!=(ثابت IdString &rhs) ثابت { أرجع eq_کلید() != rhs.eq_کلید(); }
	constexpr std::strong_ordering عامل<=>(ثابت IdString &rhs) ثابت {
		أرجع ترتیب_کلید() <=> rhs.ترتیب_کلید();
	}

	سانچا <typename... دلیلاں_3>
	constexpr bool وچ(ثابت دلیلاں_3&... دلیلاں_2) ثابت {
		أرجع (وچ_one(دلیلاں_2) || ...);
	}
	bool وچ(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	سانچا<typename T> بنية compare_ptr_by_name {
		bool عامل()(ثابت T *a, ثابت T *b) ثابت {
			أرجع (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool خالی_تھاواں() ثابت { أرجع قدر == TWINE_NULL_VAL; }
	constexpr فراغ clear() { قدر = TWINE_NULL_VAL; }
	constexpr bool is_عوامی() ثابت { أرجع قدر != TWINE_NULL_VAL && (قدر & TWINE_عوامی_ثنائی_ہندسہ); }

	constexpr IdString untag() ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this : IdString(قدر & ~TWINE_عوامی_ثنائی_ہندسہ);
	}
	constexpr IdString ٹیگ(bool pub) ثابت {
		أرجع قدر == TWINE_NULL_VAL ? *this : IdString(pub ? (قدر | TWINE_عوامی_ثنائی_ہندسہ) : (قدر & ~TWINE_عوامی_ثنائی_ہندسہ));
	}

	std::string handle_token() ثابت;
	ساكن ناپ_t handle_token_اگیتر(std::string_view token, bool &is_عوامی_2);

خاص:
	constexpr bool وچ_one(IdString rhs) ثابت { أرجع *this == rhs; }
	bool وچ_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::عامل IdString() ثابت { أرجع IdString(); }
constexpr bool NullIdString::عامل==(IdString ref) ثابت { أرجع ref.خالی_تھاواں(); }

namespace hashlib {
	سانچا<>
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

مضمن bool IdString::وچ_one(ثابت pool<IdString> &rhs) ثابت { أرجع rhs.گنتی(*this) != 0; }
مضمن bool IdString::وچ(ثابت pool<IdString> &rhs) ثابت { أرجع وچ_one(rhs); }


تعداد : چھوٹا {
#تعریف_کرو X(N) IDX_##N,
#ضمن "kernel/constids.inc"
#تعریف_رد X
	STATIC_TWINE_سرا
};

بنية ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#تعریف_کرو X(N) ساكن constexpr IdString N = IdString(IDX_##N).ٹیگ((#N)[0] != '$');
#ضمن "kernel/constids.inc"
#تعریف_رد X

	ساكن constexpr ثابت حرف* static_ناں[] = {
#تعریف_کرو X(N) #N,
#ضمن "kernel/constids.inc"
#تعریف_رد X
	};

	ساكن constexpr IdString lookup(std::string_view name)
	{
		صحيح کم = 0, بلند = STATIC_TWINE_سرا;
		طالما (بلند - کم >= 2) {
			صحيح mid = (کم + بلند) / 2;
			إذا (name < static_ناں[mid])
				بلند = mid;
			وإلا
				کم = mid;
		}

		إذا (name != static_ناں[کم])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		أرجع IdString(کم).ٹیگ(name[0] != '$');
	}

	ساكن constexpr bool is_static(IdString ref) {
		أرجع ref.untag().raw() < STATIC_TWINE_سرا;
	}

	ساكن constexpr IdString static_ref(ناپ_t raw) {
		إذا (!is_static(IdString(raw)))
			throw "not a static twine id";
		أرجع IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ساكن std::string str(IdString ref);
	ساكن std::string unescaped_str(IdString ref);
};

سانچا<ناپ_t Raw> مضمن constexpr IdString constid = ID::static_ref(Raw);

#تعریف_کرو ID(id) (YOSYS_NAMESPACE_اگیتر constid<YOSYS_NAMESPACE_اگیتر ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
بنية TwineSpec {
	// deduplicates shared prefixes
	بنية Suffix {
		IdString اگیتر_2;
		std::string پوچھ;
		خودکار عامل<=>(ثابت Suffix&) ثابت = افتراضي;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	بنية AutoSuffix {
		ثابت std::string *اگیتر_2;
		std::string پوچھ;
		خودکار عامل<=>(ثابت AutoSuffix&) ثابت = افتراضي;
	};

	// "leaf", regular deduplicated string
	بنية پتی {
		std::string s;
		خودکار عامل<=>(ثابت پتی&) ثابت = افتراضي;
	};

	std::variant<پتی, Suffix, AutoSuffix> انکڑے;

	TwineSpec(پتی v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

بنية SmallString {
	ساكن constexpr uint32_t INLINE_CAP = 8;
	ساكن constexpr ناپ_t MAX_LEN = std::numeric_limits<uint32_t>::max();

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
بنية Twine_گرہ {
	ساكن constexpr uint32_t NO_اگیتر = ~uint32_t{0};
	ساكن constexpr uint32_t DEAD = NO_اگیتر - 1;

	بنية کلید_2 {
		uint32_t اگیتر_2;
		std::string_view متن;
	};

	ساكن constexpr uint32_t extend_hash(uint32_t hash, std::string_view پوچھ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		لكل (حرف c : پوچھ)
			hash = ((hash << 5) + hash) + static_cast<بنانشانی حرف>(c);
		أرجع hash;
	}

	Twine_گرہ() = افتراضي;
	explicit Twine_گرہ(std::string_view content)
		: متن_2(content), اگیتر_3(NO_اگیتر), hash_(extend_hash(0, content)) {}
	Twine_گرہ(uint32_t اگیتر_2, std::string_view پوچھ, uint32_t hash)
		: متن_2(پوچھ), اگیتر_3(اگیتر_2), hash_(hash) {}
	Twine_گرہ(ثابت Twine_گرہ &other) = افتراضي;
	Twine_گرہ(Twine_گرہ &&other) noexcept;
	Twine_گرہ &عامل=(ثابت Twine_گرہ &other) = افتراضي;
	Twine_گرہ &عامل=(Twine_گرہ &&other) noexcept;
	~Twine_گرہ() = افتراضي;

	تعداد class قسم_2 { Dead, پتی, Suffix };

	constexpr bool is_dead() ثابت { أرجع اگیتر_3 == DEAD; }
	constexpr bool is_پتی() ثابت { أرجع اگیتر_3 == NO_اگیتر; }
	constexpr bool is_suffix() ثابت { أرجع اگیتر_3 < DEAD; }

	constexpr قسم_2 قسم() ثابت {
		أرجع اگیتر_3 == DEAD ? قسم_2::Dead
			: اگیتر_3 == NO_اگیتر ? قسم_2::پتی
			: قسم_2::Suffix;
	}

	std::string_view متن() ثابت { أرجع متن_2.view(); }
	IdString اگیتر_2() ثابت { أرجع IdString(اگیتر_3); }
	uint32_t hash() ثابت { أرجع hash_; }
	کلید_2 کلید() ثابت { أرجع {اگیتر_3, متن()}; }

	bool عامل==(ثابت Twine_گرہ &other) ثابت
		{ أرجع اگیتر_3 == other.اگیتر_3 && متن() == other.متن(); }
	bool عامل==(ثابت کلید_2 &other) ثابت
		{ أرجع اگیتر_3 == other.اگیتر_2 && متن() == other.متن; }

خاص:
	SmallString متن_2;
	uint32_t اگیتر_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_گرہ) == (حجم(فراغ *) == 8 ? 24 : 20));

بنية StaticTwines {
	ساكن فراغ init();
	ساكن ثابت Twine_گرہ &گرہ(ناپ_t idx);
	ساكن bool ready();

خاص:
	ساكن std::سمتیہ<Twine_گرہ> nodes_;
};

خارجي int64_t twine_gc_ns;
خارجي صحيح twine_gc_گنتی;

بنية TwinePool : HashConsPool<TwinePool, Twine_گرہ, IdString> {
	ساكن constexpr ناپ_t STATIC_گنتی = STATIC_TWINE_سرا;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& عامل=(ثابت TwinePool& other);
	TwinePool& عامل=(TwinePool&& other);

	ثابت Twine_گرہ& عامل[](IdString ref) ثابت;

	ساكن ثابت Twine_گرہ& static_گرہ(ناپ_t idx);

	ساكن فراغ check_ready();
	ساكن uint32_t hash_گرہ(ثابت Twine_گرہ& t) { أرجع t.hash(); }

	سانچا<typename F>
	ساكن فراغ for_each_بچہ(ثابت Twine_گرہ& t, F&& f) {
		إذا (t.is_suffix())
			f(t.اگیتر_2());
	}

	ناپ_t gc(pool<IdString>& roots);

	فراغ dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	ناپ_t str_ناپ(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view اگیتر_2) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحيح compare_by_name(IdString a, IdString b) ثابت;
	IdString اگیتر_دا(IdString ref) ثابت;

	IdString لبھو(std::string_view name) ثابت;
	IdString لبھو(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString جوڑو(TwineSpec t);
	IdString جوڑو(IdString اگیتر_2, std::string_view پوچھ);
	// Infers publicity from first character
	IdString جوڑو(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString لبھو_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	فراغ check_owned(IdString ref) ثابت;

خاص:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	فراغ append_str(IdString ref, std::string& باہرلا) ثابت;
	IdString auto_اگیتر(ثابت std::string *اگیتر_2);
	IdString جوڑو_inner(Twine_گرہ t);
	IdString intern(uint32_t اگیتر_2, std::string_view متن);
	uint32_t content_hash(uint32_t اگیتر_2, std::string_view متن) ثابت;
	IdString لبھو_content(uint32_t اگیتر_2, std::string_view متن, uint32_t hash) ثابت;
	IdString لبھو_content(uint32_t اگیتر_2, std::string_view متن) ثابت;
	ساكن ناپ_t اگلا_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	ناپ_t serial_;
};

YOSYS_NAMESPACE_سرا

#جے_انت
