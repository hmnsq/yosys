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

সংরচনা TwineSpec;
সংরচনা TwinePool;
সংরচনা IdString;

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

সংরচনা NullIdString {
	constexpr operator IdString() ধ্রুব;
	constexpr bool operator==(IdString ref) ধ্রুব;
};

সংরচনা IdString {
গুপ্ত:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#অন্যথা
	using bits_t = uint32_t;
#endif
	bits_t মান;

	স্থির constexpr পূর্ণাঙ্ক TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	স্থির constexpr পূর্ণাঙ্ক TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	স্থির constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	স্থির constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	স্থির constexpr পূর্ণাঙ্ক TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	স্থির constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	স্থির constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

সার্বজনিক:
	স্থির constexpr NullIdString Null{};

	স্থির constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	স্থির constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ধ্রুব {
		প্রত্যাগম মান == TWINE_NULL_VAL ? মান : (মান & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ধ্রুব { প্রত্যাগম মান; }

	constexpr size_t order_চাবি() ধ্রুব { প্রত্যাগম std::rotl((bits_t) eq_চাবি(), 1); }

	constexpr size_t serial() ধ্রুব {
		প্রত্যাগম মান == TWINE_NULL_VAL ? 0 : ((মান & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ধ্রুব {
		প্রত্যাগম মান == TWINE_NULL_VAL ? *this
			: IdString((মান & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : মান(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : মান((bits_t) val) {}

	constexpr size_t eq_চাবি() ধ্রুব { প্রত্যাগম মান & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ধ্রুব IdString &rhs) ধ্রুব { প্রত্যাগম eq_চাবি() == rhs.eq_চাবি(); }
	constexpr bool operator!=(ধ্রুব IdString &rhs) ধ্রুব { প্রত্যাগম eq_চাবি() != rhs.eq_চাবি(); }
	constexpr std::strong_ordering operator<=>(ধ্রুব IdString &rhs) ধ্রুব {
		প্রত্যাগম order_চাবি() <=> rhs.order_চাবি();
	}

	template <typename... Args>
	constexpr bool in(ধ্রুব Args&... args) ধ্রুব {
		প্রত্যাগম (in_one(args) || ...);
	}
	bool in(ধ্রুব pool<IdString> &rhs) ধ্রুব;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> সংরচনা compare_ptr_by_name {
		bool operator()(ধ্রুব T *a, ধ্রুব T *b) ধ্রুব {
			প্রত্যাগম (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ধ্রুব { প্রত্যাগম মান == TWINE_NULL_VAL; }
	constexpr শূন্য clear() { মান = TWINE_NULL_VAL; }
	constexpr bool isPublic() ধ্রুব { প্রত্যাগম মান != TWINE_NULL_VAL && (মান & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ধ্রুব {
		প্রত্যাগম মান == TWINE_NULL_VAL ? *this : IdString(মান & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ধ্রুব {
		প্রত্যাগম মান == TWINE_NULL_VAL ? *this : IdString(pub ? (মান | TWINE_PUBLIC_BIT) : (মান & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ধ্রুব;
	স্থির size_t handle_token_prefix(std::string_view token, bool &is_public);

গুপ্ত:
	constexpr bool in_one(IdString rhs) ধ্রুব { প্রত্যাগম *this == rhs; }
	bool in_one(ধ্রুব pool<IdString> &rhs) ধ্রুব;
};

constexpr NullIdString::operator IdString() ধ্রুব { প্রত্যাগম IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ধ্রুব { প্রত্যাগম ref.empty(); }

namespace hashlib {
	template<>
	সংরচনা hash_ops<IdString> {
		স্থির অন্তঃস্থ bool cmp(IdString a, IdString b) { প্রত্যাগম a == b; }
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_চাবি());
			প্রত্যাগম h;
		}
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_চাবি());
			প্রত্যাগম h;
		}
	};
}

অন্তঃস্থ bool IdString::in_one(ধ্রুব pool<IdString> &rhs) ধ্রুব { প্রত্যাগম rhs.গুনতি(*this) != 0; }
অন্তঃস্থ bool IdString::in(ধ্রুব pool<IdString> &rhs) ধ্রুব { প্রত্যাগম in_one(rhs); }


গণনা : short {
#define X(N) IDX_##N,
#অন্তর্ভাব "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

সংরচনা ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) স্থির constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#অন্তর্ভাব "kernel/constids.inc"
#undef X

	স্থির constexpr ধ্রুব বর্ণ* static_names[] = {
#define X(N) #N,
#অন্তর্ভাব "kernel/constids.inc"
#undef X
	};

	স্থির constexpr IdString lookup(std::string_view name)
	{
		পূর্ণাঙ্ক নিচের = 0, উপরের = STATIC_TWINE_END;
		যাবত্ (উপরের - নিচের >= 2) {
			পূর্ণাঙ্ক mid = (নিচের + উপরের) / 2;
			যদি (name < static_names[mid])
				উপরের = mid;
			অন্যথা
				নিচের = mid;
		}

		যদি (name != static_names[নিচের])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		প্রত্যাগম IdString(নিচের).tag(name[0] != '$');
	}

	স্থির constexpr bool is_static(IdString ref) {
		প্রত্যাগম ref.untag().raw() < STATIC_TWINE_END;
	}

	স্থির constexpr IdString static_ref(size_t raw) {
		যদি (!is_static(IdString(raw)))
			throw "not a static twine id";
		প্রত্যাগম IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	স্থির std::string str(IdString ref);
	স্থির std::string unescaped_str(IdString ref);
};

template<size_t Raw> অন্তঃস্থ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
সংরচনা TwineSpec {
	// deduplicates shared prefixes
	সংরচনা Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ধ্রুব Suffix&) ধ্রুব = স্বভাব;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	সংরচনা AutoSuffix {
		ধ্রুব std::string *prefix;
		std::string tail;
		auto operator<=>(ধ্রুব AutoSuffix&) ধ্রুব = স্বভাব;
	};

	// "leaf", regular deduplicated string
	সংরচনা Leaf {
		std::string s;
		auto operator<=>(ধ্রুব Leaf&) ধ্রুব = স্বভাব;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ধ্রুব;
};

সংরচনা SmallString {
	স্থির constexpr uint32_t INLINE_CAP = 8;
	স্থির constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = স্বভাব;
	explicit SmallString(std::string_view content);
	SmallString(ধ্রুব SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ধ্রুব SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ধ্রুব { প্রত্যাগম {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

গুপ্ত:
	শূন্য store(std::string_view content);
	শূন্য release();

	সঙ্ঘ {
		বর্ণ inl_[INLINE_CAP];
		বর্ণ *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
সংরচনা Twine_গ্রন্থি {
	স্থির constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	স্থির constexpr uint32_t DEAD = NO_PREFIX - 1;

	সংরচনা চাবি_2 {
		uint32_t prefix;
		std::string_view text;
	};

	স্থির constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		প্রতি (বর্ণ c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned বর্ণ>(c);
		প্রত্যাগম hash;
	}

	Twine_গ্রন্থি() = স্বভাব;
	explicit Twine_গ্রন্থি(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_গ্রন্থি(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_গ্রন্থি(ধ্রুব Twine_গ্রন্থি &other) = স্বভাব;
	Twine_গ্রন্থি(Twine_গ্রন্থি &&other) noexcept;
	Twine_গ্রন্থি &operator=(ধ্রুব Twine_গ্রন্থি &other) = স্বভাব;
	Twine_গ্রন্থি &operator=(Twine_গ্রন্থি &&other) noexcept;
	~Twine_গ্রন্থি() = স্বভাব;

	গণনা class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ধ্রুব { প্রত্যাগম prefix_ == DEAD; }
	constexpr bool is_leaf() ধ্রুব { প্রত্যাগম prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ধ্রুব { প্রত্যাগম prefix_ < DEAD; }

	constexpr Kind kind() ধ্রুব {
		প্রত্যাগম prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ধ্রুব { প্রত্যাগম text_.view(); }
	IdString prefix() ধ্রুব { প্রত্যাগম IdString(prefix_); }
	uint32_t hash() ধ্রুব { প্রত্যাগম hash_; }
	চাবি_2 চাবি() ধ্রুব { প্রত্যাগম {prefix_, text()}; }

	bool operator==(ধ্রুব Twine_গ্রন্থি &other) ধ্রুব
		{ প্রত্যাগম prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ধ্রুব চাবি_2 &other) ধ্রুব
		{ প্রত্যাগম prefix_ == other.prefix && text() == other.text; }

গুপ্ত:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(পরিমাণ(Twine_গ্রন্থি) == (পরিমাণ(শূন্য *) == 8 ? 24 : 20));

সংরচনা StaticTwines {
	স্থির শূন্য init();
	স্থির ধ্রুব Twine_গ্রন্থি &গ্রন্থি(size_t idx);
	স্থির bool ready();

গুপ্ত:
	স্থির std::vector<Twine_গ্রন্থি> nodes_;
};

বাহ্য int64_t twine_gc_ns;
বাহ্য পূর্ণাঙ্ক twine_gc_গুনতি;

সংরচনা TwinePool : HashConsPool<TwinePool, Twine_গ্রন্থি, IdString> {
	স্থির constexpr size_t STATIC_গুনতি = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ধ্রুব TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ধ্রুব TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ধ্রুব Twine_গ্রন্থি& operator[](IdString ref) ধ্রুব;

	স্থির ধ্রুব Twine_গ্রন্থি& static_গ্রন্থি(size_t idx);

	স্থির শূন্য check_ready();
	স্থির uint32_t hash_গ্রন্থি(ধ্রুব Twine_গ্রন্থি& t) { প্রত্যাগম t.hash(); }

	template<typename F>
	স্থির শূন্য for_each_child(ধ্রুব Twine_গ্রন্থি& t, F&& f) {
		যদি (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	শূন্য dump(IdString ref, std::ostream& os = std::cout) ধ্রুব;

	// Publicity bit provides escaping
	std::string str(IdString ref) ধ্রুব;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ধ্রুব;

	size_t str_size(IdString ref) ধ্রুব;
	bool begins_with(IdString ref, std::string_view prefix) ধ্রুব;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ধ্রুব;
	পূর্ণাঙ্ক compare_by_name(IdString a, IdString b) ধ্রুব;
	IdString prefix_of(IdString ref) ধ্রুব;

	IdString find(std::string_view name) ধ্রুব;
	IdString find(TwineSpec t) ধ্রুব;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ধ্রুব TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ধ্রুব TwinePool& src, IdString ref) ধ্রুব;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ধ্রুব;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ধ্রুব;
	শূন্য check_owned(IdString ref) ধ্রুব;

গুপ্ত:
	bool owns(IdString ref) ধ্রুব;
	IdString stamp(IdString ref) ধ্রুব;
	শূন্য append_str(IdString ref, std::string& out) ধ্রুব;
	IdString auto_prefix(ধ্রুব std::string *prefix);
	IdString add_inner(Twine_গ্রন্থি t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ধ্রুব;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ধ্রুব;
	IdString find_content(uint32_t prefix, std::string_view text) ধ্রুব;
	স্থির size_t next_serial();
	dict<ধ্রুব std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
