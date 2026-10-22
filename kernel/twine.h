#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#അന്തര്ഭാവ "kernel/hashcons.h"
#അന്തര്ഭാവ "kernel/yosys_common.h"

#അന്തര്ഭാവ <algorithm>
#അന്തര്ഭാവ <bit>
#അന്തര്ഭാവ <cstdint>
#അന്തര്ഭാവ <cstring>
#അന്തര്ഭാവ <deque>
#അന്തര്ഭാവ <limits>
#അന്തര്ഭാവ <span>
#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <unordered_set>
#അന്തര്ഭാവ <variant>
#അന്തര്ഭാവ <vector>

YOSYS_NAMESPACE_BEGIN

ഘടന TwineSpec;
ഘടന TwinePool;
ഘടന IdString;

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

ഘടന NullIdString {
	constexpr operator IdString() സ്ഥിരം;
	constexpr bool operator==(IdString ref) സ്ഥിരം;
};

ഘടന IdString {
സ്വകാര്യ:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#അല്ലെങ്കിൽ
	using bits_t = uint32_t;
#endif
	bits_t മൂല്യം;

	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	സ്ഥിതം constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	സ്ഥിതം constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	സ്ഥിതം constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	സ്ഥിതം constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

പൊതു:
	സ്ഥിതം constexpr NullIdString Null{};

	സ്ഥിതം constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	സ്ഥിതം constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() സ്ഥിരം {
		തിരികെ മൂല്യം == TWINE_NULL_VAL ? മൂല്യം : (മൂല്യം & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() സ്ഥിരം { തിരികെ മൂല്യം; }

	constexpr size_t order_താക്കോൽ() സ്ഥിരം { തിരികെ std::rotl((bits_t) eq_താക്കോൽ(), 1); }

	constexpr size_t serial() സ്ഥിരം {
		തിരികെ മൂല്യം == TWINE_NULL_VAL ? 0 : ((മൂല്യം & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) സ്ഥിരം {
		തിരികെ മൂല്യം == TWINE_NULL_VAL ? *this
			: IdString((മൂല്യം & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : മൂല്യം(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : മൂല്യം((bits_t) val) {}

	constexpr size_t eq_താക്കോൽ() സ്ഥിരം { തിരികെ മൂല്യം & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(സ്ഥിരം IdString &rhs) സ്ഥിരം { തിരികെ eq_താക്കോൽ() == rhs.eq_താക്കോൽ(); }
	constexpr bool operator!=(സ്ഥിരം IdString &rhs) സ്ഥിരം { തിരികെ eq_താക്കോൽ() != rhs.eq_താക്കോൽ(); }
	constexpr std::strong_ordering operator<=>(സ്ഥിരം IdString &rhs) സ്ഥിരം {
		തിരികെ order_താക്കോൽ() <=> rhs.order_താക്കോൽ();
	}

	template <typename... Args>
	constexpr bool in(സ്ഥിരം Args&... args) സ്ഥിരം {
		തിരികെ (in_one(args) || ...);
	}
	bool in(സ്ഥിരം pool<IdString> &rhs) സ്ഥിരം;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> ഘടന compare_ptr_by_name {
		bool operator()(സ്ഥിരം T *a, സ്ഥിരം T *b) സ്ഥിരം {
			തിരികെ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() സ്ഥിരം { തിരികെ മൂല്യം == TWINE_NULL_VAL; }
	constexpr ശൂന്യം clear() { മൂല്യം = TWINE_NULL_VAL; }
	constexpr bool isPublic() സ്ഥിരം { തിരികെ മൂല്യം != TWINE_NULL_VAL && (മൂല്യം & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() സ്ഥിരം {
		തിരികെ മൂല്യം == TWINE_NULL_VAL ? *this : IdString(മൂല്യം & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) സ്ഥിരം {
		തിരികെ മൂല്യം == TWINE_NULL_VAL ? *this : IdString(pub ? (മൂല്യം | TWINE_PUBLIC_BIT) : (മൂല്യം & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() സ്ഥിരം;
	സ്ഥിതം size_t handle_token_prefix(std::string_view token, bool &is_public);

സ്വകാര്യ:
	constexpr bool in_one(IdString rhs) സ്ഥിരം { തിരികെ *this == rhs; }
	bool in_one(സ്ഥിരം pool<IdString> &rhs) സ്ഥിരം;
};

constexpr NullIdString::operator IdString() സ്ഥിരം { തിരികെ IdString(); }
constexpr bool NullIdString::operator==(IdString ref) സ്ഥിരം { തിരികെ ref.empty(); }

namespace hashlib {
	template<>
	ഘടന hash_ops<IdString> {
		സ്ഥിതം വരിയിൽ bool cmp(IdString a, IdString b) { തിരികെ a == b; }
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_താക്കോൽ());
			തിരികെ h;
		}
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_താക്കോൽ());
			തിരികെ h;
		}
	};
}

വരിയിൽ bool IdString::in_one(സ്ഥിരം pool<IdString> &rhs) സ്ഥിരം { തിരികെ rhs.എണ്ണം(*this) != 0; }
വരിയിൽ bool IdString::in(സ്ഥിരം pool<IdString> &rhs) സ്ഥിരം { തിരികെ in_one(rhs); }


എണ്ണൽ : short {
#define X(N) IDX_##N,
#അന്തര്ഭാവ "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

ഘടന ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) സ്ഥിതം constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#അന്തര്ഭാവ "kernel/constids.inc"
#undef X

	സ്ഥിതം constexpr സ്ഥിരം അക്ഷരം* static_names[] = {
#define X(N) #N,
#അന്തര്ഭാവ "kernel/constids.inc"
#undef X
	};

	സ്ഥിതം constexpr IdString lookup(std::string_view name)
	{
		പൂർണ്ണസംഖ്യ താഴെയുള്ള = 0, മുകളിലെ = STATIC_TWINE_END;
		ആകുമ്പോൾ (മുകളിലെ - താഴെയുള്ള >= 2) {
			പൂർണ്ണസംഖ്യ mid = (താഴെയുള്ള + മുകളിലെ) / 2;
			എങ്കിൽ (name < static_names[mid])
				മുകളിലെ = mid;
			അല്ലെങ്കിൽ
				താഴെയുള്ള = mid;
		}

		എങ്കിൽ (name != static_names[താഴെയുള്ള])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		തിരികെ IdString(താഴെയുള്ള).tag(name[0] != '$');
	}

	സ്ഥിതം constexpr bool is_static(IdString ref) {
		തിരികെ ref.untag().raw() < STATIC_TWINE_END;
	}

	സ്ഥിതം constexpr IdString static_ref(size_t raw) {
		എങ്കിൽ (!is_static(IdString(raw)))
			throw "not a static twine id";
		തിരികെ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	സ്ഥിതം std::string str(IdString ref);
	സ്ഥിതം std::string unescaped_str(IdString ref);
};

template<size_t Raw> വരിയിൽ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ഘടന TwineSpec {
	// deduplicates shared prefixes
	ഘടന Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(സ്ഥിരം Suffix&) സ്ഥിരം = സ്ഥിരസ്ഥിതി;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ഘടന AutoSuffix {
		സ്ഥിരം std::string *prefix;
		std::string tail;
		auto operator<=>(സ്ഥിരം AutoSuffix&) സ്ഥിരം = സ്ഥിരസ്ഥിതി;
	};

	// "leaf", regular deduplicated string
	ഘടന Leaf {
		std::string s;
		auto operator<=>(സ്ഥിരം Leaf&) സ്ഥിരം = സ്ഥിരസ്ഥിതി;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() സ്ഥിരം;
};

ഘടന SmallString {
	സ്ഥിതം constexpr uint32_t INLINE_CAP = 8;
	സ്ഥിതം constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = സ്ഥിരസ്ഥിതി;
	explicit SmallString(std::string_view content);
	SmallString(സ്ഥിരം SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(സ്ഥിരം SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() സ്ഥിരം { തിരികെ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

സ്വകാര്യ:
	ശൂന്യം store(std::string_view content);
	ശൂന്യം release();

	സംഘം {
		അക്ഷരം inl_[INLINE_CAP];
		അക്ഷരം *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ഘടന Twine_കണ്ണി {
	സ്ഥിതം constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	സ്ഥിതം constexpr uint32_t DEAD = NO_PREFIX - 1;

	ഘടന താക്കോൽ_2 {
		uint32_t prefix;
		std::string_view text;
	};

	സ്ഥിതം constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		വേണ്ടി (അക്ഷരം c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned അക്ഷരം>(c);
		തിരികെ hash;
	}

	Twine_കണ്ണി() = സ്ഥിരസ്ഥിതി;
	explicit Twine_കണ്ണി(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_കണ്ണി(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_കണ്ണി(സ്ഥിരം Twine_കണ്ണി &other) = സ്ഥിരസ്ഥിതി;
	Twine_കണ്ണി(Twine_കണ്ണി &&other) noexcept;
	Twine_കണ്ണി &operator=(സ്ഥിരം Twine_കണ്ണി &other) = സ്ഥിരസ്ഥിതി;
	Twine_കണ്ണി &operator=(Twine_കണ്ണി &&other) noexcept;
	~Twine_കണ്ണി() = സ്ഥിരസ്ഥിതി;

	എണ്ണൽ class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() സ്ഥിരം { തിരികെ prefix_ == DEAD; }
	constexpr bool is_leaf() സ്ഥിരം { തിരികെ prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() സ്ഥിരം { തിരികെ prefix_ < DEAD; }

	constexpr Kind kind() സ്ഥിരം {
		തിരികെ prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() സ്ഥിരം { തിരികെ text_.view(); }
	IdString prefix() സ്ഥിരം { തിരികെ IdString(prefix_); }
	uint32_t hash() സ്ഥിരം { തിരികെ hash_; }
	താക്കോൽ_2 താക്കോൽ() സ്ഥിരം { തിരികെ {prefix_, text()}; }

	bool operator==(സ്ഥിരം Twine_കണ്ണി &other) സ്ഥിരം
		{ തിരികെ prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(സ്ഥിരം താക്കോൽ_2 &other) സ്ഥിരം
		{ തിരികെ prefix_ == other.prefix && text() == other.text; }

സ്വകാര്യ:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(വലുപ്പം(Twine_കണ്ണി) == (വലുപ്പം(ശൂന്യം *) == 8 ? 24 : 20));

ഘടന StaticTwines {
	സ്ഥിതം ശൂന്യം init();
	സ്ഥിതം സ്ഥിരം Twine_കണ്ണി &കണ്ണി(size_t idx);
	സ്ഥിതം bool ready();

സ്വകാര്യ:
	സ്ഥിതം std::vector<Twine_കണ്ണി> nodes_;
};

ബാഹ്യ int64_t twine_gc_ns;
ബാഹ്യ പൂർണ്ണസംഖ്യ twine_gc_എണ്ണം;

ഘടന TwinePool : HashConsPool<TwinePool, Twine_കണ്ണി, IdString> {
	സ്ഥിതം constexpr size_t STATIC_എണ്ണം = STATIC_TWINE_END;

	TwinePool();
	TwinePool(സ്ഥിരം TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(സ്ഥിരം TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	സ്ഥിരം Twine_കണ്ണി& operator[](IdString ref) സ്ഥിരം;

	സ്ഥിതം സ്ഥിരം Twine_കണ്ണി& static_കണ്ണി(size_t idx);

	സ്ഥിതം ശൂന്യം check_ready();
	സ്ഥിതം uint32_t hash_കണ്ണി(സ്ഥിരം Twine_കണ്ണി& t) { തിരികെ t.hash(); }

	template<typename F>
	സ്ഥിതം ശൂന്യം for_each_child(സ്ഥിരം Twine_കണ്ണി& t, F&& f) {
		എങ്കിൽ (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	ശൂന്യം dump(IdString ref, std::ostream& os = std::cout) സ്ഥിരം;

	// Publicity bit provides escaping
	std::string str(IdString ref) സ്ഥിരം;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) സ്ഥിരം;

	size_t str_size(IdString ref) സ്ഥിരം;
	bool begins_with(IdString ref, std::string_view prefix) സ്ഥിരം;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) സ്ഥിരം;
	പൂർണ്ണസംഖ്യ compare_by_name(IdString a, IdString b) സ്ഥിരം;
	IdString prefix_of(IdString ref) സ്ഥിരം;

	IdString find(std::string_view name) സ്ഥിരം;
	IdString find(TwineSpec t) സ്ഥിരം;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(സ്ഥിരം TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(സ്ഥിരം TwinePool& src, IdString ref) സ്ഥിരം;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) സ്ഥിരം;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) സ്ഥിരം;
	ശൂന്യം check_owned(IdString ref) സ്ഥിരം;

സ്വകാര്യ:
	bool owns(IdString ref) സ്ഥിരം;
	IdString stamp(IdString ref) സ്ഥിരം;
	ശൂന്യം append_str(IdString ref, std::string& out) സ്ഥിരം;
	IdString auto_prefix(സ്ഥിരം std::string *prefix);
	IdString add_inner(Twine_കണ്ണി t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) സ്ഥിരം;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) സ്ഥിരം;
	IdString find_content(uint32_t prefix, std::string_view text) സ്ഥിരം;
	സ്ഥിതം size_t next_serial();
	dict<സ്ഥിരം std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
