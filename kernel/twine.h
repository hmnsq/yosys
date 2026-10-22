#اگر_غیر_تعریف_شدہ YOSYS_TWINE_H
#تعریف_کرو YOSYS_TWINE_H

#شامل "kernel/hashcons.h"
#شامل "kernel/yosys_common.h"

#شامل <algorithm>
#شامل <bit>
#شامل <cstdint>
#شامل <cstring>
#شامل <deque>
#شامل <limits>
#شامل <span>
#شامل <string>
#شامل <string_view>
#شامل <unordered_set>
#شامل <variant>
#شامل <vector>

YOSYS_NAMESPACE_BEGIN

ساخت TwineSpec;
ساخت TwinePool;
ساخت IdString;

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

ساخت NullIdString {
	constexpr عامل IdString() ثابت;
	constexpr bool عامل==(IdString ref) ثابت;
};

ساخت IdString {
نجی:
#اگر_تعریف_شدہ YOSYS_ENABLE_TWINE_PROVENANCE
	using بٹ_t = uint64_t;
#ورنہ
	using بٹ_t = uint32_t;
#اگر_ختم
	بٹ_t قدر;

	ساکن constexpr صحیح TWINE_چوڑائی = std::numeric_limits<بٹ_t>::digits;
	ساکن constexpr صحیح TWINE_عوامی_سرکاؤ = TWINE_چوڑائی - 1;
	ساکن constexpr بٹ_t TWINE_عوامی_ثنائی_ہندسہ = بٹ_t{1} << TWINE_عوامی_سرکاؤ;
	ساکن constexpr بٹ_t TWINE_NULL_VAL = ~بٹ_t{0};
	ساکن constexpr صحیح TWINE_SERIAL_سرکاؤ = TWINE_چوڑائی == 64 ? 32 : TWINE_عوامی_سرکاؤ;
	ساکن constexpr بٹ_t TWINE_اشاریہ_نقاب = (بٹ_t{1} << TWINE_SERIAL_سرکاؤ) - 1;
	ساکن constexpr بٹ_t TWINE_SERIAL_نقاب = بٹ_t(~(TWINE_اشاریہ_نقاب | TWINE_عوامی_ثنائی_ہندسہ));

عوامی:
	ساکن constexpr NullIdString Null{};

	ساکن constexpr سائز_t MAX_اشاریہ = TWINE_اشاریہ_نقاب;
	ساکن constexpr سائز_t MAX_SERIAL = TWINE_SERIAL_نقاب >> TWINE_SERIAL_سرکاؤ;

	constexpr سائز_t raw() ثابت {
		واپس قدر == TWINE_NULL_VAL ? قدر : (قدر & ~TWINE_SERIAL_نقاب);
	}

	// raw() with the pool serial kept
	constexpr سائز_t بٹ() ثابت { واپس قدر; }

	constexpr سائز_t ترتیب_کلید() ثابت { واپس std::rotl((بٹ_t) eq_کلید(), 1); }

	constexpr سائز_t serial() ثابت {
		واپس قدر == TWINE_NULL_VAL ? 0 : ((قدر & TWINE_SERIAL_نقاب) >> TWINE_SERIAL_سرکاؤ);
	}

	constexpr IdString stamped(سائز_t pool_serial) ثابت {
		واپس قدر == TWINE_NULL_VAL ? *this
			: IdString((قدر & ~TWINE_SERIAL_نقاب)
					| (بٹ_t(pool_serial << TWINE_SERIAL_سرکاؤ) & TWINE_SERIAL_نقاب));
	}

	constexpr IdString() : قدر(TWINE_NULL_VAL) {}
	explicit constexpr IdString(سائز_t val) : قدر((بٹ_t) val) {}

	constexpr سائز_t eq_کلید() ثابت { واپس قدر & ~TWINE_SERIAL_نقاب; }

	constexpr bool عامل==(ثابت IdString &rhs) ثابت { واپس eq_کلید() == rhs.eq_کلید(); }
	constexpr bool عامل!=(ثابت IdString &rhs) ثابت { واپس eq_کلید() != rhs.eq_کلید(); }
	constexpr std::strong_ordering عامل<=>(ثابت IdString &rhs) ثابت {
		واپس ترتیب_کلید() <=> rhs.ترتیب_کلید();
	}

	سانچہ <typename... دلائل_3>
	constexpr bool اندر(ثابت دلائل_3&... دلائل_2) ثابت {
		واپس (اندر_one(دلائل_2) || ...);
	}
	bool اندر(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	سانچہ<typename T> ساخت compare_ptr_by_name {
		bool عامل()(ثابت T *a, ثابت T *b) ثابت {
			واپس (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool خالی_خانے() ثابت { واپس قدر == TWINE_NULL_VAL; }
	constexpr خالی clear() { قدر = TWINE_NULL_VAL; }
	constexpr bool is_عوامی() ثابت { واپس قدر != TWINE_NULL_VAL && (قدر & TWINE_عوامی_ثنائی_ہندسہ); }

	constexpr IdString untag() ثابت {
		واپس قدر == TWINE_NULL_VAL ? *this : IdString(قدر & ~TWINE_عوامی_ثنائی_ہندسہ);
	}
	constexpr IdString لیبل(bool pub) ثابت {
		واپس قدر == TWINE_NULL_VAL ? *this : IdString(pub ? (قدر | TWINE_عوامی_ثنائی_ہندسہ) : (قدر & ~TWINE_عوامی_ثنائی_ہندسہ));
	}

	std::string handle_token() ثابت;
	ساکن سائز_t handle_token_سابقہ(std::string_view token, bool &is_عوامی_2);

نجی:
	constexpr bool اندر_one(IdString rhs) ثابت { واپس *this == rhs; }
	bool اندر_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::عامل IdString() ثابت { واپس IdString(); }
constexpr bool NullIdString::عامل==(IdString ref) ثابت { واپس ref.خالی_خانے(); }

namespace hashlib {
	سانچہ<>
	ساخت hash_ops<IdString> {
		ساکن اندرونی bool cmp(IdString a, IdString b) { واپس a == b; }
		[[nodiscard]] ساکن اندرونی Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_کلید());
			واپس h;
		}
		[[nodiscard]] ساکن اندرونی Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_کلید());
			واپس h;
		}
	};
}

اندرونی bool IdString::اندر_one(ثابت pool<IdString> &rhs) ثابت { واپس rhs.گنتی(*this) != 0; }
اندرونی bool IdString::اندر(ثابت pool<IdString> &rhs) ثابت { واپس اندر_one(rhs); }


شمار : مختصر {
#تعریف_کرو X(N) IDX_##N,
#شامل "kernel/constids.inc"
#تعریف_منسوخ X
	STATIC_TWINE_انتہا
};

ساخت ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#تعریف_کرو X(N) ساکن constexpr IdString N = IdString(IDX_##N).لیبل((#N)[0] != '$');
#شامل "kernel/constids.inc"
#تعریف_منسوخ X

	ساکن constexpr ثابت حرف* static_نام[] = {
#تعریف_کرو X(N) #N,
#شامل "kernel/constids.inc"
#تعریف_منسوخ X
	};

	ساکن constexpr IdString lookup(std::string_view name)
	{
		صحیح کم = 0, بلند = STATIC_TWINE_انتہا;
		جب (بلند - کم >= 2) {
			صحیح mid = (کم + بلند) / 2;
			اگر (name < static_نام[mid])
				بلند = mid;
			ورنہ
				کم = mid;
		}

		اگر (name != static_نام[کم])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		واپس IdString(کم).لیبل(name[0] != '$');
	}

	ساکن constexpr bool is_static(IdString ref) {
		واپس ref.untag().raw() < STATIC_TWINE_انتہا;
	}

	ساکن constexpr IdString static_ref(سائز_t raw) {
		اگر (!is_static(IdString(raw)))
			throw "not a static twine id";
		واپس IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ساکن std::string str(IdString ref);
	ساکن std::string unescaped_str(IdString ref);
};

سانچہ<سائز_t Raw> اندرونی constexpr IdString constid = ID::static_ref(Raw);

#تعریف_کرو ID(id) (YOSYS_NAMESPACE_سابقہ constid<YOSYS_NAMESPACE_سابقہ ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ساخت TwineSpec {
	// deduplicates shared prefixes
	ساخت Suffix {
		IdString سابقہ_2;
		std::string دم;
		خودکار عامل<=>(ثابت Suffix&) ثابت = طےشدہ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ساخت AutoSuffix {
		ثابت std::string *سابقہ_2;
		std::string دم;
		خودکار عامل<=>(ثابت AutoSuffix&) ثابت = طےشدہ;
	};

	// "leaf", regular deduplicated string
	ساخت برگ {
		std::string s;
		خودکار عامل<=>(ثابت برگ&) ثابت = طےشدہ;
	};

	std::variant<برگ, Suffix, AutoSuffix> ڈیٹا;

	TwineSpec(برگ v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

ساخت SmallString {
	ساکن constexpr uint32_t INLINE_CAP = 8;
	ساکن constexpr سائز_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = طےشدہ;
	explicit SmallString(std::string_view content);
	SmallString(ثابت SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &عامل=(ثابت SmallString &other);
	SmallString &عامل=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ثابت { واپس {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

نجی:
	خالی store(std::string_view content);
	خالی release();

	اتحاد {
		حرف inl_[INLINE_CAP];
		حرف *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ساخت Twine_گرہ {
	ساکن constexpr uint32_t NO_سابقہ = ~uint32_t{0};
	ساکن constexpr uint32_t DEAD = NO_سابقہ - 1;

	ساخت کلید_2 {
		uint32_t سابقہ_2;
		std::string_view متن;
	};

	ساکن constexpr uint32_t extend_hash(uint32_t hash, std::string_view دم) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		برائے (حرف c : دم)
			hash = ((hash << 5) + hash) + static_cast<غیرعلامتی حرف>(c);
		واپس hash;
	}

	Twine_گرہ() = طےشدہ;
	explicit Twine_گرہ(std::string_view content)
		: متن_2(content), سابقہ_3(NO_سابقہ), hash_(extend_hash(0, content)) {}
	Twine_گرہ(uint32_t سابقہ_2, std::string_view دم, uint32_t hash)
		: متن_2(دم), سابقہ_3(سابقہ_2), hash_(hash) {}
	Twine_گرہ(ثابت Twine_گرہ &other) = طےشدہ;
	Twine_گرہ(Twine_گرہ &&other) noexcept;
	Twine_گرہ &عامل=(ثابت Twine_گرہ &other) = طےشدہ;
	Twine_گرہ &عامل=(Twine_گرہ &&other) noexcept;
	~Twine_گرہ() = طےشدہ;

	شمار class قسم_2 { Dead, برگ, Suffix };

	constexpr bool is_dead() ثابت { واپس سابقہ_3 == DEAD; }
	constexpr bool is_برگ() ثابت { واپس سابقہ_3 == NO_سابقہ; }
	constexpr bool is_suffix() ثابت { واپس سابقہ_3 < DEAD; }

	constexpr قسم_2 قسم() ثابت {
		واپس سابقہ_3 == DEAD ? قسم_2::Dead
			: سابقہ_3 == NO_سابقہ ? قسم_2::برگ
			: قسم_2::Suffix;
	}

	std::string_view متن() ثابت { واپس متن_2.view(); }
	IdString سابقہ_2() ثابت { واپس IdString(سابقہ_3); }
	uint32_t hash() ثابت { واپس hash_; }
	کلید_2 کلید() ثابت { واپس {سابقہ_3, متن()}; }

	bool عامل==(ثابت Twine_گرہ &other) ثابت
		{ واپس سابقہ_3 == other.سابقہ_3 && متن() == other.متن(); }
	bool عامل==(ثابت کلید_2 &other) ثابت
		{ واپس سابقہ_3 == other.سابقہ_2 && متن() == other.متن; }

نجی:
	SmallString متن_2;
	uint32_t سابقہ_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_گرہ) == (حجم(خالی *) == 8 ? 24 : 20));

ساخت StaticTwines {
	ساکن خالی init();
	ساکن ثابت Twine_گرہ &گرہ(سائز_t idx);
	ساکن bool ready();

نجی:
	ساکن std::سمتیہ<Twine_گرہ> nodes_;
};

خارجی int64_t twine_gc_ns;
خارجی صحیح twine_gc_گنتی;

ساخت TwinePool : HashConsPool<TwinePool, Twine_گرہ, IdString> {
	ساکن constexpr سائز_t STATIC_گنتی = STATIC_TWINE_انتہا;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& عامل=(ثابت TwinePool& other);
	TwinePool& عامل=(TwinePool&& other);

	ثابت Twine_گرہ& عامل[](IdString ref) ثابت;

	ساکن ثابت Twine_گرہ& static_گرہ(سائز_t idx);

	ساکن خالی check_ready();
	ساکن uint32_t hash_گرہ(ثابت Twine_گرہ& t) { واپس t.hash(); }

	سانچہ<typename F>
	ساکن خالی for_each_بچہ(ثابت Twine_گرہ& t, F&& f) {
		اگر (t.is_suffix())
			f(t.سابقہ_2());
	}

	سائز_t gc(pool<IdString>& roots);

	خالی dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	سائز_t str_سائز(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view سابقہ_2) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحیح compare_by_name(IdString a, IdString b) ثابت;
	IdString سابقہ_کا(IdString ref) ثابت;

	IdString ڈھونڈو(std::string_view name) ثابت;
	IdString ڈھونڈو(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString جوڑو(TwineSpec t);
	IdString جوڑو(IdString سابقہ_2, std::string_view دم);
	// Infers publicity from first character
	IdString جوڑو(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString ڈھونڈو_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	خالی check_owned(IdString ref) ثابت;

نجی:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	خالی append_str(IdString ref, std::string& برآمد) ثابت;
	IdString auto_سابقہ(ثابت std::string *سابقہ_2);
	IdString جوڑو_inner(Twine_گرہ t);
	IdString intern(uint32_t سابقہ_2, std::string_view متن);
	uint32_t content_hash(uint32_t سابقہ_2, std::string_view متن) ثابت;
	IdString ڈھونڈو_content(uint32_t سابقہ_2, std::string_view متن, uint32_t hash) ثابت;
	IdString ڈھونڈو_content(uint32_t سابقہ_2, std::string_view متن) ثابت;
	ساکن سائز_t اگلا_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	سائز_t serial_;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
