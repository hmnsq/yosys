#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#અન્તર્ભાવ "kernel/hashcons.h"
#અન્તર્ભાવ "kernel/yosys_common.h"

#અન્તર્ભાવ <algorithm>
#અન્તર્ભાવ <bit>
#અન્તર્ભાવ <cstdint>
#અન્તર્ભાવ <cstring>
#અન્તર્ભાવ <deque>
#અન્તર્ભાવ <limits>
#અન્તર્ભાવ <span>
#અન્તર્ભાવ <string>
#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <unordered_set>
#અન્તર્ભાવ <variant>
#અન્તર્ભાવ <vector>

YOSYS_NAMESPACE_BEGIN

સંરચના TwineSpec;
સંરચના TwinePool;
સંરચના IdString;

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

સંરચના NullIdString {
	constexpr operator IdString() અચલ;
	constexpr bool operator==(IdString ref) અચલ;
};

સંરચના IdString {
ખાનગી:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#નહીંતર
	using bits_t = uint32_t;
#endif
	bits_t કિંમત;

	સ્થિર constexpr પૂર્ણાંક TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	સ્થિર constexpr પૂર્ણાંક TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	સ્થિર constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	સ્થિર constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	સ્થિર constexpr પૂર્ણાંક TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	સ્થિર constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	સ્થિર constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

જાહેર:
	સ્થિર constexpr NullIdString Null{};

	સ્થિર constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	સ્થિર constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() અચલ {
		પાછા કિંમત == TWINE_NULL_VAL ? કિંમત : (કિંમત & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() અચલ { પાછા કિંમત; }

	constexpr size_t order_ચાવી() અચલ { પાછા std::rotl((bits_t) eq_ચાવી(), 1); }

	constexpr size_t serial() અચલ {
		પાછા કિંમત == TWINE_NULL_VAL ? 0 : ((કિંમત & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) અચલ {
		પાછા કિંમત == TWINE_NULL_VAL ? *this
			: IdString((કિંમત & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : કિંમત(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : કિંમત((bits_t) val) {}

	constexpr size_t eq_ચાવી() અચલ { પાછા કિંમત & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(અચલ IdString &rhs) અચલ { પાછા eq_ચાવી() == rhs.eq_ચાવી(); }
	constexpr bool operator!=(અચલ IdString &rhs) અચલ { પાછા eq_ચાવી() != rhs.eq_ચાવી(); }
	constexpr std::strong_ordering operator<=>(અચલ IdString &rhs) અચલ {
		પાછા order_ચાવી() <=> rhs.order_ચાવી();
	}

	template <typename... Args>
	constexpr bool in(અચલ Args&... args) અચલ {
		પાછા (in_one(args) || ...);
	}
	bool in(અચલ pool<IdString> &rhs) અચલ;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> સંરચના compare_ptr_by_name {
		bool operator()(અચલ T *a, અચલ T *b) અચલ {
			પાછા (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() અચલ { પાછા કિંમત == TWINE_NULL_VAL; }
	constexpr શૂન્ય clear() { કિંમત = TWINE_NULL_VAL; }
	constexpr bool isPublic() અચલ { પાછા કિંમત != TWINE_NULL_VAL && (કિંમત & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() અચલ {
		પાછા કિંમત == TWINE_NULL_VAL ? *this : IdString(કિંમત & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) અચલ {
		પાછા કિંમત == TWINE_NULL_VAL ? *this : IdString(pub ? (કિંમત | TWINE_PUBLIC_BIT) : (કિંમત & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() અચલ;
	સ્થિર size_t handle_token_prefix(std::string_view token, bool &is_public);

ખાનગી:
	constexpr bool in_one(IdString rhs) અચલ { પાછા *this == rhs; }
	bool in_one(અચલ pool<IdString> &rhs) અચલ;
};

constexpr NullIdString::operator IdString() અચલ { પાછા IdString(); }
constexpr bool NullIdString::operator==(IdString ref) અચલ { પાછા ref.empty(); }

namespace hashlib {
	template<>
	સંરચના hash_ops<IdString> {
		સ્થિર અંતઃરેખ bool cmp(IdString a, IdString b) { પાછા a == b; }
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ચાવી());
			પાછા h;
		}
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ચાવી());
			પાછા h;
		}
	};
}

અંતઃરેખ bool IdString::in_one(અચલ pool<IdString> &rhs) અચલ { પાછા rhs.ગણતરી(*this) != 0; }
અંતઃરેખ bool IdString::in(અચલ pool<IdString> &rhs) અચલ { પાછા in_one(rhs); }


ગણના : short {
#define X(N) IDX_##N,
#અન્તર્ભાવ "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

સંરચના ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) સ્થિર constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#અન્તર્ભાવ "kernel/constids.inc"
#undef X

	સ્થિર constexpr અચલ અક્ષર* static_names[] = {
#define X(N) #N,
#અન્તર્ભાવ "kernel/constids.inc"
#undef X
	};

	સ્થિર constexpr IdString lookup(std::string_view name)
	{
		પૂર્ણાંક નીચલું = 0, ઉપલું = STATIC_TWINE_END;
		જ્યારે (ઉપલું - નીચલું >= 2) {
			પૂર્ણાંક mid = (નીચલું + ઉપલું) / 2;
			જો (name < static_names[mid])
				ઉપલું = mid;
			નહીંતર
				નીચલું = mid;
		}

		જો (name != static_names[નીચલું])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		પાછા IdString(નીચલું).tag(name[0] != '$');
	}

	સ્થિર constexpr bool is_static(IdString ref) {
		પાછા ref.untag().raw() < STATIC_TWINE_END;
	}

	સ્થિર constexpr IdString static_ref(size_t raw) {
		જો (!is_static(IdString(raw)))
			throw "not a static twine id";
		પાછા IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	સ્થિર std::string str(IdString ref);
	સ્થિર std::string unescaped_str(IdString ref);
};

template<size_t Raw> અંતઃરેખ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
સંરચના TwineSpec {
	// deduplicates shared prefixes
	સંરચના Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(અચલ Suffix&) અચલ = મૂળભૂત;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	સંરચના AutoSuffix {
		અચલ std::string *prefix;
		std::string tail;
		auto operator<=>(અચલ AutoSuffix&) અચલ = મૂળભૂત;
	};

	// "leaf", regular deduplicated string
	સંરચના Leaf {
		std::string s;
		auto operator<=>(અચલ Leaf&) અચલ = મૂળભૂત;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() અચલ;
};

સંરચના SmallString {
	સ્થિર constexpr uint32_t INLINE_CAP = 8;
	સ્થિર constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = મૂળભૂત;
	explicit SmallString(std::string_view content);
	SmallString(અચલ SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(અચલ SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() અચલ { પાછા {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ખાનગી:
	શૂન્ય store(std::string_view content);
	શૂન્ય release();

	સંઘ {
		અક્ષર inl_[INLINE_CAP];
		અક્ષર *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
સંરચના Twine_ગાંઠ {
	સ્થિર constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	સ્થિર constexpr uint32_t DEAD = NO_PREFIX - 1;

	સંરચના ચાવી_2 {
		uint32_t prefix;
		std::string_view text;
	};

	સ્થિર constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		માટે (અક્ષર c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned અક્ષર>(c);
		પાછા hash;
	}

	Twine_ગાંઠ() = મૂળભૂત;
	explicit Twine_ગાંઠ(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_ગાંઠ(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_ગાંઠ(અચલ Twine_ગાંઠ &other) = મૂળભૂત;
	Twine_ગાંઠ(Twine_ગાંઠ &&other) noexcept;
	Twine_ગાંઠ &operator=(અચલ Twine_ગાંઠ &other) = મૂળભૂત;
	Twine_ગાંઠ &operator=(Twine_ગાંઠ &&other) noexcept;
	~Twine_ગાંઠ() = મૂળભૂત;

	ગણના class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() અચલ { પાછા prefix_ == DEAD; }
	constexpr bool is_leaf() અચલ { પાછા prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() અચલ { પાછા prefix_ < DEAD; }

	constexpr Kind kind() અચલ {
		પાછા prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() અચલ { પાછા text_.view(); }
	IdString prefix() અચલ { પાછા IdString(prefix_); }
	uint32_t hash() અચલ { પાછા hash_; }
	ચાવી_2 ચાવી() અચલ { પાછા {prefix_, text()}; }

	bool operator==(અચલ Twine_ગાંઠ &other) અચલ
		{ પાછા prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(અચલ ચાવી_2 &other) અચલ
		{ પાછા prefix_ == other.prefix && text() == other.text; }

ખાનગી:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(કદ(Twine_ગાંઠ) == (કદ(શૂન્ય *) == 8 ? 24 : 20));

સંરચના StaticTwines {
	સ્થિર શૂન્ય init();
	સ્થિર અચલ Twine_ગાંઠ &ગાંઠ(size_t idx);
	સ્થિર bool ready();

ખાનગી:
	સ્થિર std::vector<Twine_ગાંઠ> nodes_;
};

બાહ્ય int64_t twine_gc_ns;
બાહ્ય પૂર્ણાંક twine_gc_ગણતરી;

સંરચના TwinePool : HashConsPool<TwinePool, Twine_ગાંઠ, IdString> {
	સ્થિર constexpr size_t STATIC_ગણતરી = STATIC_TWINE_END;

	TwinePool();
	TwinePool(અચલ TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(અચલ TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	અચલ Twine_ગાંઠ& operator[](IdString ref) અચલ;

	સ્થિર અચલ Twine_ગાંઠ& static_ગાંઠ(size_t idx);

	સ્થિર શૂન્ય check_ready();
	સ્થિર uint32_t hash_ગાંઠ(અચલ Twine_ગાંઠ& t) { પાછા t.hash(); }

	template<typename F>
	સ્થિર શૂન્ય for_each_child(અચલ Twine_ગાંઠ& t, F&& f) {
		જો (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	શૂન્ય dump(IdString ref, std::ostream& os = std::cout) અચલ;

	// Publicity bit provides escaping
	std::string str(IdString ref) અચલ;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) અચલ;

	size_t str_size(IdString ref) અચલ;
	bool begins_with(IdString ref, std::string_view prefix) અચલ;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) અચલ;
	પૂર્ણાંક compare_by_name(IdString a, IdString b) અચલ;
	IdString prefix_of(IdString ref) અચલ;

	IdString find(std::string_view name) અચલ;
	IdString find(TwineSpec t) અચલ;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(અચલ TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(અચલ TwinePool& src, IdString ref) અચલ;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) અચલ;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) અચલ;
	શૂન્ય check_owned(IdString ref) અચલ;

ખાનગી:
	bool owns(IdString ref) અચલ;
	IdString stamp(IdString ref) અચલ;
	શૂન્ય append_str(IdString ref, std::string& out) અચલ;
	IdString auto_prefix(અચલ std::string *prefix);
	IdString add_inner(Twine_ગાંઠ t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) અચલ;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) અચલ;
	IdString find_content(uint32_t prefix, std::string_view text) અચલ;
	સ્થિર size_t next_serial();
	dict<અચલ std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
