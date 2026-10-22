#که_نه_تعریف_شوی YOSYS_TWINE_H
#تعریف_کړه YOSYS_TWINE_H

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
	constexpr چلوونکی IdString() ثابت;
	constexpr bool چلوونکی==(IdString ref) ثابت;
};

بنية IdString {
خاص:
#که_تعریف_شوی YOSYS_ENABLE_TWINE_PROVENANCE
	using بټونه_t = uint64_t;
#وإلا
	using بټونه_t = uint32_t;
#د_که_پای
	بټونه_t ارزښت;

	ساكن constexpr صحيح TWINE_پلنوالی = std::numeric_limits<بټونه_t>::digits;
	ساكن constexpr صحيح TWINE_عامه_ښويدنه = TWINE_پلنوالی - 1;
	ساكن constexpr بټونه_t TWINE_عامه_بټ = بټونه_t{1} << TWINE_عامه_ښويدنه;
	ساكن constexpr بټونه_t TWINE_NULL_VAL = ~بټونه_t{0};
	ساكن constexpr صحيح TWINE_SERIAL_ښويدنه = TWINE_پلنوالی == 64 ? 32 : TWINE_عامه_ښويدنه;
	ساكن constexpr بټونه_t TWINE_شاخص_ماسک = (بټونه_t{1} << TWINE_SERIAL_ښويدنه) - 1;
	ساكن constexpr بټونه_t TWINE_SERIAL_ماسک = بټونه_t(~(TWINE_شاخص_ماسک | TWINE_عامه_بټ));

عام:
	ساكن constexpr NullIdString Null{};

	ساكن constexpr اندازه_t MAX_شاخص = TWINE_شاخص_ماسک;
	ساكن constexpr اندازه_t MAX_SERIAL = TWINE_SERIAL_ماسک >> TWINE_SERIAL_ښويدنه;

	constexpr اندازه_t raw() ثابت {
		أرجع ارزښت == TWINE_NULL_VAL ? ارزښت : (ارزښت & ~TWINE_SERIAL_ماسک);
	}

	// raw() with the pool serial kept
	constexpr اندازه_t بټونه() ثابت { أرجع ارزښت; }

	constexpr اندازه_t ترتیب_کلی() ثابت { أرجع std::rotl((بټونه_t) eq_کلی(), 1); }

	constexpr اندازه_t serial() ثابت {
		أرجع ارزښت == TWINE_NULL_VAL ? 0 : ((ارزښت & TWINE_SERIAL_ماسک) >> TWINE_SERIAL_ښويدنه);
	}

	constexpr IdString stamped(اندازه_t pool_serial) ثابت {
		أرجع ارزښت == TWINE_NULL_VAL ? *this
			: IdString((ارزښت & ~TWINE_SERIAL_ماسک)
					| (بټونه_t(pool_serial << TWINE_SERIAL_ښويدنه) & TWINE_SERIAL_ماسک));
	}

	constexpr IdString() : ارزښت(TWINE_NULL_VAL) {}
	explicit constexpr IdString(اندازه_t val) : ارزښت((بټونه_t) val) {}

	constexpr اندازه_t eq_کلی() ثابت { أرجع ارزښت & ~TWINE_SERIAL_ماسک; }

	constexpr bool چلوونکی==(ثابت IdString &rhs) ثابت { أرجع eq_کلی() == rhs.eq_کلی(); }
	constexpr bool چلوونکی!=(ثابت IdString &rhs) ثابت { أرجع eq_کلی() != rhs.eq_کلی(); }
	constexpr std::strong_ordering چلوونکی<=>(ثابت IdString &rhs) ثابت {
		أرجع ترتیب_کلی() <=> rhs.ترتیب_کلی();
	}

	کينډۍ <typename... دلیلونه_3>
	constexpr bool دننه(ثابت دلیلونه_3&... دلیلونه_2) ثابت {
		أرجع (دننه_one(دلیلونه_2) || ...);
	}
	bool دننه(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	کينډۍ<typename T> بنية compare_ptr_by_name {
		bool چلوونکی()(ثابت T *a, ثابت T *b) ثابت {
			أرجع (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool تشه() ثابت { أرجع ارزښت == TWINE_NULL_VAL; }
	constexpr فراغ clear() { ارزښت = TWINE_NULL_VAL; }
	constexpr bool is_عامه() ثابت { أرجع ارزښت != TWINE_NULL_VAL && (ارزښت & TWINE_عامه_بټ); }

	constexpr IdString untag() ثابت {
		أرجع ارزښت == TWINE_NULL_VAL ? *this : IdString(ارزښت & ~TWINE_عامه_بټ);
	}
	constexpr IdString نښان(bool pub) ثابت {
		أرجع ارزښت == TWINE_NULL_VAL ? *this : IdString(pub ? (ارزښت | TWINE_عامه_بټ) : (ارزښت & ~TWINE_عامه_بټ));
	}

	std::string handle_token() ثابت;
	ساكن اندازه_t handle_token_مختاړی(std::string_view token, bool &is_عامه_2);

خاص:
	constexpr bool دننه_one(IdString rhs) ثابت { أرجع *this == rhs; }
	bool دننه_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::چلوونکی IdString() ثابت { أرجع IdString(); }
constexpr bool NullIdString::چلوونکی==(IdString ref) ثابت { أرجع ref.تشه(); }

namespace hashlib {
	کينډۍ<>
	بنية hash_ops<IdString> {
		ساكن مضمن bool cmp(IdString a, IdString b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_کلی());
			أرجع h;
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_کلی());
			أرجع h;
		}
	};
}

مضمن bool IdString::دننه_one(ثابت pool<IdString> &rhs) ثابت { أرجع rhs.شمېر(*this) != 0; }
مضمن bool IdString::دننه(ثابت pool<IdString> &rhs) ثابت { أرجع دننه_one(rhs); }


تعداد : لنډ {
#تعریف_کړه X(N) IDX_##N,
#ضمن "kernel/constids.inc"
#تعریف_لغوه X
	STATIC_TWINE_پای_ټکی
};

بنية ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#تعریف_کړه X(N) ساكن constexpr IdString N = IdString(IDX_##N).نښان((#N)[0] != '$');
#ضمن "kernel/constids.inc"
#تعریف_لغوه X

	ساكن constexpr ثابت حرف* static_نومونه[] = {
#تعریف_کړه X(N) #N,
#ضمن "kernel/constids.inc"
#تعریف_لغوه X
	};

	ساكن constexpr IdString lookup(std::string_view name)
	{
		صحيح ټیټ = 0, لوړ = STATIC_TWINE_پای_ټکی;
		طالما (لوړ - ټیټ >= 2) {
			صحيح mid = (ټیټ + لوړ) / 2;
			إذا (name < static_نومونه[mid])
				لوړ = mid;
			وإلا
				ټیټ = mid;
		}

		إذا (name != static_نومونه[ټیټ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		أرجع IdString(ټیټ).نښان(name[0] != '$');
	}

	ساكن constexpr bool is_static(IdString ref) {
		أرجع ref.untag().raw() < STATIC_TWINE_پای_ټکی;
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

کينډۍ<اندازه_t Raw> مضمن constexpr IdString constid = ID::static_ref(Raw);

#تعریف_کړه ID(id) (YOSYS_NAMESPACE_مختاړی constid<YOSYS_NAMESPACE_مختاړی ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
بنية TwineSpec {
	// deduplicates shared prefixes
	بنية Suffix {
		IdString مختاړی_2;
		std::string لکۍ;
		خپلکاری چلوونکی<=>(ثابت Suffix&) ثابت = افتراضي;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	بنية AutoSuffix {
		ثابت std::string *مختاړی_2;
		std::string لکۍ;
		خپلکاری چلوونکی<=>(ثابت AutoSuffix&) ثابت = افتراضي;
	};

	// "leaf", regular deduplicated string
	بنية ونې_پاڼه {
		std::string s;
		خپلکاری چلوونکی<=>(ثابت ونې_پاڼه&) ثابت = افتراضي;
	};

	std::variant<ونې_پاڼه, Suffix, AutoSuffix> معلومات;

	TwineSpec(ونې_پاڼه v);
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
	SmallString &چلوونکی=(ثابت SmallString &other);
	SmallString &چلوونکی=(SmallString &&other) noexcept;
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
بنية Twine_غوټه {
	ساكن constexpr uint32_t NO_مختاړی = ~uint32_t{0};
	ساكن constexpr uint32_t DEAD = NO_مختاړی - 1;

	بنية کلی_2 {
		uint32_t مختاړی_2;
		std::string_view متن;
	};

	ساكن constexpr uint32_t extend_hash(uint32_t hash, std::string_view لکۍ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		لكل (حرف c : لکۍ)
			hash = ((hash << 5) + hash) + static_cast<بینښه حرف>(c);
		أرجع hash;
	}

	Twine_غوټه() = افتراضي;
	explicit Twine_غوټه(std::string_view content)
		: متن_2(content), مختاړی_3(NO_مختاړی), hash_(extend_hash(0, content)) {}
	Twine_غوټه(uint32_t مختاړی_2, std::string_view لکۍ, uint32_t hash)
		: متن_2(لکۍ), مختاړی_3(مختاړی_2), hash_(hash) {}
	Twine_غوټه(ثابت Twine_غوټه &other) = افتراضي;
	Twine_غوټه(Twine_غوټه &&other) noexcept;
	Twine_غوټه &چلوونکی=(ثابت Twine_غوټه &other) = افتراضي;
	Twine_غوټه &چلوونکی=(Twine_غوټه &&other) noexcept;
	~Twine_غوټه() = افتراضي;

	تعداد class ډول_2 { Dead, ونې_پاڼه, Suffix };

	constexpr bool is_dead() ثابت { أرجع مختاړی_3 == DEAD; }
	constexpr bool is_ونې_پاڼه() ثابت { أرجع مختاړی_3 == NO_مختاړی; }
	constexpr bool is_suffix() ثابت { أرجع مختاړی_3 < DEAD; }

	constexpr ډول_2 ډول() ثابت {
		أرجع مختاړی_3 == DEAD ? ډول_2::Dead
			: مختاړی_3 == NO_مختاړی ? ډول_2::ونې_پاڼه
			: ډول_2::Suffix;
	}

	std::string_view متن() ثابت { أرجع متن_2.view(); }
	IdString مختاړی_2() ثابت { أرجع IdString(مختاړی_3); }
	uint32_t hash() ثابت { أرجع hash_; }
	کلی_2 کلی() ثابت { أرجع {مختاړی_3, متن()}; }

	bool چلوونکی==(ثابت Twine_غوټه &other) ثابت
		{ أرجع مختاړی_3 == other.مختاړی_3 && متن() == other.متن(); }
	bool چلوونکی==(ثابت کلی_2 &other) ثابت
		{ أرجع مختاړی_3 == other.مختاړی_2 && متن() == other.متن; }

خاص:
	SmallString متن_2;
	uint32_t مختاړی_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_غوټه) == (حجم(فراغ *) == 8 ? 24 : 20));

بنية StaticTwines {
	ساكن فراغ init();
	ساكن ثابت Twine_غوټه &غوټه(اندازه_t idx);
	ساكن bool ready();

خاص:
	ساكن std::ویکټور<Twine_غوټه> nodes_;
};

خارجي int64_t twine_gc_ns;
خارجي صحيح twine_gc_شمېر;

بنية TwinePool : HashConsPool<TwinePool, Twine_غوټه, IdString> {
	ساكن constexpr اندازه_t STATIC_شمېر = STATIC_TWINE_پای_ټکی;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& چلوونکی=(ثابت TwinePool& other);
	TwinePool& چلوونکی=(TwinePool&& other);

	ثابت Twine_غوټه& چلوونکی[](IdString ref) ثابت;

	ساكن ثابت Twine_غوټه& static_غوټه(اندازه_t idx);

	ساكن فراغ check_ready();
	ساكن uint32_t hash_غوټه(ثابت Twine_غوټه& t) { أرجع t.hash(); }

	کينډۍ<typename F>
	ساكن فراغ for_each_ماشوم(ثابت Twine_غوټه& t, F&& f) {
		إذا (t.is_suffix())
			f(t.مختاړی_2());
	}

	اندازه_t gc(pool<IdString>& roots);

	فراغ dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	اندازه_t str_اندازه(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view مختاړی_2) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحيح compare_by_name(IdString a, IdString b) ثابت;
	IdString مختاړی_د(IdString ref) ثابت;

	IdString پیدا_کړه(std::string_view name) ثابت;
	IdString پیدا_کړه(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString جمع_کړه(TwineSpec t);
	IdString جمع_کړه(IdString مختاړی_2, std::string_view لکۍ);
	// Infers publicity from first character
	IdString جمع_کړه(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString پیدا_کړه_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	فراغ check_owned(IdString ref) ثابت;

خاص:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	فراغ append_str(IdString ref, std::string& بهر) ثابت;
	IdString auto_مختاړی(ثابت std::string *مختاړی_2);
	IdString جمع_کړه_inner(Twine_غوټه t);
	IdString intern(uint32_t مختاړی_2, std::string_view متن);
	uint32_t content_hash(uint32_t مختاړی_2, std::string_view متن) ثابت;
	IdString پیدا_کړه_content(uint32_t مختاړی_2, std::string_view متن, uint32_t hash) ثابت;
	IdString پیدا_کړه_content(uint32_t مختاړی_2, std::string_view متن) ثابت;
	ساكن اندازه_t بل_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	اندازه_t serial_;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
