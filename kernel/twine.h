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
	constexpr operator IdString() ध्रुव;
	constexpr bool operator==(IdString ref) ध्रुव;
};

संरचना IdString {
गुप्त:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#अञ्ञथा
	using bits_t = uint32_t;
#endif
	bits_t मानम्;

	स्थिर constexpr पूर्णाङ्क TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	स्थिर constexpr पूर्णाङ्क TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	स्थिर constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	स्थिर constexpr पूर्णाङ्क TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	स्थिर constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	स्थिर constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

सार्वजनिक:
	स्थिर constexpr NullIdString Null{};

	स्थिर constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	स्थिर constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ध्रुव {
		पटिनिवत्त मानम् == TWINE_NULL_VAL ? मानम् : (मानम् & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ध्रुव { पटिनिवत्त मानम्; }

	constexpr size_t order_कुञ्चिका() ध्रुव { पटिनिवत्त std::rotl((bits_t) eq_कुञ्चिका(), 1); }

	constexpr size_t serial() ध्रुव {
		पटिनिवत्त मानम् == TWINE_NULL_VAL ? 0 : ((मानम् & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ध्रुव {
		पटिनिवत्त मानम् == TWINE_NULL_VAL ? *this
			: IdString((मानम् & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : मानम्(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : मानम्((bits_t) val) {}

	constexpr size_t eq_कुञ्चिका() ध्रुव { पटिनिवत्त मानम् & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ध्रुव IdString &rhs) ध्रुव { पटिनिवत्त eq_कुञ्चिका() == rhs.eq_कुञ्चिका(); }
	constexpr bool operator!=(ध्रुव IdString &rhs) ध्रुव { पटिनिवत्त eq_कुञ्चिका() != rhs.eq_कुञ्चिका(); }
	constexpr std::strong_ordering operator<=>(ध्रुव IdString &rhs) ध्रुव {
		पटिनिवत्त order_कुञ्चिका() <=> rhs.order_कुञ्चिका();
	}

	template <typename... Args>
	constexpr bool in(ध्रुव Args&... args) ध्रुव {
		पटिनिवत्त (in_one(args) || ...);
	}
	bool in(ध्रुव pool<IdString> &rhs) ध्रुव;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> संरचना compare_ptr_by_name {
		bool operator()(ध्रुव T *a, ध्रुव T *b) ध्रुव {
			पटिनिवत्त (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ध्रुव { पटिनिवत्त मानम् == TWINE_NULL_VAL; }
	constexpr शून्य clear() { मानम् = TWINE_NULL_VAL; }
	constexpr bool isPublic() ध्रुव { पटिनिवत्त मानम् != TWINE_NULL_VAL && (मानम् & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ध्रुव {
		पटिनिवत्त मानम् == TWINE_NULL_VAL ? *this : IdString(मानम् & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ध्रुव {
		पटिनिवत्त मानम् == TWINE_NULL_VAL ? *this : IdString(pub ? (मानम् | TWINE_PUBLIC_BIT) : (मानम् & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ध्रुव;
	स्थिर size_t handle_token_prefix(std::string_view token, bool &is_public);

गुप्त:
	constexpr bool in_one(IdString rhs) ध्रुव { पटिनिवत्त *this == rhs; }
	bool in_one(ध्रुव pool<IdString> &rhs) ध्रुव;
};

constexpr NullIdString::operator IdString() ध्रुव { पटिनिवत्त IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ध्रुव { पटिनिवत्त ref.empty(); }

namespace hashlib {
	template<>
	संरचना hash_ops<IdString> {
		स्थिर अन्तःस्थ bool cmp(IdString a, IdString b) { पटिनिवत्त a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_कुञ्चिका());
			पटिनिवत्त h;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_कुञ्चिका());
			पटिनिवत्त h;
		}
	};
}

अन्तःस्थ bool IdString::in_one(ध्रुव pool<IdString> &rhs) ध्रुव { पटिनिवत्त rhs.गणनम्(*this) != 0; }
अन्तःस्थ bool IdString::in(ध्रुव pool<IdString> &rhs) ध्रुव { पटिनिवत्त in_one(rhs); }


गणना : short {
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

	स्थिर constexpr ध्रुव वर्ण* static_names[] = {
#define X(N) #N,
#अन्तर्भाव "kernel/constids.inc"
#undef X
	};

	स्थिर constexpr IdString lookup(std::string_view name)
	{
		पूर्णाङ्क नीचः = 0, उच्चः = STATIC_TWINE_END;
		याव (उच्चः - नीचः >= 2) {
			पूर्णाङ्क mid = (नीचः + उच्चः) / 2;
			सचे (name < static_names[mid])
				उच्चः = mid;
			अञ्ञथा
				नीचः = mid;
		}

		सचे (name != static_names[नीचः])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		पटिनिवत्त IdString(नीचः).tag(name[0] != '$');
	}

	स्थिर constexpr bool is_static(IdString ref) {
		पटिनिवत्त ref.untag().raw() < STATIC_TWINE_END;
	}

	स्थिर constexpr IdString static_ref(size_t raw) {
		सचे (!is_static(IdString(raw)))
			throw "not a static twine id";
		पटिनिवत्त IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	स्थिर std::string str(IdString ref);
	स्थिर std::string unescaped_str(IdString ref);
};

template<size_t Raw> अन्तःस्थ constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
संरचना TwineSpec {
	// deduplicates shared prefixes
	संरचना Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ध्रुव Suffix&) ध्रुव = स्वभाव;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	संरचना AutoSuffix {
		ध्रुव std::string *prefix;
		std::string tail;
		auto operator<=>(ध्रुव AutoSuffix&) ध्रुव = स्वभाव;
	};

	// "leaf", regular deduplicated string
	संरचना Leaf {
		std::string s;
		auto operator<=>(ध्रुव Leaf&) ध्रुव = स्वभाव;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ध्रुव;
};

संरचना SmallString {
	स्थिर constexpr uint32_t INLINE_CAP = 8;
	स्थिर constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = स्वभाव;
	explicit SmallString(std::string_view content);
	SmallString(ध्रुव SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ध्रुव SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ध्रुव { पटिनिवत्त {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

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
संरचना Twine_ग्रन्थिः {
	स्थिर constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	स्थिर constexpr uint32_t DEAD = NO_PREFIX - 1;

	संरचना कुञ्चिका_2 {
		uint32_t prefix;
		std::string_view text;
	};

	स्थिर constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		अत्थाय (वर्ण c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned वर्ण>(c);
		पटिनिवत्त hash;
	}

	Twine_ग्रन्थिः() = स्वभाव;
	explicit Twine_ग्रन्थिः(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_ग्रन्थिः(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_ग्रन्थिः(ध्रुव Twine_ग्रन्थिः &other) = स्वभाव;
	Twine_ग्रन्थिः(Twine_ग्रन्थिः &&other) noexcept;
	Twine_ग्रन्थिः &operator=(ध्रुव Twine_ग्रन्थिः &other) = स्वभाव;
	Twine_ग्रन्थिः &operator=(Twine_ग्रन्थिः &&other) noexcept;
	~Twine_ग्रन्थिः() = स्वभाव;

	गणना class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ध्रुव { पटिनिवत्त prefix_ == DEAD; }
	constexpr bool is_leaf() ध्रुव { पटिनिवत्त prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ध्रुव { पटिनिवत्त prefix_ < DEAD; }

	constexpr Kind kind() ध्रुव {
		पटिनिवत्त prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ध्रुव { पटिनिवत्त text_.view(); }
	IdString prefix() ध्रुव { पटिनिवत्त IdString(prefix_); }
	uint32_t hash() ध्रुव { पटिनिवत्त hash_; }
	कुञ्चिका_2 कुञ्चिका() ध्रुव { पटिनिवत्त {prefix_, text()}; }

	bool operator==(ध्रुव Twine_ग्रन्थिः &other) ध्रुव
		{ पटिनिवत्त prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ध्रुव कुञ्चिका_2 &other) ध्रुव
		{ पटिनिवत्त prefix_ == other.prefix && text() == other.text; }

गुप्त:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(परिमाण(Twine_ग्रन्थिः) == (परिमाण(शून्य *) == 8 ? 24 : 20));

संरचना StaticTwines {
	स्थिर शून्य init();
	स्थिर ध्रुव Twine_ग्रन्थिः &ग्रन्थिः(size_t idx);
	स्थिर bool ready();

गुप्त:
	स्थिर std::vector<Twine_ग्रन्थिः> nodes_;
};

बाह्य int64_t twine_gc_ns;
बाह्य पूर्णाङ्क twine_gc_गणनम्;

संरचना TwinePool : HashConsPool<TwinePool, Twine_ग्रन्थिः, IdString> {
	स्थिर constexpr size_t STATIC_गणनम् = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ध्रुव TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ध्रुव TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ध्रुव Twine_ग्रन्थिः& operator[](IdString ref) ध्रुव;

	स्थिर ध्रुव Twine_ग्रन्थिः& static_ग्रन्थिः(size_t idx);

	स्थिर शून्य check_ready();
	स्थिर uint32_t hash_ग्रन्थिः(ध्रुव Twine_ग्रन्थिः& t) { पटिनिवत्त t.hash(); }

	template<typename F>
	स्थिर शून्य for_each_child(ध्रुव Twine_ग्रन्थिः& t, F&& f) {
		सचे (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	शून्य dump(IdString ref, std::ostream& os = std::cout) ध्रुव;

	// Publicity bit provides escaping
	std::string str(IdString ref) ध्रुव;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ध्रुव;

	size_t str_size(IdString ref) ध्रुव;
	bool begins_with(IdString ref, std::string_view prefix) ध्रुव;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ध्रुव;
	पूर्णाङ्क compare_by_name(IdString a, IdString b) ध्रुव;
	IdString prefix_of(IdString ref) ध्रुव;

	IdString find(std::string_view name) ध्रुव;
	IdString find(TwineSpec t) ध्रुव;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ध्रुव TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ध्रुव TwinePool& src, IdString ref) ध्रुव;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ध्रुव;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ध्रुव;
	शून्य check_owned(IdString ref) ध्रुव;

गुप्त:
	bool owns(IdString ref) ध्रुव;
	IdString stamp(IdString ref) ध्रुव;
	शून्य append_str(IdString ref, std::string& out) ध्रुव;
	IdString auto_prefix(ध्रुव std::string *prefix);
	IdString add_inner(Twine_ग्रन्थिः t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ध्रुव;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ध्रुव;
	IdString find_content(uint32_t prefix, std::string_view text) ध्रुव;
	स्थिर size_t next_serial();
	dict<ध्रुव std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
