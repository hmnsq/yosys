#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#அந்தர்பாவ "kernel/hashcons.h"
#அந்தர்பாவ "kernel/yosys_common.h"

#அந்தர்பாவ <algorithm>
#அந்தர்பாவ <bit>
#அந்தர்பாவ <cstdint>
#அந்தர்பாவ <cstring>
#அந்தர்பாவ <deque>
#அந்தர்பாவ <limits>
#அந்தர்பாவ <span>
#அந்தர்பாவ <string>
#அந்தர்பாவ <string_view>
#அந்தர்பாவ <unordered_set>
#அந்தர்பாவ <variant>
#அந்தர்பாவ <vector>

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு TwineSpec;
கட்டமைப்பு TwinePool;
கட்டமைப்பு IdString;

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

கட்டமைப்பு NullIdString {
	constexpr operator IdString() மாறிலி;
	constexpr bool operator==(IdString ref) மாறிலி;
};

கட்டமைப்பு IdString {
தனிப்பட்ட:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#இல்லையெனில்
	using bits_t = uint32_t;
#endif
	bits_t மதிப்பு;

	நிலையான constexpr முழு TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	நிலையான constexpr முழு TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	நிலையான constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	நிலையான constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	நிலையான constexpr முழு TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	நிலையான constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	நிலையான constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

பொது:
	நிலையான constexpr NullIdString Null{};

	நிலையான constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	நிலையான constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() மாறிலி {
		திரும்பு மதிப்பு == TWINE_NULL_VAL ? மதிப்பு : (மதிப்பு & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() மாறிலி { திரும்பு மதிப்பு; }

	constexpr size_t order_சாவி() மாறிலி { திரும்பு std::rotl((bits_t) eq_சாவி(), 1); }

	constexpr size_t serial() மாறிலி {
		திரும்பு மதிப்பு == TWINE_NULL_VAL ? 0 : ((மதிப்பு & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) மாறிலி {
		திரும்பு மதிப்பு == TWINE_NULL_VAL ? *this
			: IdString((மதிப்பு & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : மதிப்பு(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : மதிப்பு((bits_t) val) {}

	constexpr size_t eq_சாவி() மாறிலி { திரும்பு மதிப்பு & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(மாறிலி IdString &rhs) மாறிலி { திரும்பு eq_சாவி() == rhs.eq_சாவி(); }
	constexpr bool operator!=(மாறிலி IdString &rhs) மாறிலி { திரும்பு eq_சாவி() != rhs.eq_சாவி(); }
	constexpr std::strong_ordering operator<=>(மாறிலி IdString &rhs) மாறிலி {
		திரும்பு order_சாவி() <=> rhs.order_சாவி();
	}

	template <typename... Args>
	constexpr bool in(மாறிலி Args&... args) மாறிலி {
		திரும்பு (in_one(args) || ...);
	}
	bool in(மாறிலி pool<IdString> &rhs) மாறிலி;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> கட்டமைப்பு compare_ptr_by_name {
		bool operator()(மாறிலி T *a, மாறிலி T *b) மாறிலி {
			திரும்பு (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() மாறிலி { திரும்பு மதிப்பு == TWINE_NULL_VAL; }
	constexpr வெறுமை clear() { மதிப்பு = TWINE_NULL_VAL; }
	constexpr bool isPublic() மாறிலி { திரும்பு மதிப்பு != TWINE_NULL_VAL && (மதிப்பு & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() மாறிலி {
		திரும்பு மதிப்பு == TWINE_NULL_VAL ? *this : IdString(மதிப்பு & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) மாறிலி {
		திரும்பு மதிப்பு == TWINE_NULL_VAL ? *this : IdString(pub ? (மதிப்பு | TWINE_PUBLIC_BIT) : (மதிப்பு & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() மாறிலி;
	நிலையான size_t handle_token_prefix(std::string_view token, bool &is_public);

தனிப்பட்ட:
	constexpr bool in_one(IdString rhs) மாறிலி { திரும்பு *this == rhs; }
	bool in_one(மாறிலி pool<IdString> &rhs) மாறிலி;
};

constexpr NullIdString::operator IdString() மாறிலி { திரும்பு IdString(); }
constexpr bool NullIdString::operator==(IdString ref) மாறிலி { திரும்பு ref.empty(); }

namespace hashlib {
	template<>
	கட்டமைப்பு hash_ops<IdString> {
		நிலையான உள்வரி bool cmp(IdString a, IdString b) { திரும்பு a == b; }
		[[nodiscard]] நிலையான உள்வரி Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_சாவி());
			திரும்பு h;
		}
		[[nodiscard]] நிலையான உள்வரி Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_சாவி());
			திரும்பு h;
		}
	};
}

உள்வரி bool IdString::in_one(மாறிலி pool<IdString> &rhs) மாறிலி { திரும்பு rhs.மொத்தம்(*this) != 0; }
உள்வரி bool IdString::in(மாறிலி pool<IdString> &rhs) மாறிலி { திரும்பு in_one(rhs); }


எண்ணிக்கை : short {
#define X(N) IDX_##N,
#அந்தர்பாவ "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

கட்டமைப்பு ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) நிலையான constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#அந்தர்பாவ "kernel/constids.inc"
#undef X

	நிலையான constexpr மாறிலி எழுத்து* static_names[] = {
#define X(N) #N,
#அந்தர்பாவ "kernel/constids.inc"
#undef X
	};

	நிலையான constexpr IdString lookup(std::string_view name)
	{
		முழு கீழ் = 0, உயர்வு = STATIC_TWINE_END;
		போது (உயர்வு - கீழ் >= 2) {
			முழு mid = (கீழ் + உயர்வு) / 2;
			எனில் (name < static_names[mid])
				உயர்வு = mid;
			இல்லையெனில்
				கீழ் = mid;
		}

		எனில் (name != static_names[கீழ்])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		திரும்பு IdString(கீழ்).tag(name[0] != '$');
	}

	நிலையான constexpr bool is_static(IdString ref) {
		திரும்பு ref.untag().raw() < STATIC_TWINE_END;
	}

	நிலையான constexpr IdString static_ref(size_t raw) {
		எனில் (!is_static(IdString(raw)))
			throw "not a static twine id";
		திரும்பு IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	நிலையான std::string str(IdString ref);
	நிலையான std::string unescaped_str(IdString ref);
};

template<size_t Raw> உள்வரி constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
கட்டமைப்பு TwineSpec {
	// deduplicates shared prefixes
	கட்டமைப்பு Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(மாறிலி Suffix&) மாறிலி = இயல்பு;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	கட்டமைப்பு AutoSuffix {
		மாறிலி std::string *prefix;
		std::string tail;
		auto operator<=>(மாறிலி AutoSuffix&) மாறிலி = இயல்பு;
	};

	// "leaf", regular deduplicated string
	கட்டமைப்பு Leaf {
		std::string s;
		auto operator<=>(மாறிலி Leaf&) மாறிலி = இயல்பு;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() மாறிலி;
};

கட்டமைப்பு SmallString {
	நிலையான constexpr uint32_t INLINE_CAP = 8;
	நிலையான constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = இயல்பு;
	explicit SmallString(std::string_view content);
	SmallString(மாறிலி SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(மாறிலி SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() மாறிலி { திரும்பு {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

தனிப்பட்ட:
	வெறுமை store(std::string_view content);
	வெறுமை release();

	ஒன்றிப்பு {
		எழுத்து inl_[INLINE_CAP];
		எழுத்து *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
கட்டமைப்பு Twine_முனை {
	நிலையான constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	நிலையான constexpr uint32_t DEAD = NO_PREFIX - 1;

	கட்டமைப்பு சாவி_2 {
		uint32_t prefix;
		std::string_view text;
	};

	நிலையான constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		க்கு (எழுத்து c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned எழுத்து>(c);
		திரும்பு hash;
	}

	Twine_முனை() = இயல்பு;
	explicit Twine_முனை(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_முனை(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_முனை(மாறிலி Twine_முனை &other) = இயல்பு;
	Twine_முனை(Twine_முனை &&other) noexcept;
	Twine_முனை &operator=(மாறிலி Twine_முனை &other) = இயல்பு;
	Twine_முனை &operator=(Twine_முனை &&other) noexcept;
	~Twine_முனை() = இயல்பு;

	எண்ணிக்கை class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() மாறிலி { திரும்பு prefix_ == DEAD; }
	constexpr bool is_leaf() மாறிலி { திரும்பு prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() மாறிலி { திரும்பு prefix_ < DEAD; }

	constexpr Kind kind() மாறிலி {
		திரும்பு prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() மாறிலி { திரும்பு text_.view(); }
	IdString prefix() மாறிலி { திரும்பு IdString(prefix_); }
	uint32_t hash() மாறிலி { திரும்பு hash_; }
	சாவி_2 சாவி() மாறிலி { திரும்பு {prefix_, text()}; }

	bool operator==(மாறிலி Twine_முனை &other) மாறிலி
		{ திரும்பு prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(மாறிலி சாவி_2 &other) மாறிலி
		{ திரும்பு prefix_ == other.prefix && text() == other.text; }

தனிப்பட்ட:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(அளவு(Twine_முனை) == (அளவு(வெறுமை *) == 8 ? 24 : 20));

கட்டமைப்பு StaticTwines {
	நிலையான வெறுமை init();
	நிலையான மாறிலி Twine_முனை &முனை(size_t idx);
	நிலையான bool ready();

தனிப்பட்ட:
	நிலையான std::vector<Twine_முனை> nodes_;
};

வெளி int64_t twine_gc_ns;
வெளி முழு twine_gc_மொத்தம்;

கட்டமைப்பு TwinePool : HashConsPool<TwinePool, Twine_முனை, IdString> {
	நிலையான constexpr size_t STATIC_மொத்தம் = STATIC_TWINE_END;

	TwinePool();
	TwinePool(மாறிலி TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(மாறிலி TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	மாறிலி Twine_முனை& operator[](IdString ref) மாறிலி;

	நிலையான மாறிலி Twine_முனை& static_முனை(size_t idx);

	நிலையான வெறுமை check_ready();
	நிலையான uint32_t hash_முனை(மாறிலி Twine_முனை& t) { திரும்பு t.hash(); }

	template<typename F>
	நிலையான வெறுமை for_each_child(மாறிலி Twine_முனை& t, F&& f) {
		எனில் (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	வெறுமை dump(IdString ref, std::ostream& os = std::cout) மாறிலி;

	// Publicity bit provides escaping
	std::string str(IdString ref) மாறிலி;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) மாறிலி;

	size_t str_size(IdString ref) மாறிலி;
	bool begins_with(IdString ref, std::string_view prefix) மாறிலி;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) மாறிலி;
	முழு compare_by_name(IdString a, IdString b) மாறிலி;
	IdString prefix_of(IdString ref) மாறிலி;

	IdString find(std::string_view name) மாறிலி;
	IdString find(TwineSpec t) மாறிலி;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(மாறிலி TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(மாறிலி TwinePool& src, IdString ref) மாறிலி;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) மாறிலி;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) மாறிலி;
	வெறுமை check_owned(IdString ref) மாறிலி;

தனிப்பட்ட:
	bool owns(IdString ref) மாறிலி;
	IdString stamp(IdString ref) மாறிலி;
	வெறுமை append_str(IdString ref, std::string& out) மாறிலி;
	IdString auto_prefix(மாறிலி std::string *prefix);
	IdString add_inner(Twine_முனை t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) மாறிலி;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) மாறிலி;
	IdString find_content(uint32_t prefix, std::string_view text) மாறிலி;
	நிலையான size_t next_serial();
	dict<மாறிலி std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
