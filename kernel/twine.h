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
	constexpr चालक IdString() अचर;
	constexpr bool चालक==(IdString ref) अचर;
};

संरचना IdString {
गुप्त:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#अन्यथा
	using bits_t = uint32_t;
#endif
	bits_t मान;

	स्थिर constexpr पूर्णाङ्क TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	स्थिर constexpr पूर्णाङ्क TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	स्थिर constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	स्थिर constexpr पूर्णाङ्क TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	स्थिर constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

सार्वजनिक:
	स्थिर constexpr NullIdString Null{};

	स्थिर constexpr माप_प्रकार MAX_INDEX = TWINE_INDEX_MASK;
	स्थिर constexpr माप_प्रकार MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr माप_प्रकार raw() अचर {
		फर्काऊ मान == TWINE_NULL_VAL ? मान : (मान & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr माप_प्रकार bits() अचर { फर्काऊ मान; }

	constexpr माप_प्रकार order_साँचो() अचर { फर्काऊ std::rotl((bits_t) eq_साँचो(), 1); }

	constexpr माप_प्रकार serial() अचर {
		फर्काऊ मान == TWINE_NULL_VAL ? 0 : ((मान & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(माप_प्रकार pool_serial) अचर {
		फर्काऊ मान == TWINE_NULL_VAL ? *this
			: IdString((मान & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : मान(TWINE_NULL_VAL) {}
	explicit constexpr IdString(माप_प्रकार val) : मान((bits_t) val) {}

	constexpr माप_प्रकार eq_साँचो() अचर { फर्काऊ मान & ~TWINE_SERIAL_MASK; }

	constexpr bool चालक==(अचर IdString &rhs) अचर { फर्काऊ eq_साँचो() == rhs.eq_साँचो(); }
	constexpr bool चालक!=(अचर IdString &rhs) अचर { फर्काऊ eq_साँचो() != rhs.eq_साँचो(); }
	constexpr std::strong_ordering चालक<=>(अचर IdString &rhs) अचर {
		फर्काऊ order_साँचो() <=> rhs.order_साँचो();
	}

	ढाँचा <typename... Args>
	constexpr bool in(अचर Args&... args) अचर {
		फर्काऊ (in_one(args) || ...);
	}
	bool in(अचर pool<IdString> &rhs) अचर;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	ढाँचा<typename T> संरचना compare_ptr_by_name {
		bool चालक()(अचर T *a, अचर T *b) अचर {
			फर्काऊ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() अचर { फर्काऊ मान == TWINE_NULL_VAL; }
	constexpr शून्य clear() { मान = TWINE_NULL_VAL; }
	constexpr bool isPublic() अचर { फर्काऊ मान != TWINE_NULL_VAL && (मान & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() अचर {
		फर्काऊ मान == TWINE_NULL_VAL ? *this : IdString(मान & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) अचर {
		फर्काऊ मान == TWINE_NULL_VAL ? *this : IdString(pub ? (मान | TWINE_PUBLIC_BIT) : (मान & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() अचर;
	स्थिर माप_प्रकार handle_token_prefix(std::string_view token, bool &is_public);

गुप्त:
	constexpr bool in_one(IdString rhs) अचर { फर्काऊ *this == rhs; }
	bool in_one(अचर pool<IdString> &rhs) अचर;
};

constexpr NullIdString::चालक IdString() अचर { फर्काऊ IdString(); }
constexpr bool NullIdString::चालक==(IdString ref) अचर { फर्काऊ ref.empty(); }

namespace hashlib {
	ढाँचा<>
	संरचना hash_ops<IdString> {
		स्थिर अन्तःस्थ bool cmp(IdString a, IdString b) { फर्काऊ a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_साँचो());
			फर्काऊ h;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_साँचो());
			फर्काऊ h;
		}
	};
}

अन्तःस्थ bool IdString::in_one(अचर pool<IdString> &rhs) अचर { फर्काऊ rhs.गन्ती(*this) != 0; }
अन्तःस्थ bool IdString::in(अचर pool<IdString> &rhs) अचर { फर्काऊ in_one(rhs); }


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

	स्थिर constexpr अचर वर्ण* static_names[] = {
#define X(N) #N,
#अन्तर्भाव "kernel/constids.inc"
#undef X
	};

	स्थिर constexpr IdString lookup(std::string_view name)
	{
		पूर्णाङ्क तल्लो = 0, माथिल्लो = STATIC_TWINE_END;
		जबसम्म (माथिल्लो - तल्लो >= 2) {
			पूर्णाङ्क mid = (तल्लो + माथिल्लो) / 2;
			यदि (name < static_names[mid])
				माथिल्लो = mid;
			अन्यथा
				तल्लो = mid;
		}

		यदि (name != static_names[तल्लो])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		फर्काऊ IdString(तल्लो).tag(name[0] != '$');
	}

	स्थिर constexpr bool is_static(IdString ref) {
		फर्काऊ ref.untag().raw() < STATIC_TWINE_END;
	}

	स्थिर constexpr IdString static_ref(माप_प्रकार raw) {
		यदि (!is_static(IdString(raw)))
			throw "not a static twine id";
		फर्काऊ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	स्थिर std::string str(IdString ref);
	स्थिर std::string unescaped_str(IdString ref);
};

ढाँचा<माप_प्रकार Raw> अन्तःस्थ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
संरचना TwineSpec {
	// deduplicates shared prefixes
	संरचना Suffix {
		IdString prefix;
		std::string tail;
		स्वतः चालक<=>(अचर Suffix&) अचर = स्वभाव;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	संरचना AutoSuffix {
		अचर std::string *prefix;
		std::string tail;
		स्वतः चालक<=>(अचर AutoSuffix&) अचर = स्वभाव;
	};

	// "leaf", regular deduplicated string
	संरचना Leaf {
		std::string s;
		स्वतः चालक<=>(अचर Leaf&) अचर = स्वभाव;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() अचर;
};

संरचना SmallString {
	स्थिर constexpr uint32_t INLINE_CAP = 8;
	स्थिर constexpr माप_प्रकार MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = स्वभाव;
	explicit SmallString(std::string_view content);
	SmallString(अचर SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &चालक=(अचर SmallString &other);
	SmallString &चालक=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() अचर { फर्काऊ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

गुप्त:
	शून्य store(std::string_view content);
	शून्य release();

	सङ्घ {
		वर्ण inl_[INLINE_CAP];
		वर्ण *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
संरचना Twine_गाँठो {
	स्थिर constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	स्थिर constexpr uint32_t DEAD = NO_PREFIX - 1;

	संरचना साँचो_2 {
		uint32_t prefix;
		std::string_view text;
	};

	स्थिर constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		लागि (वर्ण c : tail)
			hash = ((hash << 5) + hash) + static_cast<अचिन्हित वर्ण>(c);
		फर्काऊ hash;
	}

	Twine_गाँठो() = स्वभाव;
	explicit Twine_गाँठो(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_गाँठो(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_गाँठो(अचर Twine_गाँठो &other) = स्वभाव;
	Twine_गाँठो(Twine_गाँठो &&other) noexcept;
	Twine_गाँठो &चालक=(अचर Twine_गाँठो &other) = स्वभाव;
	Twine_गाँठो &चालक=(Twine_गाँठो &&other) noexcept;
	~Twine_गाँठो() = स्वभाव;

	गणना class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() अचर { फर्काऊ prefix_ == DEAD; }
	constexpr bool is_leaf() अचर { फर्काऊ prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() अचर { फर्काऊ prefix_ < DEAD; }

	constexpr Kind kind() अचर {
		फर्काऊ prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() अचर { फर्काऊ text_.view(); }
	IdString prefix() अचर { फर्काऊ IdString(prefix_); }
	uint32_t hash() अचर { फर्काऊ hash_; }
	साँचो_2 साँचो() अचर { फर्काऊ {prefix_, text()}; }

	bool चालक==(अचर Twine_गाँठो &other) अचर
		{ फर्काऊ prefix_ == other.prefix_ && text() == other.text(); }
	bool चालक==(अचर साँचो_2 &other) अचर
		{ फर्काऊ prefix_ == other.prefix && text() == other.text; }

गुप्त:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(परिमाण(Twine_गाँठो) == (परिमाण(शून्य *) == 8 ? 24 : 20));

संरचना StaticTwines {
	स्थिर शून्य init();
	स्थिर अचर Twine_गाँठो &गाँठो(माप_प्रकार idx);
	स्थिर bool ready();

गुप्त:
	स्थिर std::vector<Twine_गाँठो> nodes_;
};

बाह्य int64_t twine_gc_ns;
बाह्य पूर्णाङ्क twine_gc_गन्ती;

संरचना TwinePool : HashConsPool<TwinePool, Twine_गाँठो, IdString> {
	स्थिर constexpr माप_प्रकार STATIC_गन्ती = STATIC_TWINE_END;

	TwinePool();
	TwinePool(अचर TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& चालक=(अचर TwinePool& other);
	TwinePool& चालक=(TwinePool&& other);

	अचर Twine_गाँठो& चालक[](IdString ref) अचर;

	स्थिर अचर Twine_गाँठो& static_गाँठो(माप_प्रकार idx);

	स्थिर शून्य check_ready();
	स्थिर uint32_t hash_गाँठो(अचर Twine_गाँठो& t) { फर्काऊ t.hash(); }

	ढाँचा<typename F>
	स्थिर शून्य for_each_child(अचर Twine_गाँठो& t, F&& f) {
		यदि (t.is_suffix())
			f(t.prefix());
	}

	माप_प्रकार gc(pool<IdString>& roots);

	शून्य dump(IdString ref, std::ostream& os = std::cout) अचर;

	// Publicity bit provides escaping
	std::string str(IdString ref) अचर;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) अचर;

	माप_प्रकार str_size(IdString ref) अचर;
	bool begins_with(IdString ref, std::string_view prefix) अचर;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) अचर;
	पूर्णाङ्क compare_by_name(IdString a, IdString b) अचर;
	IdString prefix_of(IdString ref) अचर;

	IdString find(std::string_view name) अचर;
	IdString find(TwineSpec t) अचर;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(अचर TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(अचर TwinePool& src, IdString ref) अचर;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) अचर;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) अचर;
	शून्य check_owned(IdString ref) अचर;

गुप्त:
	bool owns(IdString ref) अचर;
	IdString stamp(IdString ref) अचर;
	शून्य append_str(IdString ref, std::string& out) अचर;
	IdString auto_prefix(अचर std::string *prefix);
	IdString add_inner(Twine_गाँठो t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) अचर;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) अचर;
	IdString find_content(uint32_t prefix, std::string_view text) अचर;
	स्थिर माप_प्रकार next_serial();
	dict<अचर std::string *, IdString> auto_prefixes;
	माप_प्रकार serial_;
};

YOSYS_NAMESPACE_END

#endif
