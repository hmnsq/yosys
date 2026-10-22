#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#ଅନ୍ତର୍ଭାଵ "kernel/hashcons.h"
#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <bit>
#ଅନ୍ତର୍ଭାଵ <cstdint>
#ଅନ୍ତର୍ଭାଵ <cstring>
#ଅନ୍ତର୍ଭାଵ <deque>
#ଅନ୍ତର୍ଭାଵ <limits>
#ଅନ୍ତର୍ଭାଵ <span>
#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <unordered_set>
#ଅନ୍ତର୍ଭାଵ <variant>
#ଅନ୍ତର୍ଭାଵ <vector>

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା TwineSpec;
ସଂରଚନା TwinePool;
ସଂରଚନା IdString;

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

ସଂରଚନା NullIdString {
	constexpr operator IdString() ଅଚଳ;
	constexpr bool operator==(IdString ref) ଅଚଳ;
};

ସଂରଚନା IdString {
ବ୍ୟକ୍ତିଗତ:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#ଅନ୍ୟଥା
	using bits_t = uint32_t;
#endif
	bits_t ମୂଲ୍ୟ;

	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	ସ୍ଥିର constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	ସ୍ଥିର constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	ସ୍ଥିର constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	ସ୍ଥିର constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

ସର୍ବସାଧାରଣ:
	ସ୍ଥିର constexpr NullIdString Null{};

	ସ୍ଥିର constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	ସ୍ଥିର constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ଅଚଳ {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL ? ମୂଲ୍ୟ : (ମୂଲ୍ୟ & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ; }

	constexpr size_t order_ଚାବି() ଅଚଳ { ଫେରନ୍ତୁ std::rotl((bits_t) eq_ଚାବି(), 1); }

	constexpr size_t serial() ଅଚଳ {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL ? 0 : ((ମୂଲ୍ୟ & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ଅଚଳ {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL ? *this
			: IdString((ମୂଲ୍ୟ & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : ମୂଲ୍ୟ(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : ମୂଲ୍ୟ((bits_t) val) {}

	constexpr size_t eq_ଚାବି() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ଅଚଳ IdString &rhs) ଅଚଳ { ଫେରନ୍ତୁ eq_ଚାବି() == rhs.eq_ଚାବି(); }
	constexpr bool operator!=(ଅଚଳ IdString &rhs) ଅଚଳ { ଫେରନ୍ତୁ eq_ଚାବି() != rhs.eq_ଚାବି(); }
	constexpr std::strong_ordering operator<=>(ଅଚଳ IdString &rhs) ଅଚଳ {
		ଫେରନ୍ତୁ order_ଚାବି() <=> rhs.order_ଚାବି();
	}

	template <typename... Args>
	constexpr bool in(ଅଚଳ Args&... args) ଅଚଳ {
		ଫେରନ୍ତୁ (in_one(args) || ...);
	}
	bool in(ଅଚଳ pool<IdString> &rhs) ଅଚଳ;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> ସଂରଚନା compare_ptr_by_name {
		bool operator()(ଅଚଳ T *a, ଅଚଳ T *b) ଅଚଳ {
			ଫେରନ୍ତୁ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL; }
	constexpr ଶୂନ୍ୟ clear() { ମୂଲ୍ୟ = TWINE_NULL_VAL; }
	constexpr bool isPublic() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ != TWINE_NULL_VAL && (ମୂଲ୍ୟ & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ଅଚଳ {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL ? *this : IdString(ମୂଲ୍ୟ & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ଅଚଳ {
		ଫେରନ୍ତୁ ମୂଲ୍ୟ == TWINE_NULL_VAL ? *this : IdString(pub ? (ମୂଲ୍ୟ | TWINE_PUBLIC_BIT) : (ମୂଲ୍ୟ & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ଅଚଳ;
	ସ୍ଥିର size_t handle_token_prefix(std::string_view token, bool &is_public);

ବ୍ୟକ୍ତିଗତ:
	constexpr bool in_one(IdString rhs) ଅଚଳ { ଫେରନ୍ତୁ *this == rhs; }
	bool in_one(ଅଚଳ pool<IdString> &rhs) ଅଚଳ;
};

constexpr NullIdString::operator IdString() ଅଚଳ { ଫେରନ୍ତୁ IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ଅଚଳ { ଫେରନ୍ତୁ ref.empty(); }

namespace hashlib {
	template<>
	ସଂରଚନା hash_ops<IdString> {
		ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(IdString a, IdString b) { ଫେରନ୍ତୁ a == b; }
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ଚାବି());
			ଫେରନ୍ତୁ h;
		}
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ଚାବି());
			ଫେରନ୍ତୁ h;
		}
	};
}

ଧାଡ଼ିଭିତରେ bool IdString::in_one(ଅଚଳ pool<IdString> &rhs) ଅଚଳ { ଫେରନ୍ତୁ rhs.ଗଣନା_2(*this) != 0; }
ଧାଡ଼ିଭିତରେ bool IdString::in(ଅଚଳ pool<IdString> &rhs) ଅଚଳ { ଫେରନ୍ତୁ in_one(rhs); }


ଗଣନା : short {
#define X(N) IDX_##N,
#ଅନ୍ତର୍ଭାଵ "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

ସଂରଚନା ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) ସ୍ଥିର constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#ଅନ୍ତର୍ଭାଵ "kernel/constids.inc"
#undef X

	ସ୍ଥିର constexpr ଅଚଳ ଅକ୍ଷର* static_names[] = {
#define X(N) #N,
#ଅନ୍ତର୍ଭାଵ "kernel/constids.inc"
#undef X
	};

	ସ୍ଥିର constexpr IdString lookup(std::string_view name)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ = 0, ଉପର = STATIC_TWINE_END;
		ଯେତେବେଳେ (ଉପର - ତଳ >= 2) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା mid = (ତଳ + ଉପର) / 2;
			ଯଦି (name < static_names[mid])
				ଉପର = mid;
			ଅନ୍ୟଥା
				ତଳ = mid;
		}

		ଯଦି (name != static_names[ତଳ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		ଫେରନ୍ତୁ IdString(ତଳ).tag(name[0] != '$');
	}

	ସ୍ଥିର constexpr bool is_static(IdString ref) {
		ଫେରନ୍ତୁ ref.untag().raw() < STATIC_TWINE_END;
	}

	ସ୍ଥିର constexpr IdString static_ref(size_t raw) {
		ଯଦି (!is_static(IdString(raw)))
			throw "not a static twine id";
		ଫେରନ୍ତୁ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ସ୍ଥିର std::string str(IdString ref);
	ସ୍ଥିର std::string unescaped_str(IdString ref);
};

template<size_t Raw> ଧାଡ଼ିଭିତରେ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ସଂରଚନା TwineSpec {
	// deduplicates shared prefixes
	ସଂରଚନା Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ଅଚଳ Suffix&) ଅଚଳ = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ସଂରଚନା AutoSuffix {
		ଅଚଳ std::string *prefix;
		std::string tail;
		auto operator<=>(ଅଚଳ AutoSuffix&) ଅଚଳ = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	};

	// "leaf", regular deduplicated string
	ସଂରଚନା Leaf {
		std::string s;
		auto operator<=>(ଅଚଳ Leaf&) ଅଚଳ = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ଅଚଳ;
};

ସଂରଚନା SmallString {
	ସ୍ଥିର constexpr uint32_t INLINE_CAP = 8;
	ସ୍ଥିର constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	explicit SmallString(std::string_view content);
	SmallString(ଅଚଳ SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ଅଚଳ SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ଅଚଳ { ଫେରନ୍ତୁ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ store(std::string_view content);
	ଶୂନ୍ୟ release();

	ସଂଘ {
		ଅକ୍ଷର inl_[INLINE_CAP];
		ଅକ୍ଷର *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ସଂରଚନା Twine_ଗଣ୍ଠି {
	ସ୍ଥିର constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	ସ୍ଥିର constexpr uint32_t DEAD = NO_PREFIX - 1;

	ସଂରଚନା ଚାବି_2 {
		uint32_t prefix;
		std::string_view text;
	};

	ସ୍ଥିର constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		ପାଇଁ (ଅକ୍ଷର c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned ଅକ୍ଷର>(c);
		ଫେରନ୍ତୁ hash;
	}

	Twine_ଗଣ୍ଠି() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	explicit Twine_ଗଣ୍ଠି(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_ଗଣ୍ଠି(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_ଗଣ୍ଠି(ଅଚଳ Twine_ଗଣ୍ଠି &other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	Twine_ଗଣ୍ଠି(Twine_ଗଣ୍ଠି &&other) noexcept;
	Twine_ଗଣ୍ଠି &operator=(ଅଚଳ Twine_ଗଣ୍ଠି &other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	Twine_ଗଣ୍ଠି &operator=(Twine_ଗଣ୍ଠି &&other) noexcept;
	~Twine_ଗଣ୍ଠି() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଗଣନା class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ଅଚଳ { ଫେରନ୍ତୁ prefix_ == DEAD; }
	constexpr bool is_leaf() ଅଚଳ { ଫେରନ୍ତୁ prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ଅଚଳ { ଫେରନ୍ତୁ prefix_ < DEAD; }

	constexpr Kind kind() ଅଚଳ {
		ଫେରନ୍ତୁ prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ଅଚଳ { ଫେରନ୍ତୁ text_.view(); }
	IdString prefix() ଅଚଳ { ଫେରନ୍ତୁ IdString(prefix_); }
	uint32_t hash() ଅଚଳ { ଫେରନ୍ତୁ hash_; }
	ଚାବି_2 ଚାବି() ଅଚଳ { ଫେରନ୍ତୁ {prefix_, text()}; }

	bool operator==(ଅଚଳ Twine_ଗଣ୍ଠି &other) ଅଚଳ
		{ ଫେରନ୍ତୁ prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ଅଚଳ ଚାବି_2 &other) ଅଚଳ
		{ ଫେରନ୍ତୁ prefix_ == other.prefix && text() == other.text; }

ବ୍ୟକ୍ତିଗତ:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(ଆକାର(Twine_ଗଣ୍ଠି) == (ଆକାର(ଶୂନ୍ୟ *) == 8 ? 24 : 20));

ସଂରଚନା StaticTwines {
	ସ୍ଥିର ଶୂନ୍ୟ init();
	ସ୍ଥିର ଅଚଳ Twine_ଗଣ୍ଠି &ଗଣ୍ଠି(size_t idx);
	ସ୍ଥିର bool ready();

ବ୍ୟକ୍ତିଗତ:
	ସ୍ଥିର std::vector<Twine_ଗଣ୍ଠି> nodes_;
};

ବାହ୍ୟ int64_t twine_gc_ns;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା twine_gc_ଗଣନା;

ସଂରଚନା TwinePool : HashConsPool<TwinePool, Twine_ଗଣ୍ଠି, IdString> {
	ସ୍ଥିର constexpr size_t STATIC_ଗଣନା = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ଅଚଳ TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ଅଚଳ TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ଅଚଳ Twine_ଗଣ୍ଠି& operator[](IdString ref) ଅଚଳ;

	ସ୍ଥିର ଅଚଳ Twine_ଗଣ୍ଠି& static_ଗଣ୍ଠି(size_t idx);

	ସ୍ଥିର ଶୂନ୍ୟ check_ready();
	ସ୍ଥିର uint32_t hash_ଗଣ୍ଠି(ଅଚଳ Twine_ଗଣ୍ଠି& t) { ଫେରନ୍ତୁ t.hash(); }

	template<typename F>
	ସ୍ଥିର ଶୂନ୍ୟ for_each_child(ଅଚଳ Twine_ଗଣ୍ଠି& t, F&& f) {
		ଯଦି (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	ଶୂନ୍ୟ dump(IdString ref, std::ostream& os = std::cout) ଅଚଳ;

	// Publicity bit provides escaping
	std::string str(IdString ref) ଅଚଳ;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ଅଚଳ;

	size_t str_size(IdString ref) ଅଚଳ;
	bool begins_with(IdString ref, std::string_view prefix) ଅଚଳ;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ଅଚଳ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare_by_name(IdString a, IdString b) ଅଚଳ;
	IdString prefix_of(IdString ref) ଅଚଳ;

	IdString find(std::string_view name) ଅଚଳ;
	IdString find(TwineSpec t) ଅଚଳ;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ଅଚଳ TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ଅଚଳ TwinePool& src, IdString ref) ଅଚଳ;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ଅଚଳ;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ଅଚଳ;
	ଶୂନ୍ୟ check_owned(IdString ref) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
	bool owns(IdString ref) ଅଚଳ;
	IdString stamp(IdString ref) ଅଚଳ;
	ଶୂନ୍ୟ append_str(IdString ref, std::string& out) ଅଚଳ;
	IdString auto_prefix(ଅଚଳ std::string *prefix);
	IdString add_inner(Twine_ଗଣ୍ଠି t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ଅଚଳ;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ଅଚଳ;
	IdString find_content(uint32_t prefix, std::string_view text) ଅଚଳ;
	ସ୍ଥିର size_t next_serial();
	dict<ଅଚଳ std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
