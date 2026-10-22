#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#अन्तर्भाव "kernel/hashcons.h"
#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <algorithm>
#अन्तर्भाव <bit>
#अन्तर्भाव <cstdint>
#अन्तर्भाव <cstring>
#अन्तर्भाव <deque>
#अन्तर्भाव <limits>
#अन्तर्भाव <span>
#अन्तर्भाव <string>
#अन्तर्भाव <string_view>
#अन्तर्भाव <unordered_set>
#अन्तर्भाव <variant>
#अन्तर्भाव <vector>

YOSYS_NAMESPACE_BEGIN

संरचना TwineSpec;
संरचना TwinePool;
संरचना IdString;

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

संरचना NullIdString {
	constexpr चालक IdString() स्थिरांक;
	constexpr bool चालक==(IdString ref) स्थिरांक;
};

संरचना IdString {
खाजगी:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#अन्यथा
	using bits_t = uint32_t;
#endif
	bits_t मूल्य;

	स्थिर constexpr पूर्णांक TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	स्थिर constexpr पूर्णांक TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	स्थिर constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	स्थिर constexpr पूर्णांक TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	स्थिर constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

सार्वजनिक:
	स्थिर constexpr NullIdString Null{};

	स्थिर constexpr माप_प्रकार MAX_INDEX = TWINE_INDEX_MASK;
	स्थिर constexpr माप_प्रकार MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr माप_प्रकार raw() स्थिरांक {
		परत मूल्य == TWINE_NULL_VAL ? मूल्य : (मूल्य & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr माप_प्रकार bits() स्थिरांक { परत मूल्य; }

	constexpr माप_प्रकार order_किल्ली() स्थिरांक { परत std::rotl((bits_t) eq_किल्ली(), 1); }

	constexpr माप_प्रकार serial() स्थिरांक {
		परत मूल्य == TWINE_NULL_VAL ? 0 : ((मूल्य & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(माप_प्रकार pool_serial) स्थिरांक {
		परत मूल्य == TWINE_NULL_VAL ? *this
			: IdString((मूल्य & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : मूल्य(TWINE_NULL_VAL) {}
	explicit constexpr IdString(माप_प्रकार val) : मूल्य((bits_t) val) {}

	constexpr माप_प्रकार eq_किल्ली() स्थिरांक { परत मूल्य & ~TWINE_SERIAL_MASK; }

	constexpr bool चालक==(स्थिरांक IdString &rhs) स्थिरांक { परत eq_किल्ली() == rhs.eq_किल्ली(); }
	constexpr bool चालक!=(स्थिरांक IdString &rhs) स्थिरांक { परत eq_किल्ली() != rhs.eq_किल्ली(); }
	constexpr std::strong_ordering चालक<=>(स्थिरांक IdString &rhs) स्थिरांक {
		परत order_किल्ली() <=> rhs.order_किल्ली();
	}

	ढाँचा <typename... Args>
	constexpr bool in(स्थिरांक Args&... args) स्थिरांक {
		परत (in_one(args) || ...);
	}
	bool in(स्थिरांक pool<IdString> &rhs) स्थिरांक;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	ढाँचा<typename T> संरचना compare_ptr_by_name {
		bool चालक()(स्थिरांक T *a, स्थिरांक T *b) स्थिरांक {
			परत (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() स्थिरांक { परत मूल्य == TWINE_NULL_VAL; }
	constexpr शून्य clear() { मूल्य = TWINE_NULL_VAL; }
	constexpr bool isPublic() स्थिरांक { परत मूल्य != TWINE_NULL_VAL && (मूल्य & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() स्थिरांक {
		परत मूल्य == TWINE_NULL_VAL ? *this : IdString(मूल्य & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) स्थिरांक {
		परत मूल्य == TWINE_NULL_VAL ? *this : IdString(pub ? (मूल्य | TWINE_PUBLIC_BIT) : (मूल्य & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() स्थिरांक;
	स्थिर माप_प्रकार handle_token_prefix(std::string_view token, bool &is_public);

खाजगी:
	constexpr bool in_one(IdString rhs) स्थिरांक { परत *this == rhs; }
	bool in_one(स्थिरांक pool<IdString> &rhs) स्थिरांक;
};

constexpr NullIdString::चालक IdString() स्थिरांक { परत IdString(); }
constexpr bool NullIdString::चालक==(IdString ref) स्थिरांक { परत ref.empty(); }

namespace hashlib {
	ढाँचा<>
	संरचना hash_ops<IdString> {
		स्थिर ओळीत bool cmp(IdString a, IdString b) { परत a == b; }
		[[nodiscard]] स्थिर ओळीत Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_किल्ली());
			परत h;
		}
		[[nodiscard]] स्थिर ओळीत Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_किल्ली());
			परत h;
		}
	};
}

ओळीत bool IdString::in_one(स्थिरांक pool<IdString> &rhs) स्थिरांक { परत rhs.मोजणी(*this) != 0; }
ओळीत bool IdString::in(स्थिरांक pool<IdString> &rhs) स्थिरांक { परत in_one(rhs); }


गणना : लघु {
#define X(N) IDX_##N,
#अन्तर्भाव "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

संरचना ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) स्थिर constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#अन्तर्भाव "kernel/constids.inc"
#undef X

	स्थिर constexpr स्थिरांक अक्षर* static_names[] = {
#define X(N) #N,
#अन्तर्भाव "kernel/constids.inc"
#undef X
	};

	स्थिर constexpr IdString lookup(std::string_view name)
	{
		पूर्णांक खालचा = 0, वरचा = STATIC_TWINE_END;
		पर्यंत (वरचा - खालचा >= 2) {
			पूर्णांक mid = (खालचा + वरचा) / 2;
			जर (name < static_names[mid])
				वरचा = mid;
			अन्यथा
				खालचा = mid;
		}

		जर (name != static_names[खालचा])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		परत IdString(खालचा).tag(name[0] != '$');
	}

	स्थिर constexpr bool is_static(IdString ref) {
		परत ref.untag().raw() < STATIC_TWINE_END;
	}

	स्थिर constexpr IdString static_ref(माप_प्रकार raw) {
		जर (!is_static(IdString(raw)))
			throw "not a static twine id";
		परत IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	स्थिर std::string str(IdString ref);
	स्थिर std::string unescaped_str(IdString ref);
};

ढाँचा<माप_प्रकार Raw> ओळीत constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
संरचना TwineSpec {
	// deduplicates shared prefixes
	संरचना Suffix {
		IdString prefix;
		std::string tail;
		स्वतः चालक<=>(स्थिरांक Suffix&) स्थिरांक = पूर्वनिर्धारित;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	संरचना AutoSuffix {
		स्थिरांक std::string *prefix;
		std::string tail;
		स्वतः चालक<=>(स्थिरांक AutoSuffix&) स्थिरांक = पूर्वनिर्धारित;
	};

	// "leaf", regular deduplicated string
	संरचना Leaf {
		std::string s;
		स्वतः चालक<=>(स्थिरांक Leaf&) स्थिरांक = पूर्वनिर्धारित;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() स्थिरांक;
};

संरचना SmallString {
	स्थिर constexpr uint32_t INLINE_CAP = 8;
	स्थिर constexpr माप_प्रकार MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = पूर्वनिर्धारित;
	explicit SmallString(std::string_view content);
	SmallString(स्थिरांक SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &चालक=(स्थिरांक SmallString &other);
	SmallString &चालक=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() स्थिरांक { परत {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

खाजगी:
	शून्य store(std::string_view content);
	शून्य release();

	संघ {
		अक्षर inl_[INLINE_CAP];
		अक्षर *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
संरचना Twine_गाठ {
	स्थिर constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	स्थिर constexpr uint32_t DEAD = NO_PREFIX - 1;

	संरचना किल्ली_2 {
		uint32_t prefix;
		std::string_view text;
	};

	स्थिर constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		साठी (अक्षर c : tail)
			hash = ((hash << 5) + hash) + static_cast<अचिन्हित अक्षर>(c);
		परत hash;
	}

	Twine_गाठ() = पूर्वनिर्धारित;
	explicit Twine_गाठ(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_गाठ(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_गाठ(स्थिरांक Twine_गाठ &other) = पूर्वनिर्धारित;
	Twine_गाठ(Twine_गाठ &&other) noexcept;
	Twine_गाठ &चालक=(स्थिरांक Twine_गाठ &other) = पूर्वनिर्धारित;
	Twine_गाठ &चालक=(Twine_गाठ &&other) noexcept;
	~Twine_गाठ() = पूर्वनिर्धारित;

	गणना class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() स्थिरांक { परत prefix_ == DEAD; }
	constexpr bool is_leaf() स्थिरांक { परत prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() स्थिरांक { परत prefix_ < DEAD; }

	constexpr Kind kind() स्थिरांक {
		परत prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() स्थिरांक { परत text_.view(); }
	IdString prefix() स्थिरांक { परत IdString(prefix_); }
	uint32_t hash() स्थिरांक { परत hash_; }
	किल्ली_2 किल्ली() स्थिरांक { परत {prefix_, text()}; }

	bool चालक==(स्थिरांक Twine_गाठ &other) स्थिरांक
		{ परत prefix_ == other.prefix_ && text() == other.text(); }
	bool चालक==(स्थिरांक किल्ली_2 &other) स्थिरांक
		{ परत prefix_ == other.prefix && text() == other.text; }

खाजगी:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(आकार(Twine_गाठ) == (आकार(शून्य *) == 8 ? 24 : 20));

संरचना StaticTwines {
	स्थिर शून्य init();
	स्थिर स्थिरांक Twine_गाठ &गाठ(माप_प्रकार idx);
	स्थिर bool ready();

खाजगी:
	स्थिर std::vector<Twine_गाठ> nodes_;
};

बाह्य int64_t twine_gc_ns;
बाह्य पूर्णांक twine_gc_मोजणी;

संरचना TwinePool : HashConsPool<TwinePool, Twine_गाठ, IdString> {
	स्थिर constexpr माप_प्रकार STATIC_मोजणी = STATIC_TWINE_END;

	TwinePool();
	TwinePool(स्थिरांक TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& चालक=(स्थिरांक TwinePool& other);
	TwinePool& चालक=(TwinePool&& other);

	स्थिरांक Twine_गाठ& चालक[](IdString ref) स्थिरांक;

	स्थिर स्थिरांक Twine_गाठ& static_गाठ(माप_प्रकार idx);

	स्थिर शून्य check_ready();
	स्थिर uint32_t hash_गाठ(स्थिरांक Twine_गाठ& t) { परत t.hash(); }

	ढाँचा<typename F>
	स्थिर शून्य for_each_child(स्थिरांक Twine_गाठ& t, F&& f) {
		जर (t.is_suffix())
			f(t.prefix());
	}

	माप_प्रकार gc(pool<IdString>& roots);

	शून्य dump(IdString ref, std::ostream& os = std::cout) स्थिरांक;

	// Publicity bit provides escaping
	std::string str(IdString ref) स्थिरांक;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) स्थिरांक;

	माप_प्रकार str_size(IdString ref) स्थिरांक;
	bool begins_with(IdString ref, std::string_view prefix) स्थिरांक;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) स्थिरांक;
	पूर्णांक compare_by_name(IdString a, IdString b) स्थिरांक;
	IdString prefix_of(IdString ref) स्थिरांक;

	IdString find(std::string_view name) स्थिरांक;
	IdString find(TwineSpec t) स्थिरांक;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(स्थिरांक TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(स्थिरांक TwinePool& src, IdString ref) स्थिरांक;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) स्थिरांक;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) स्थिरांक;
	शून्य check_owned(IdString ref) स्थिरांक;

खाजगी:
	bool owns(IdString ref) स्थिरांक;
	IdString stamp(IdString ref) स्थिरांक;
	शून्य append_str(IdString ref, std::string& out) स्थिरांक;
	IdString auto_prefix(स्थिरांक std::string *prefix);
	IdString add_inner(Twine_गाठ t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) स्थिरांक;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) स्थिरांक;
	IdString find_content(uint32_t prefix, std::string_view text) स्थिरांक;
	स्थिर माप_प्रकार next_serial();
	dict<स्थिरांक std::string *, IdString> auto_prefixes;
	माप_प्रकार serial_;
};

YOSYS_NAMESPACE_END

#endif
