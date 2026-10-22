#अगर_अघोषित YOSYS_TWINE_H
#घोषणा YOSYS_TWINE_H

#समावेश "kernel/hashcons.h"
#समावेश "kernel/yosys_common.h"

#समावेश <algorithm>
#समावेश <bit>
#समावेश <cstdint>
#समावेश <cstring>
#समावेश <deque>
#समावेश <limits>
#समावेश <span>
#समावेश <string>
#समावेश <string_view>
#समावेश <unordered_set>
#समावेश <variant>
#समावेश <vector>

YOSYS_NAMESPACE_BEGIN

काष्ठा TwineSpec;
काष्ठा TwinePool;
काष्ठा IdString;

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

काष्ठा NullIdString {
	constexpr चालक IdString() स्थिर;
	constexpr bool चालक==(IdString ref) स्थिर;
};

काष्ठा IdString {
निजी:
#अगर_घोषित YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#अन्यथा
	using bits_t = uint32_t;
#पूर्ण_अगर
	bits_t मूल्य;

	अटल constexpr पूर्णांक TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	अटल constexpr पूर्णांक TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	अटल constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	अटल constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	अटल constexpr पूर्णांक TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	अटल constexpr bits_t TWINE_सूचकांक_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	अटल constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_सूचकांक_MASK | TWINE_PUBLIC_BIT));

खुला:
	अटल constexpr NullIdString Null{};

	अटल constexpr माप_प्रकार MAX_सूचकांक = TWINE_सूचकांक_MASK;
	अटल constexpr माप_प्रकार MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr माप_प्रकार raw() स्थिर {
		वापस मूल्य == TWINE_NULL_VAL ? मूल्य : (मूल्य & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr माप_प्रकार bits() स्थिर { वापस मूल्य; }

	constexpr माप_प्रकार order_कुंजी() स्थिर { वापस std::rotl((bits_t) eq_कुंजी(), 1); }

	constexpr माप_प्रकार serial() स्थिर {
		वापस मूल्य == TWINE_NULL_VAL ? 0 : ((मूल्य & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(माप_प्रकार pool_serial) स्थिर {
		वापस मूल्य == TWINE_NULL_VAL ? *this
			: IdString((मूल्य & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : मूल्य(TWINE_NULL_VAL) {}
	explicit constexpr IdString(माप_प्रकार val) : मूल्य((bits_t) val) {}

	constexpr माप_प्रकार eq_कुंजी() स्थिर { वापस मूल्य & ~TWINE_SERIAL_MASK; }

	constexpr bool चालक==(स्थिर IdString &rhs) स्थिर { वापस eq_कुंजी() == rhs.eq_कुंजी(); }
	constexpr bool चालक!=(स्थिर IdString &rhs) स्थिर { वापस eq_कुंजी() != rhs.eq_कुंजी(); }
	constexpr std::strong_ordering चालक<=>(स्थिर IdString &rhs) स्थिर {
		वापस order_कुंजी() <=> rhs.order_कुंजी();
	}

	ढाँचा <typename... Args>
	constexpr bool in(स्थिर Args&... args) स्थिर {
		वापस (in_one(args) || ...);
	}
	bool in(स्थिर pool<IdString> &rhs) स्थिर;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	ढाँचा<typename T> काष्ठा compare_ptr_by_name {
		bool चालक()(स्थिर T *a, स्थिर T *b) स्थिर {
			वापस (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() स्थिर { वापस मूल्य == TWINE_NULL_VAL; }
	constexpr व्योम clear() { मूल्य = TWINE_NULL_VAL; }
	constexpr bool isPublic() स्थिर { वापस मूल्य != TWINE_NULL_VAL && (मूल्य & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() स्थिर {
		वापस मूल्य == TWINE_NULL_VAL ? *this : IdString(मूल्य & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) स्थिर {
		वापस मूल्य == TWINE_NULL_VAL ? *this : IdString(pub ? (मूल्य | TWINE_PUBLIC_BIT) : (मूल्य & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() स्थिर;
	अटल माप_प्रकार handle_token_prefix(std::string_view token, bool &is_public);

निजी:
	constexpr bool in_one(IdString rhs) स्थिर { वापस *this == rhs; }
	bool in_one(स्थिर pool<IdString> &rhs) स्थिर;
};

constexpr NullIdString::चालक IdString() स्थिर { वापस IdString(); }
constexpr bool NullIdString::चालक==(IdString ref) स्थिर { वापस ref.empty(); }

namespace hashlib {
	ढाँचा<>
	काष्ठा hash_ops<IdString> {
		अटल अंतरभूत bool cmp(IdString a, IdString b) { वापस a == b; }
		[[nodiscard]] अटल अंतरभूत Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_कुंजी());
			वापस h;
		}
		[[nodiscard]] अटल अंतरभूत Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_कुंजी());
			वापस h;
		}
	};
}

अंतरभूत bool IdString::in_one(स्थिर pool<IdString> &rhs) स्थिर { वापस rhs.गिनती(*this) != 0; }
अंतरभूत bool IdString::in(स्थिर pool<IdString> &rhs) स्थिर { वापस in_one(rhs); }


क्रमागत : लघु {
#घोषणा X(N) IDX_##N,
#समावेश "kernel/constids.inc"
#अघोषित X
	STATIC_TWINE_END
};

काष्ठा ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#घोषणा X(N) अटल constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#समावेश "kernel/constids.inc"
#अघोषित X

	अटल constexpr स्थिर अक्षर* static_names[] = {
#घोषणा X(N) #N,
#समावेश "kernel/constids.inc"
#अघोषित X
	};

	अटल constexpr IdString lookup(std::string_view name)
	{
		पूर्णांक निचला = 0, ऊपरी = STATIC_TWINE_END;
		जबतक (ऊपरी - निचला >= 2) {
			पूर्णांक mid = (निचला + ऊपरी) / 2;
			अगर (name < static_names[mid])
				ऊपरी = mid;
			अन्यथा
				निचला = mid;
		}

		अगर (name != static_names[निचला])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		वापस IdString(निचला).tag(name[0] != '$');
	}

	अटल constexpr bool is_static(IdString ref) {
		वापस ref.untag().raw() < STATIC_TWINE_END;
	}

	अटल constexpr IdString static_ref(माप_प्रकार raw) {
		अगर (!is_static(IdString(raw)))
			throw "not a static twine id";
		वापस IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	अटल std::string str(IdString ref);
	अटल std::string unescaped_str(IdString ref);
};

ढाँचा<माप_प्रकार Raw> अंतरभूत constexpr IdString constid = ID::static_ref(Raw);

#घोषणा ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
काष्ठा TwineSpec {
	// deduplicates shared prefixes
	काष्ठा Suffix {
		IdString prefix;
		std::string पुच्छ;
		स्वतः चालक<=>(स्थिर Suffix&) स्थिर = शेष;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	काष्ठा AutoSuffix {
		स्थिर std::string *prefix;
		std::string पुच्छ;
		स्वतः चालक<=>(स्थिर AutoSuffix&) स्थिर = शेष;
	};

	// "leaf", regular deduplicated string
	काष्ठा Leaf {
		std::string s;
		स्वतः चालक<=>(स्थिर Leaf&) स्थिर = शेष;
	};

	std::variant<Leaf, Suffix, AutoSuffix> तथ्य;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() स्थिर;
};

काष्ठा SmallString {
	अटल constexpr uint32_t INLINE_CAP = 8;
	अटल constexpr माप_प्रकार MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = शेष;
	explicit SmallString(std::string_view content);
	SmallString(स्थिर SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &चालक=(स्थिर SmallString &other);
	SmallString &चालक=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() स्थिर { वापस {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

निजी:
	व्योम store(std::string_view content);
	व्योम release();

	जोड़ {
		अक्षर inl_[INLINE_CAP];
		अक्षर *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
काष्ठा Twine_गाँठ {
	अटल constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	अटल constexpr uint32_t DEAD = NO_PREFIX - 1;

	काष्ठा कुंजी_2 {
		uint32_t prefix;
		std::string_view text;
	};

	अटल constexpr uint32_t extend_hash(uint32_t hash, std::string_view पुच्छ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		क्रम (अक्षर c : पुच्छ)
			hash = ((hash << 5) + hash) + static_cast<अचिन्हित अक्षर>(c);
		वापस hash;
	}

	Twine_गाँठ() = शेष;
	explicit Twine_गाँठ(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_गाँठ(uint32_t prefix, std::string_view पुच्छ, uint32_t hash)
		: text_(पुच्छ), prefix_(prefix), hash_(hash) {}
	Twine_गाँठ(स्थिर Twine_गाँठ &other) = शेष;
	Twine_गाँठ(Twine_गाँठ &&other) noexcept;
	Twine_गाँठ &चालक=(स्थिर Twine_गाँठ &other) = शेष;
	Twine_गाँठ &चालक=(Twine_गाँठ &&other) noexcept;
	~Twine_गाँठ() = शेष;

	क्रमागत class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() स्थिर { वापस prefix_ == DEAD; }
	constexpr bool is_leaf() स्थिर { वापस prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() स्थिर { वापस prefix_ < DEAD; }

	constexpr Kind kind() स्थिर {
		वापस prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() स्थिर { वापस text_.view(); }
	IdString prefix() स्थिर { वापस IdString(prefix_); }
	uint32_t hash() स्थिर { वापस hash_; }
	कुंजी_2 कुंजी() स्थिर { वापस {prefix_, text()}; }

	bool चालक==(स्थिर Twine_गाँठ &other) स्थिर
		{ वापस prefix_ == other.prefix_ && text() == other.text(); }
	bool चालक==(स्थिर कुंजी_2 &other) स्थिर
		{ वापस prefix_ == other.prefix && text() == other.text; }

निजी:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(माप(Twine_गाँठ) == (माप(व्योम *) == 8 ? 24 : 20));

काष्ठा StaticTwines {
	अटल व्योम init();
	अटल स्थिर Twine_गाँठ &गाँठ(माप_प्रकार idx);
	अटल bool ready();

निजी:
	अटल std::vector<Twine_गाँठ> nodes_;
};

बाह्य int64_t twine_gc_ns;
बाह्य पूर्णांक twine_gc_गिनती;

काष्ठा TwinePool : HashConsPool<TwinePool, Twine_गाँठ, IdString> {
	अटल constexpr माप_प्रकार STATIC_गिनती = STATIC_TWINE_END;

	TwinePool();
	TwinePool(स्थिर TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& चालक=(स्थिर TwinePool& other);
	TwinePool& चालक=(TwinePool&& other);

	स्थिर Twine_गाँठ& चालक[](IdString ref) स्थिर;

	अटल स्थिर Twine_गाँठ& static_गाँठ(माप_प्रकार idx);

	अटल व्योम check_ready();
	अटल uint32_t hash_गाँठ(स्थिर Twine_गाँठ& t) { वापस t.hash(); }

	ढाँचा<typename F>
	अटल व्योम for_each_child(स्थिर Twine_गाँठ& t, F&& f) {
		अगर (t.is_suffix())
			f(t.prefix());
	}

	माप_प्रकार gc(pool<IdString>& roots);

	व्योम dump(IdString ref, std::ostream& os = std::cout) स्थिर;

	// Publicity bit provides escaping
	std::string str(IdString ref) स्थिर;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) स्थिर;

	माप_प्रकार str_आकार(IdString ref) स्थिर;
	bool begins_with(IdString ref, std::string_view prefix) स्थिर;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) स्थिर;
	पूर्णांक compare_by_name(IdString a, IdString b) स्थिर;
	IdString prefix_of(IdString ref) स्थिर;

	IdString find(std::string_view name) स्थिर;
	IdString find(TwineSpec t) स्थिर;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view पुच्छ);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(स्थिर TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(स्थिर TwinePool& src, IdString ref) स्थिर;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) स्थिर;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) स्थिर;
	व्योम check_owned(IdString ref) स्थिर;

निजी:
	bool owns(IdString ref) स्थिर;
	IdString stamp(IdString ref) स्थिर;
	व्योम append_str(IdString ref, std::string& out) स्थिर;
	IdString auto_prefix(स्थिर std::string *prefix);
	IdString add_inner(Twine_गाँठ t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) स्थिर;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) स्थिर;
	IdString find_content(uint32_t prefix, std::string_view text) स्थिर;
	अटल माप_प्रकार अग्र_serial();
	dict<स्थिर std::string *, IdString> auto_prefixes;
	माप_प्रकार serial_;
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
