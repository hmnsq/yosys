#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#ಅನ್ತರ್ಭಾವ "kernel/hashcons.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <bit>
#ಅನ್ತರ್ಭಾವ <cstdint>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <limits>
#ಅನ್ತರ್ಭಾವ <span>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <unordered_set>
#ಅನ್ತರ್ಭಾವ <variant>
#ಅನ್ತರ್ಭಾವ <vector>

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ TwineSpec;
ಸಂರಚನಾ TwinePool;
ಸಂರಚನಾ IdString;

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

ಸಂರಚನಾ NullIdString {
	constexpr operator IdString() ಧ್ರುವ;
	constexpr bool operator==(IdString ref) ಧ್ರುವ;
};

ಸಂರಚನಾ IdString {
ಗುಪ್ತ:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#ಅನ್ಯಥಾ
	using bits_t = uint32_t;
#endif
	bits_t ಮೌಲ್ಯ;

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	ಸ್ಥಿರ constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	ಸ್ಥಿರ constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	ಸ್ಥಿರ constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	ಸ್ಥಿರ constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ constexpr NullIdString Null{};

	ಸ್ಥಿರ constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	ಸ್ಥಿರ constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL ? ಮೌಲ್ಯ : (ಮೌಲ್ಯ & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ; }

	constexpr size_t order_ಕೀಲಿ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::rotl((bits_t) eq_ಕೀಲಿ(), 1); }

	constexpr size_t serial() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL ? 0 : ((ಮೌಲ್ಯ & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this
			: IdString((ಮೌಲ್ಯ & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : ಮೌಲ್ಯ(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : ಮೌಲ್ಯ((bits_t) val) {}

	constexpr size_t eq_ಕೀಲಿ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ಧ್ರುವ IdString &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ eq_ಕೀಲಿ() == rhs.eq_ಕೀಲಿ(); }
	constexpr bool operator!=(ಧ್ರುವ IdString &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ eq_ಕೀಲಿ() != rhs.eq_ಕೀಲಿ(); }
	constexpr std::strong_ordering operator<=>(ಧ್ರುವ IdString &rhs) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ order_ಕೀಲಿ() <=> rhs.order_ಕೀಲಿ();
	}

	template <typename... Args>
	constexpr bool in(ಧ್ರುವ Args&... args) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ (in_one(args) || ...);
	}
	bool in(ಧ್ರುವ pool<IdString> &rhs) ಧ್ರುವ;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> ಸಂರಚನಾ compare_ptr_by_name {
		bool operator()(ಧ್ರುವ T *a, ಧ್ರುವ T *b) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL; }
	constexpr ಶೂನ್ಯ clear() { ಮೌಲ್ಯ = TWINE_NULL_VAL; }
	constexpr bool isPublic() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ != TWINE_NULL_VAL && (ಮೌಲ್ಯ & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this : IdString(ಮೌಲ್ಯ & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this : IdString(pub ? (ಮೌಲ್ಯ | TWINE_PUBLIC_BIT) : (ಮೌಲ್ಯ & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ಧ್ರುವ;
	ಸ್ಥಿರ size_t handle_token_prefix(std::string_view token, bool &is_public);

ಗುಪ್ತ:
	constexpr bool in_one(IdString rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *this == rhs; }
	bool in_one(ಧ್ರುವ pool<IdString> &rhs) ಧ್ರುವ;
};

constexpr NullIdString::operator IdString() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ref.empty(); }

namespace hashlib {
	template<>
	ಸಂರಚನಾ hash_ops<IdString> {
		ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(IdString a, IdString b) { ಪ್ರತ್ಯಾಗಮ a == b; }
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ಕೀಲಿ());
			ಪ್ರತ್ಯಾಗಮ h;
		}
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ಕೀಲಿ());
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};
}

ಅನ್ತಃಸ್ಥ bool IdString::in_one(ಧ್ರುವ pool<IdString> &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ rhs.ಎಣಿಕೆ(*this) != 0; }
ಅನ್ತಃಸ್ಥ bool IdString::in(ಧ್ರುವ pool<IdString> &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ in_one(rhs); }


ಗಣನಾ : short {
#define X(N) IDX_##N,
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

ಸಂರಚನಾ ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) ಸ್ಥಿರ constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#undef X

	ಸ್ಥಿರ constexpr ಧ್ರುವ ವರ್ಣ* static_names[] = {
#define X(N) #N,
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#undef X
	};

	ಸ್ಥಿರ constexpr IdString lookup(std::string_view name)
	{
		ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ = 0, ಮೇಲಿನ = STATIC_TWINE_END;
		ಯಾವತ್ (ಮೇಲಿನ - ಕೆಳಗಿನ >= 2) {
			ಪೂರ್ಣಾಙ್ಕ mid = (ಕೆಳಗಿನ + ಮೇಲಿನ) / 2;
			ಯದಿ (name < static_names[mid])
				ಮೇಲಿನ = mid;
			ಅನ್ಯಥಾ
				ಕೆಳಗಿನ = mid;
		}

		ಯದಿ (name != static_names[ಕೆಳಗಿನ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		ಪ್ರತ್ಯಾಗಮ IdString(ಕೆಳಗಿನ).tag(name[0] != '$');
	}

	ಸ್ಥಿರ constexpr bool is_static(IdString ref) {
		ಪ್ರತ್ಯಾಗಮ ref.untag().raw() < STATIC_TWINE_END;
	}

	ಸ್ಥಿರ constexpr IdString static_ref(size_t raw) {
		ಯದಿ (!is_static(IdString(raw)))
			throw "not a static twine id";
		ಪ್ರತ್ಯಾಗಮ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ಸ್ಥಿರ std::string str(IdString ref);
	ಸ್ಥಿರ std::string unescaped_str(IdString ref);
};

template<size_t Raw> ಅನ್ತಃಸ್ಥ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ಸಂರಚನಾ TwineSpec {
	// deduplicates shared prefixes
	ಸಂರಚನಾ Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ಧ್ರುವ Suffix&) ಧ್ರುವ = ಸ್ವಭಾವ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ಸಂರಚನಾ AutoSuffix {
		ಧ್ರುವ std::string *prefix;
		std::string tail;
		auto operator<=>(ಧ್ರುವ AutoSuffix&) ಧ್ರುವ = ಸ್ವಭಾವ;
	};

	// "leaf", regular deduplicated string
	ಸಂರಚನಾ Leaf {
		std::string s;
		auto operator<=>(ಧ್ರುವ Leaf&) ಧ್ರುವ = ಸ್ವಭಾವ;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ಧ್ರುವ;
};

ಸಂರಚನಾ SmallString {
	ಸ್ಥಿರ constexpr uint32_t INLINE_CAP = 8;
	ಸ್ಥಿರ constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = ಸ್ವಭಾವ;
	explicit SmallString(std::string_view content);
	SmallString(ಧ್ರುವ SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ಧ್ರುವ SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ಗುಪ್ತ:
	ಶೂನ್ಯ store(std::string_view content);
	ಶೂನ್ಯ release();

	ಸಙ್ಘ {
		ವರ್ಣ inl_[INLINE_CAP];
		ವರ್ಣ *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ಸಂರಚನಾ Twine_ಗಂಟು {
	ಸ್ಥಿರ constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	ಸ್ಥಿರ constexpr uint32_t DEAD = NO_PREFIX - 1;

	ಸಂರಚನಾ ಕೀಲಿ_2 {
		uint32_t prefix;
		std::string_view text;
	};

	ಸ್ಥಿರ constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		ಪ್ರತಿ (ವರ್ಣ c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned ವರ್ಣ>(c);
		ಪ್ರತ್ಯಾಗಮ hash;
	}

	Twine_ಗಂಟು() = ಸ್ವಭಾವ;
	explicit Twine_ಗಂಟು(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_ಗಂಟು(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_ಗಂಟು(ಧ್ರುವ Twine_ಗಂಟು &other) = ಸ್ವಭಾವ;
	Twine_ಗಂಟು(Twine_ಗಂಟು &&other) noexcept;
	Twine_ಗಂಟು &operator=(ಧ್ರುವ Twine_ಗಂಟು &other) = ಸ್ವಭಾವ;
	Twine_ಗಂಟು &operator=(Twine_ಗಂಟು &&other) noexcept;
	~Twine_ಗಂಟು() = ಸ್ವಭಾವ;

	ಗಣನಾ class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ prefix_ == DEAD; }
	constexpr bool is_leaf() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ prefix_ < DEAD; }

	constexpr Kind kind() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ text_.view(); }
	IdString prefix() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ IdString(prefix_); }
	uint32_t hash() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ hash_; }
	ಕೀಲಿ_2 ಕೀಲಿ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {prefix_, text()}; }

	bool operator==(ಧ್ರುವ Twine_ಗಂಟು &other) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ಧ್ರುವ ಕೀಲಿ_2 &other) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ prefix_ == other.prefix && text() == other.text; }

ಗುಪ್ತ:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(ಪರಿಮಾಣ(Twine_ಗಂಟು) == (ಪರಿಮಾಣ(ಶೂನ್ಯ *) == 8 ? 24 : 20));

ಸಂರಚನಾ StaticTwines {
	ಸ್ಥಿರ ಶೂನ್ಯ init();
	ಸ್ಥಿರ ಧ್ರುವ Twine_ಗಂಟು &ಗಂಟು(size_t idx);
	ಸ್ಥಿರ bool ready();

ಗುಪ್ತ:
	ಸ್ಥಿರ std::vector<Twine_ಗಂಟು> nodes_;
};

ಬಾಹ್ಯ int64_t twine_gc_ns;
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ twine_gc_ಎಣಿಕೆ;

ಸಂರಚನಾ TwinePool : HashConsPool<TwinePool, Twine_ಗಂಟು, IdString> {
	ಸ್ಥಿರ constexpr size_t STATIC_ಎಣಿಕೆ = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ಧ್ರುವ TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ಧ್ರುವ TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ಧ್ರುವ Twine_ಗಂಟು& operator[](IdString ref) ಧ್ರುವ;

	ಸ್ಥಿರ ಧ್ರುವ Twine_ಗಂಟು& static_ಗಂಟು(size_t idx);

	ಸ್ಥಿರ ಶೂನ್ಯ check_ready();
	ಸ್ಥಿರ uint32_t hash_ಗಂಟು(ಧ್ರುವ Twine_ಗಂಟು& t) { ಪ್ರತ್ಯಾಗಮ t.hash(); }

	template<typename F>
	ಸ್ಥಿರ ಶೂನ್ಯ for_each_child(ಧ್ರುವ Twine_ಗಂಟು& t, F&& f) {
		ಯದಿ (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	ಶೂನ್ಯ dump(IdString ref, std::ostream& os = std::cout) ಧ್ರುವ;

	// Publicity bit provides escaping
	std::string str(IdString ref) ಧ್ರುವ;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ಧ್ರುವ;

	size_t str_size(IdString ref) ಧ್ರುವ;
	bool begins_with(IdString ref, std::string_view prefix) ಧ್ರುವ;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ಧ್ರುವ;
	ಪೂರ್ಣಾಙ್ಕ compare_by_name(IdString a, IdString b) ಧ್ರುವ;
	IdString prefix_of(IdString ref) ಧ್ರುವ;

	IdString find(std::string_view name) ಧ್ರುವ;
	IdString find(TwineSpec t) ಧ್ರುವ;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ಧ್ರುವ TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ಧ್ರುವ TwinePool& src, IdString ref) ಧ್ರುವ;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ಧ್ರುವ;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ಧ್ರುವ;
	ಶೂನ್ಯ check_owned(IdString ref) ಧ್ರುವ;

ಗುಪ್ತ:
	bool owns(IdString ref) ಧ್ರುವ;
	IdString stamp(IdString ref) ಧ್ರುವ;
	ಶೂನ್ಯ append_str(IdString ref, std::string& out) ಧ್ರುವ;
	IdString auto_prefix(ಧ್ರುವ std::string *prefix);
	IdString add_inner(Twine_ಗಂಟು t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ಧ್ರುವ;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ಧ್ರುವ;
	IdString find_content(uint32_t prefix, std::string_view text) ಧ್ರುವ;
	ಸ್ಥಿರ size_t next_serial();
	dict<ಧ್ರುವ std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
