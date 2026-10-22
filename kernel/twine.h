#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#অন্তর্ভাব "kernel/hashcons.h"
#অন্তর্ভাব "kernel/yosys_common.h"

#অন্তর্ভাব <algorithm>
#অন্তর্ভাব <bit>
#অন্তর্ভাব <cstdint>
#অন্তর্ভাব <cstring>
#অন্তর্ভাব <deque>
#অন্তর্ভাব <limits>
#অন্তর্ভাব <span>
#অন্তর্ভাব <string>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <unordered_set>
#অন্তর্ভাব <variant>
#অন্তর্ভাব <vector>

YOSYS_NAMESPACE_BEGIN

কাঠামো TwineSpec;
কাঠামো TwinePool;
কাঠামো IdString;

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

কাঠামো NullIdString {
	constexpr operator IdString() ধ্রুবক;
	constexpr bool operator==(IdString ref) ধ্রুবক;
};

কাঠামো IdString {
ব্যক্তিগত:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#অন্যথা
	using bits_t = uint32_t;
#endif
	bits_t মান;

	স্থির constexpr পূর্ণসংখ্যা TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	স্থির constexpr পূর্ণসংখ্যা TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	স্থির constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	স্থির constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	স্থির constexpr পূর্ণসংখ্যা TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	স্থির constexpr bits_t TWINE_সূচক_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	স্থির constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_সূচক_MASK | TWINE_PUBLIC_BIT));

সর্বজনীন:
	স্থির constexpr NullIdString Null{};

	স্থির constexpr আকার_t MAX_সূচক = TWINE_সূচক_MASK;
	স্থির constexpr আকার_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr আকার_t raw() ধ্রুবক {
		ফেরত মান == TWINE_NULL_VAL ? মান : (মান & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr আকার_t bits() ধ্রুবক { ফেরত মান; }

	constexpr আকার_t order_চাবি() ধ্রুবক { ফেরত std::rotl((bits_t) eq_চাবি(), 1); }

	constexpr আকার_t serial() ধ্রুবক {
		ফেরত মান == TWINE_NULL_VAL ? 0 : ((মান & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(আকার_t pool_serial) ধ্রুবক {
		ফেরত মান == TWINE_NULL_VAL ? *this
			: IdString((মান & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : মান(TWINE_NULL_VAL) {}
	explicit constexpr IdString(আকার_t val) : মান((bits_t) val) {}

	constexpr আকার_t eq_চাবি() ধ্রুবক { ফেরত মান & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ধ্রুবক IdString &rhs) ধ্রুবক { ফেরত eq_চাবি() == rhs.eq_চাবি(); }
	constexpr bool operator!=(ধ্রুবক IdString &rhs) ধ্রুবক { ফেরত eq_চাবি() != rhs.eq_চাবি(); }
	constexpr std::strong_ordering operator<=>(ধ্রুবক IdString &rhs) ধ্রুবক {
		ফেরত order_চাবি() <=> rhs.order_চাবি();
	}

	template <typename... Args>
	constexpr bool in(ধ্রুবক Args&... args) ধ্রুবক {
		ফেরত (in_one(args) || ...);
	}
	bool in(ধ্রুবক pool<IdString> &rhs) ধ্রুবক;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> কাঠামো compare_ptr_by_name {
		bool operator()(ধ্রুবক T *a, ধ্রুবক T *b) ধ্রুবক {
			ফেরত (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ধ্রুবক { ফেরত মান == TWINE_NULL_VAL; }
	constexpr শূন্য clear() { মান = TWINE_NULL_VAL; }
	constexpr bool isPublic() ধ্রুবক { ফেরত মান != TWINE_NULL_VAL && (মান & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ধ্রুবক {
		ফেরত মান == TWINE_NULL_VAL ? *this : IdString(মান & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ধ্রুবক {
		ফেরত মান == TWINE_NULL_VAL ? *this : IdString(pub ? (মান | TWINE_PUBLIC_BIT) : (মান & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ধ্রুবক;
	স্থির আকার_t handle_token_prefix(std::string_view token, bool &is_public);

ব্যক্তিগত:
	constexpr bool in_one(IdString rhs) ধ্রুবক { ফেরত *this == rhs; }
	bool in_one(ধ্রুবক pool<IdString> &rhs) ধ্রুবক;
};

constexpr NullIdString::operator IdString() ধ্রুবক { ফেরত IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ধ্রুবক { ফেরত ref.empty(); }

namespace hashlib {
	template<>
	কাঠামো hash_ops<IdString> {
		স্থির অন্তঃরেখ bool cmp(IdString a, IdString b) { ফেরত a == b; }
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_চাবি());
			ফেরত h;
		}
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_চাবি());
			ফেরত h;
		}
	};
}

অন্তঃরেখ bool IdString::in_one(ধ্রুবক pool<IdString> &rhs) ধ্রুবক { ফেরত rhs.গুনতি(*this) != 0; }
অন্তঃরেখ bool IdString::in(ধ্রুবক pool<IdString> &rhs) ধ্রুবক { ফেরত in_one(rhs); }


গণনা : short {
#define X(N) IDX_##N,
#অন্তর্ভাব "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

কাঠামো ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) স্থির constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#অন্তর্ভাব "kernel/constids.inc"
#undef X

	স্থির constexpr ধ্রুবক অক্ষর* static_names[] = {
#define X(N) #N,
#অন্তর্ভাব "kernel/constids.inc"
#undef X
	};

	স্থির constexpr IdString lookup(std::string_view name)
	{
		পূর্ণসংখ্যা নিচের = 0, উপরের = STATIC_TWINE_END;
		যখন (উপরের - নিচের >= 2) {
			পূর্ণসংখ্যা mid = (নিচের + উপরের) / 2;
			যদি (name < static_names[mid])
				উপরের = mid;
			অন্যথা
				নিচের = mid;
		}

		যদি (name != static_names[নিচের])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		ফেরত IdString(নিচের).tag(name[0] != '$');
	}

	স্থির constexpr bool is_static(IdString ref) {
		ফেরত ref.untag().raw() < STATIC_TWINE_END;
	}

	স্থির constexpr IdString static_ref(আকার_t raw) {
		যদি (!is_static(IdString(raw)))
			throw "not a static twine id";
		ফেরত IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	স্থির std::string str(IdString ref);
	স্থির std::string unescaped_str(IdString ref);
};

template<আকার_t Raw> অন্তঃরেখ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
কাঠামো TwineSpec {
	// deduplicates shared prefixes
	কাঠামো Suffix {
		IdString prefix;
		std::string লেজ;
		auto operator<=>(ধ্রুবক Suffix&) ধ্রুবক = পূর্বনির্ধারিত;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	কাঠামো AutoSuffix {
		ধ্রুবক std::string *prefix;
		std::string লেজ;
		auto operator<=>(ধ্রুবক AutoSuffix&) ধ্রুবক = পূর্বনির্ধারিত;
	};

	// "leaf", regular deduplicated string
	কাঠামো Leaf {
		std::string s;
		auto operator<=>(ধ্রুবক Leaf&) ধ্রুবক = পূর্বনির্ধারিত;
	};

	std::variant<Leaf, Suffix, AutoSuffix> তথ্য;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ধ্রুবক;
};

কাঠামো SmallString {
	স্থির constexpr uint32_t INLINE_CAP = 8;
	স্থির constexpr আকার_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = পূর্বনির্ধারিত;
	explicit SmallString(std::string_view content);
	SmallString(ধ্রুবক SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ধ্রুবক SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ধ্রুবক { ফেরত {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ব্যক্তিগত:
	শূন্য store(std::string_view content);
	শূন্য release();

	সংঘ {
		অক্ষর inl_[INLINE_CAP];
		অক্ষর *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
কাঠামো Twine_গ্রন্থি {
	স্থির constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	স্থির constexpr uint32_t DEAD = NO_PREFIX - 1;

	কাঠামো চাবি_2 {
		uint32_t prefix;
		std::string_view text;
	};

	স্থির constexpr uint32_t extend_hash(uint32_t hash, std::string_view লেজ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		জন্য (অক্ষর c : লেজ)
			hash = ((hash << 5) + hash) + static_cast<unsigned অক্ষর>(c);
		ফেরত hash;
	}

	Twine_গ্রন্থি() = পূর্বনির্ধারিত;
	explicit Twine_গ্রন্থি(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_গ্রন্থি(uint32_t prefix, std::string_view লেজ, uint32_t hash)
		: text_(লেজ), prefix_(prefix), hash_(hash) {}
	Twine_গ্রন্থি(ধ্রুবক Twine_গ্রন্থি &other) = পূর্বনির্ধারিত;
	Twine_গ্রন্থি(Twine_গ্রন্থি &&other) noexcept;
	Twine_গ্রন্থি &operator=(ধ্রুবক Twine_গ্রন্থি &other) = পূর্বনির্ধারিত;
	Twine_গ্রন্থি &operator=(Twine_গ্রন্থি &&other) noexcept;
	~Twine_গ্রন্থি() = পূর্বনির্ধারিত;

	গণনা class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ধ্রুবক { ফেরত prefix_ == DEAD; }
	constexpr bool is_leaf() ধ্রুবক { ফেরত prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ধ্রুবক { ফেরত prefix_ < DEAD; }

	constexpr Kind kind() ধ্রুবক {
		ফেরত prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ধ্রুবক { ফেরত text_.view(); }
	IdString prefix() ধ্রুবক { ফেরত IdString(prefix_); }
	uint32_t hash() ধ্রুবক { ফেরত hash_; }
	চাবি_2 চাবি() ধ্রুবক { ফেরত {prefix_, text()}; }

	bool operator==(ধ্রুবক Twine_গ্রন্থি &other) ধ্রুবক
		{ ফেরত prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ধ্রুবক চাবি_2 &other) ধ্রুবক
		{ ফেরত prefix_ == other.prefix && text() == other.text; }

ব্যক্তিগত:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(আকার(Twine_গ্রন্থি) == (আকার(শূন্য *) == 8 ? 24 : 20));

কাঠামো StaticTwines {
	স্থির শূন্য init();
	স্থির ধ্রুবক Twine_গ্রন্থি &গ্রন্থি(আকার_t idx);
	স্থির bool ready();

ব্যক্তিগত:
	স্থির std::vector<Twine_গ্রন্থি> nodes_;
};

বাহ্যিক int64_t twine_gc_ns;
বাহ্যিক পূর্ণসংখ্যা twine_gc_গুনতি;

কাঠামো TwinePool : HashConsPool<TwinePool, Twine_গ্রন্থি, IdString> {
	স্থির constexpr আকার_t STATIC_গুনতি = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ধ্রুবক TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ধ্রুবক TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ধ্রুবক Twine_গ্রন্থি& operator[](IdString ref) ধ্রুবক;

	স্থির ধ্রুবক Twine_গ্রন্থি& static_গ্রন্থি(আকার_t idx);

	স্থির শূন্য check_ready();
	স্থির uint32_t hash_গ্রন্থি(ধ্রুবক Twine_গ্রন্থি& t) { ফেরত t.hash(); }

	template<typename F>
	স্থির শূন্য for_each_child(ধ্রুবক Twine_গ্রন্থি& t, F&& f) {
		যদি (t.is_suffix())
			f(t.prefix());
	}

	আকার_t gc(pool<IdString>& roots);

	শূন্য dump(IdString ref, std::ostream& os = std::cout) ধ্রুবক;

	// Publicity bit provides escaping
	std::string str(IdString ref) ধ্রুবক;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ধ্রুবক;

	আকার_t str_আকার(IdString ref) ধ্রুবক;
	bool begins_with(IdString ref, std::string_view prefix) ধ্রুবক;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ধ্রুবক;
	পূর্ণসংখ্যা compare_by_name(IdString a, IdString b) ধ্রুবক;
	IdString prefix_of(IdString ref) ধ্রুবক;

	IdString find(std::string_view name) ধ্রুবক;
	IdString find(TwineSpec t) ধ্রুবক;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view লেজ);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ধ্রুবক TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ধ্রুবক TwinePool& src, IdString ref) ধ্রুবক;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ধ্রুবক;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ধ্রুবক;
	শূন্য check_owned(IdString ref) ধ্রুবক;

ব্যক্তিগত:
	bool owns(IdString ref) ধ্রুবক;
	IdString stamp(IdString ref) ধ্রুবক;
	শূন্য append_str(IdString ref, std::string& out) ধ্রুবক;
	IdString auto_prefix(ধ্রুবক std::string *prefix);
	IdString add_inner(Twine_গ্রন্থি t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ধ্রুবক;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ধ্রুবক;
	IdString find_content(uint32_t prefix, std::string_view text) ধ্রুবক;
	স্থির আকার_t পরবর্তী_serial();
	dict<ধ্রুবক std::string *, IdString> auto_prefixes;
	আকার_t serial_;
};

YOSYS_NAMESPACE_END

#endif
