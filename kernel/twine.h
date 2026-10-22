#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

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
	constexpr operator IdString() ثابت;
	constexpr bool operator==(IdString ref) ثابت;
};

بنية IdString {
خاص:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#وإلا
	using bits_t = uint32_t;
#endif
	bits_t قىممەت;

	ساكن constexpr صحيح TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	ساكن constexpr صحيح TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	ساكن constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	ساكن constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	ساكن constexpr صحيح TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	ساكن constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	ساكن constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

عام:
	ساكن constexpr NullIdString Null{};

	ساكن constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	ساكن constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ثابت {
		أرجع قىممەت == TWINE_NULL_VAL ? قىممەت : (قىممەت & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ثابت { أرجع قىممەت; }

	constexpr size_t order_ئاچقۇچ() ثابت { أرجع std::rotl((bits_t) eq_ئاچقۇچ(), 1); }

	constexpr size_t serial() ثابت {
		أرجع قىممەت == TWINE_NULL_VAL ? 0 : ((قىممەت & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ثابت {
		أرجع قىممەت == TWINE_NULL_VAL ? *this
			: IdString((قىممەت & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : قىممەت(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : قىممەت((bits_t) val) {}

	constexpr size_t eq_ئاچقۇچ() ثابت { أرجع قىممەت & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ثابت IdString &rhs) ثابت { أرجع eq_ئاچقۇچ() == rhs.eq_ئاچقۇچ(); }
	constexpr bool operator!=(ثابت IdString &rhs) ثابت { أرجع eq_ئاچقۇچ() != rhs.eq_ئاچقۇچ(); }
	constexpr std::strong_ordering operator<=>(ثابت IdString &rhs) ثابت {
		أرجع order_ئاچقۇچ() <=> rhs.order_ئاچقۇچ();
	}

	template <typename... Args>
	constexpr bool in(ثابت Args&... args) ثابت {
		أرجع (in_one(args) || ...);
	}
	bool in(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> بنية compare_ptr_by_name {
		bool operator()(ثابت T *a, ثابت T *b) ثابت {
			أرجع (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ثابت { أرجع قىممەت == TWINE_NULL_VAL; }
	constexpr فراغ clear() { قىممەت = TWINE_NULL_VAL; }
	constexpr bool isPublic() ثابت { أرجع قىممەت != TWINE_NULL_VAL && (قىممەت & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ثابت {
		أرجع قىممەت == TWINE_NULL_VAL ? *this : IdString(قىممەت & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ثابت {
		أرجع قىممەت == TWINE_NULL_VAL ? *this : IdString(pub ? (قىممەت | TWINE_PUBLIC_BIT) : (قىممەت & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ثابت;
	ساكن size_t handle_token_prefix(std::string_view token, bool &is_public);

خاص:
	constexpr bool in_one(IdString rhs) ثابت { أرجع *this == rhs; }
	bool in_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::operator IdString() ثابت { أرجع IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ثابت { أرجع ref.empty(); }

namespace hashlib {
	template<>
	بنية hash_ops<IdString> {
		ساكن مضمن bool cmp(IdString a, IdString b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ئاچقۇچ());
			أرجع h;
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ئاچقۇچ());
			أرجع h;
		}
	};
}

مضمن bool IdString::in_one(ثابت pool<IdString> &rhs) ثابت { أرجع rhs.سانى(*this) != 0; }
مضمن bool IdString::in(ثابت pool<IdString> &rhs) ثابت { أرجع in_one(rhs); }


تعداد : short {
#define X(N) IDX_##N,
#ضمن "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

بنية ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) ساكن constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#ضمن "kernel/constids.inc"
#undef X

	ساكن constexpr ثابت حرف* static_names[] = {
#define X(N) #N,
#ضمن "kernel/constids.inc"
#undef X
	};

	ساكن constexpr IdString lookup(std::string_view name)
	{
		صحيح تۆۋەن = 0, يۇقىرى = STATIC_TWINE_END;
		طالما (يۇقىرى - تۆۋەن >= 2) {
			صحيح mid = (تۆۋەن + يۇقىرى) / 2;
			إذا (name < static_names[mid])
				يۇقىرى = mid;
			وإلا
				تۆۋەن = mid;
		}

		إذا (name != static_names[تۆۋەن])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		أرجع IdString(تۆۋەن).tag(name[0] != '$');
	}

	ساكن constexpr bool is_static(IdString ref) {
		أرجع ref.untag().raw() < STATIC_TWINE_END;
	}

	ساكن constexpr IdString static_ref(size_t raw) {
		إذا (!is_static(IdString(raw)))
			throw "not a static twine id";
		أرجع IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ساكن std::string str(IdString ref);
	ساكن std::string unescaped_str(IdString ref);
};

template<size_t Raw> مضمن constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
بنية TwineSpec {
	// deduplicates shared prefixes
	بنية Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ثابت Suffix&) ثابت = افتراضي;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	بنية AutoSuffix {
		ثابت std::string *prefix;
		std::string tail;
		auto operator<=>(ثابت AutoSuffix&) ثابت = افتراضي;
	};

	// "leaf", regular deduplicated string
	بنية Leaf {
		std::string s;
		auto operator<=>(ثابت Leaf&) ثابت = افتراضي;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

بنية SmallString {
	ساكن constexpr uint32_t INLINE_CAP = 8;
	ساكن constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = افتراضي;
	explicit SmallString(std::string_view content);
	SmallString(ثابت SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ثابت SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
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
بنية Twine_تۈگۈن {
	ساكن constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	ساكن constexpr uint32_t DEAD = NO_PREFIX - 1;

	بنية ئاچقۇچ_2 {
		uint32_t prefix;
		std::string_view text;
	};

	ساكن constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		لكل (حرف c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned حرف>(c);
		أرجع hash;
	}

	Twine_تۈگۈن() = افتراضي;
	explicit Twine_تۈگۈن(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_تۈگۈن(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_تۈگۈن(ثابت Twine_تۈگۈن &other) = افتراضي;
	Twine_تۈگۈن(Twine_تۈگۈن &&other) noexcept;
	Twine_تۈگۈن &operator=(ثابت Twine_تۈگۈن &other) = افتراضي;
	Twine_تۈگۈن &operator=(Twine_تۈگۈن &&other) noexcept;
	~Twine_تۈگۈن() = افتراضي;

	تعداد class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ثابت { أرجع prefix_ == DEAD; }
	constexpr bool is_leaf() ثابت { أرجع prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ثابت { أرجع prefix_ < DEAD; }

	constexpr Kind kind() ثابت {
		أرجع prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ثابت { أرجع text_.view(); }
	IdString prefix() ثابت { أرجع IdString(prefix_); }
	uint32_t hash() ثابت { أرجع hash_; }
	ئاچقۇچ_2 ئاچقۇچ() ثابت { أرجع {prefix_, text()}; }

	bool operator==(ثابت Twine_تۈگۈن &other) ثابت
		{ أرجع prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ثابت ئاچقۇچ_2 &other) ثابت
		{ أرجع prefix_ == other.prefix && text() == other.text; }

خاص:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(حجم(Twine_تۈگۈن) == (حجم(فراغ *) == 8 ? 24 : 20));

بنية StaticTwines {
	ساكن فراغ init();
	ساكن ثابت Twine_تۈگۈن &تۈگۈن(size_t idx);
	ساكن bool ready();

خاص:
	ساكن std::vector<Twine_تۈگۈن> nodes_;
};

خارجي int64_t twine_gc_ns;
خارجي صحيح twine_gc_سانى;

بنية TwinePool : HashConsPool<TwinePool, Twine_تۈگۈن, IdString> {
	ساكن constexpr size_t STATIC_سانى = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ثابت TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ثابت Twine_تۈگۈن& operator[](IdString ref) ثابت;

	ساكن ثابت Twine_تۈگۈن& static_تۈگۈن(size_t idx);

	ساكن فراغ check_ready();
	ساكن uint32_t hash_تۈگۈن(ثابت Twine_تۈگۈن& t) { أرجع t.hash(); }

	template<typename F>
	ساكن فراغ for_each_child(ثابت Twine_تۈگۈن& t, F&& f) {
		إذا (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	فراغ dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	size_t str_size(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view prefix) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحيح compare_by_name(IdString a, IdString b) ثابت;
	IdString prefix_of(IdString ref) ثابت;

	IdString find(std::string_view name) ثابت;
	IdString find(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	فراغ check_owned(IdString ref) ثابت;

خاص:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	فراغ append_str(IdString ref, std::string& out) ثابت;
	IdString auto_prefix(ثابت std::string *prefix);
	IdString add_inner(Twine_تۈگۈن t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ثابت;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت;
	IdString find_content(uint32_t prefix, std::string_view text) ثابت;
	ساكن size_t next_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
