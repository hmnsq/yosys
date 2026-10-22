#यदि_अपरिभाषितम् YOSYS_TWINE_H
#परिभाषय YOSYS_TWINE_H

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
	constexpr संक्रियकः IdString() ध्रुव;
	constexpr bool संक्रियकः==(IdString ref) ध्रुव;
};

संरचना IdString {
गुप्त:
#यदि_परिभाषितम् YOSYS_ENABLE_TWINE_PROVENANCE
	using द्वयङ्काः_t = uint64_t;
#अन्यथा
	using द्वयङ्काः_t = uint32_t;
#यदि_अन्तः
	द्वयङ्काः_t मानम्;

	स्थिर constexpr पूर्णाङ्क TWINE_विस्तारः = std::numeric_limits<द्वयङ्काः_t>::digits;
	स्थिर constexpr पूर्णाङ्क TWINE_लौकिकम्_सरणम् = TWINE_विस्तारः - 1;
	स्थिर constexpr द्वयङ्काः_t TWINE_लौकिकम्_BIT = द्वयङ्काः_t{1} << TWINE_लौकिकम्_सरणम्;
	स्थिर constexpr द्वयङ्काः_t TWINE_NULL_VAL = ~द्वयङ्काः_t{0};
	स्थिर constexpr पूर्णाङ्क TWINE_SERIAL_सरणम् = TWINE_विस्तारः == 64 ? 32 : TWINE_लौकिकम्_सरणम्;
	स्थिर constexpr द्वयङ्काः_t TWINE_सूचकांक_आवरणम् = (द्वयङ्काः_t{1} << TWINE_SERIAL_सरणम्) - 1;
	स्थिर constexpr द्वयङ्काः_t TWINE_SERIAL_आवरणम् = द्वयङ्काः_t(~(TWINE_सूचकांक_आवरणम् | TWINE_लौकिकम्_BIT));

सार्वजनिक:
	स्थिर constexpr NullIdString Null{};

	स्थिर constexpr आकार_t MAX_सूचकांक = TWINE_सूचकांक_आवरणम्;
	स्थिर constexpr आकार_t MAX_SERIAL = TWINE_SERIAL_आवरणम् >> TWINE_SERIAL_सरणम्;

	constexpr आकार_t raw() ध्रुव {
		प्रत्यागम मानम् == TWINE_NULL_VAL ? मानम् : (मानम् & ~TWINE_SERIAL_आवरणम्);
	}

	// raw() with the pool serial kept
	constexpr आकार_t द्वयङ्काः() ध्रुव { प्रत्यागम मानम्; }

	constexpr आकार_t क्रमः_कुञ्चिका() ध्रुव { प्रत्यागम std::rotl((द्वयङ्काः_t) eq_कुञ्चिका(), 1); }

	constexpr आकार_t serial() ध्रुव {
		प्रत्यागम मानम् == TWINE_NULL_VAL ? 0 : ((मानम् & TWINE_SERIAL_आवरणम्) >> TWINE_SERIAL_सरणम्);
	}

	constexpr IdString stamped(आकार_t pool_serial) ध्रुव {
		प्रत्यागम मानम् == TWINE_NULL_VAL ? *this
			: IdString((मानम् & ~TWINE_SERIAL_आवरणम्)
					| (द्वयङ्काः_t(pool_serial << TWINE_SERIAL_सरणम्) & TWINE_SERIAL_आवरणम्));
	}

	constexpr IdString() : मानम्(TWINE_NULL_VAL) {}
	explicit constexpr IdString(आकार_t val) : मानम्((द्वयङ्काः_t) val) {}

	constexpr आकार_t eq_कुञ्चिका() ध्रुव { प्रत्यागम मानम् & ~TWINE_SERIAL_आवरणम्; }

	constexpr bool संक्रियकः==(ध्रुव IdString &rhs) ध्रुव { प्रत्यागम eq_कुञ्चिका() == rhs.eq_कुञ्चिका(); }
	constexpr bool संक्रियकः!=(ध्रुव IdString &rhs) ध्रुव { प्रत्यागम eq_कुञ्चिका() != rhs.eq_कुञ्चिका(); }
	constexpr std::strong_ordering संक्रियकः<=>(ध्रुव IdString &rhs) ध्रुव {
		प्रत्यागम क्रमः_कुञ्चिका() <=> rhs.क्रमः_कुञ्चिका();
	}

	प्रतिरूपम् <typename... तर्काः_3>
	constexpr bool अन्तर्(ध्रुव तर्काः_3&... तर्काः_2) ध्रुव {
		प्रत्यागम (अन्तर्_one(तर्काः_2) || ...);
	}
	bool अन्तर्(ध्रुव pool<IdString> &rhs) ध्रुव;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	प्रतिरूपम्<typename T> संरचना compare_ptr_by_name {
		bool संक्रियकः()(ध्रुव T *a, ध्रुव T *b) ध्रुव {
			प्रत्यागम (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool रिक्तानि() ध्रुव { प्रत्यागम मानम् == TWINE_NULL_VAL; }
	constexpr शून्य clear() { मानम् = TWINE_NULL_VAL; }
	constexpr bool is_लौकिकम्() ध्रुव { प्रत्यागम मानम् != TWINE_NULL_VAL && (मानम् & TWINE_लौकिकम्_BIT); }

	constexpr IdString untag() ध्रुव {
		प्रत्यागम मानम् == TWINE_NULL_VAL ? *this : IdString(मानम् & ~TWINE_लौकिकम्_BIT);
	}
	constexpr IdString चिह्नकम्(bool pub) ध्रुव {
		प्रत्यागम मानम् == TWINE_NULL_VAL ? *this : IdString(pub ? (मानम् | TWINE_लौकिकम्_BIT) : (मानम् & ~TWINE_लौकिकम्_BIT));
	}

	std::string handle_token() ध्रुव;
	स्थिर आकार_t handle_token_उपसर्गः(std::string_view token, bool &is_लौकिकम्_2);

गुप्त:
	constexpr bool अन्तर्_one(IdString rhs) ध्रुव { प्रत्यागम *this == rhs; }
	bool अन्तर्_one(ध्रुव pool<IdString> &rhs) ध्रुव;
};

constexpr NullIdString::संक्रियकः IdString() ध्रुव { प्रत्यागम IdString(); }
constexpr bool NullIdString::संक्रियकः==(IdString ref) ध्रुव { प्रत्यागम ref.रिक्तानि(); }

namespace hashlib {
	प्रतिरूपम्<>
	संरचना hash_ops<IdString> {
		स्थिर अन्तःस्थ bool cmp(IdString a, IdString b) { प्रत्यागम a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_कुञ्चिका());
			प्रत्यागम h;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_कुञ्चिका());
			प्रत्यागम h;
		}
	};
}

अन्तःस्थ bool IdString::अन्तर्_one(ध्रुव pool<IdString> &rhs) ध्रुव { प्रत्यागम rhs.गणनम्(*this) != 0; }
अन्तःस्थ bool IdString::अन्तर्(ध्रुव pool<IdString> &rhs) ध्रुव { प्रत्यागम अन्तर्_one(rhs); }


गणना : ह्रस्व {
#परिभाषय X(N) IDX_##N,
#अन्तर्भाव "kernel/constids.inc"
#अपरिभाषय X
	STATIC_TWINE_अन्तः
};

संरचना ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#परिभाषय X(N) स्थिर constexpr IdString N = IdString(IDX_##N).चिह्नकम्((#N)[0] != '$');
#अन्तर्भाव "kernel/constids.inc"
#अपरिभाषय X

	स्थिर constexpr ध्रुव वर्ण* static_नामानि[] = {
#परिभाषय X(N) #N,
#अन्तर्भाव "kernel/constids.inc"
#अपरिभाषय X
	};

	स्थिर constexpr IdString lookup(std::string_view name)
	{
		पूर्णाङ्क नीचः = 0, उच्चः = STATIC_TWINE_अन्तः;
		यावत् (उच्चः - नीचः >= 2) {
			पूर्णाङ्क mid = (नीचः + उच्चः) / 2;
			यदि (name < static_नामानि[mid])
				उच्चः = mid;
			अन्यथा
				नीचः = mid;
		}

		यदि (name != static_नामानि[नीचः])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		प्रत्यागम IdString(नीचः).चिह्नकम्(name[0] != '$');
	}

	स्थिर constexpr bool is_static(IdString ref) {
		प्रत्यागम ref.untag().raw() < STATIC_TWINE_अन्तः;
	}

	स्थिर constexpr IdString static_ref(आकार_t raw) {
		यदि (!is_static(IdString(raw)))
			throw "not a static twine id";
		प्रत्यागम IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	स्थिर std::string str(IdString ref);
	स्थिर std::string unescaped_str(IdString ref);
};

प्रतिरूपम्<आकार_t Raw> अन्तःस्थ constexpr IdString constid = ID::static_ref(Raw);

#परिभाषय ID(id) (YOSYS_NAMESPACE_उपसर्गः constid<YOSYS_NAMESPACE_उपसर्गः ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
संरचना TwineSpec {
	// deduplicates shared prefixes
	संरचना Suffix {
		IdString उपसर्गः_2;
		std::string पुच्छ;
		स्वचालित संक्रियकः<=>(ध्रुव Suffix&) ध्रुव = स्वभाव;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	संरचना AutoSuffix {
		ध्रुव std::string *उपसर्गः_2;
		std::string पुच्छ;
		स्वचालित संक्रियकः<=>(ध्रुव AutoSuffix&) ध्रुव = स्वभाव;
	};

	// "leaf", regular deduplicated string
	संरचना पर्णम् {
		std::string s;
		स्वचालित संक्रियकः<=>(ध्रुव पर्णम्&) ध्रुव = स्वभाव;
	};

	std::variant<पर्णम्, Suffix, AutoSuffix> दत्त;

	TwineSpec(पर्णम् v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ध्रुव;
};

संरचना SmallString {
	स्थिर constexpr uint32_t INLINE_CAP = 8;
	स्थिर constexpr आकार_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = स्वभाव;
	explicit SmallString(std::string_view content);
	SmallString(ध्रुव SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &संक्रियकः=(ध्रुव SmallString &other);
	SmallString &संक्रियकः=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ध्रुव { प्रत्यागम {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

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
	स्थिर constexpr uint32_t NO_उपसर्गः = ~uint32_t{0};
	स्थिर constexpr uint32_t DEAD = NO_उपसर्गः - 1;

	संरचना कुञ्चिका_2 {
		uint32_t उपसर्गः_2;
		std::string_view text;
	};

	स्थिर constexpr uint32_t extend_hash(uint32_t hash, std::string_view पुच्छ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		प्रति (वर्ण c : पुच्छ)
			hash = ((hash << 5) + hash) + static_cast<अचिह्नित वर्ण>(c);
		प्रत्यागम hash;
	}

	Twine_ग्रन्थिः() = स्वभाव;
	explicit Twine_ग्रन्थिः(std::string_view content)
		: text_(content), उपसर्गः_3(NO_उपसर्गः), hash_(extend_hash(0, content)) {}
	Twine_ग्रन्थिः(uint32_t उपसर्गः_2, std::string_view पुच्छ, uint32_t hash)
		: text_(पुच्छ), उपसर्गः_3(उपसर्गः_2), hash_(hash) {}
	Twine_ग्रन्थिः(ध्रुव Twine_ग्रन्थिः &other) = स्वभाव;
	Twine_ग्रन्थिः(Twine_ग्रन्थिः &&other) noexcept;
	Twine_ग्रन्थिः &संक्रियकः=(ध्रुव Twine_ग्रन्थिः &other) = स्वभाव;
	Twine_ग्रन्थिः &संक्रियकः=(Twine_ग्रन्थिः &&other) noexcept;
	~Twine_ग्रन्थिः() = स्वभाव;

	गणना class विधा_2 { Dead, पर्णम्, Suffix };

	constexpr bool is_dead() ध्रुव { प्रत्यागम उपसर्गः_3 == DEAD; }
	constexpr bool is_पर्णम्() ध्रुव { प्रत्यागम उपसर्गः_3 == NO_उपसर्गः; }
	constexpr bool is_suffix() ध्रुव { प्रत्यागम उपसर्गः_3 < DEAD; }

	constexpr विधा_2 विधा() ध्रुव {
		प्रत्यागम उपसर्गः_3 == DEAD ? विधा_2::Dead
			: उपसर्गः_3 == NO_उपसर्गः ? विधा_2::पर्णम्
			: विधा_2::Suffix;
	}

	std::string_view text() ध्रुव { प्रत्यागम text_.view(); }
	IdString उपसर्गः_2() ध्रुव { प्रत्यागम IdString(उपसर्गः_3); }
	uint32_t hash() ध्रुव { प्रत्यागम hash_; }
	कुञ्चिका_2 कुञ्चिका() ध्रुव { प्रत्यागम {उपसर्गः_3, text()}; }

	bool संक्रियकः==(ध्रुव Twine_ग्रन्थिः &other) ध्रुव
		{ प्रत्यागम उपसर्गः_3 == other.उपसर्गः_3 && text() == other.text(); }
	bool संक्रियकः==(ध्रुव कुञ्चिका_2 &other) ध्रुव
		{ प्रत्यागम उपसर्गः_3 == other.उपसर्गः_2 && text() == other.text; }

गुप्त:
	SmallString text_;
	uint32_t उपसर्गः_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(परिमाण(Twine_ग्रन्थिः) == (परिमाण(शून्य *) == 8 ? 24 : 20));

संरचना StaticTwines {
	स्थिर शून्य init();
	स्थिर ध्रुव Twine_ग्रन्थिः &ग्रन्थिः(आकार_t idx);
	स्थिर bool ready();

गुप्त:
	स्थिर std::सदिशम्<Twine_ग्रन्थिः> nodes_;
};

बाह्य int64_t twine_gc_ns;
बाह्य पूर्णाङ्क twine_gc_गणनम्;

संरचना TwinePool : HashConsPool<TwinePool, Twine_ग्रन्थिः, IdString> {
	स्थिर constexpr आकार_t STATIC_गणनम् = STATIC_TWINE_अन्तः;

	TwinePool();
	TwinePool(ध्रुव TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& संक्रियकः=(ध्रुव TwinePool& other);
	TwinePool& संक्रियकः=(TwinePool&& other);

	ध्रुव Twine_ग्रन्थिः& संक्रियकः[](IdString ref) ध्रुव;

	स्थिर ध्रुव Twine_ग्रन्थिः& static_ग्रन्थिः(आकार_t idx);

	स्थिर शून्य check_ready();
	स्थिर uint32_t hash_ग्रन्थिः(ध्रुव Twine_ग्रन्थिः& t) { प्रत्यागम t.hash(); }

	प्रतिरूपम्<typename F>
	स्थिर शून्य for_each_child(ध्रुव Twine_ग्रन्थिः& t, F&& f) {
		यदि (t.is_suffix())
			f(t.उपसर्गः_2());
	}

	आकार_t gc(pool<IdString>& roots);

	शून्य dump(IdString ref, std::ostream& os = std::cout) ध्रुव;

	// Publicity bit provides escaping
	std::string str(IdString ref) ध्रुव;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ध्रुव;

	आकार_t str_आकार(IdString ref) ध्रुव;
	bool begins_with(IdString ref, std::string_view उपसर्गः_2) ध्रुव;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ध्रुव;
	पूर्णाङ्क compare_by_name(IdString a, IdString b) ध्रुव;
	IdString उपसर्गः_सम्बन्धि(IdString ref) ध्रुव;

	IdString find(std::string_view name) ध्रुव;
	IdString find(TwineSpec t) ध्रुव;
	// Doesn't infer publicity
	IdString सङ्कलय(TwineSpec t);
	IdString सङ्कलय(IdString उपसर्गः_2, std::string_view पुच्छ);
	// Infers publicity from first character
	IdString सङ्कलय(std::string_view s);
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
	शून्य append_str(IdString ref, std::string& बहिर्गमनम्) ध्रुव;
	IdString auto_उपसर्गः(ध्रुव std::string *उपसर्गः_2);
	IdString सङ्कलय_inner(Twine_ग्रन्थिः t);
	IdString intern(uint32_t उपसर्गः_2, std::string_view text);
	uint32_t content_hash(uint32_t उपसर्गः_2, std::string_view text) ध्रुव;
	IdString find_content(uint32_t उपसर्गः_2, std::string_view text, uint32_t hash) ध्रुव;
	IdString find_content(uint32_t उपसर्गः_2, std::string_view text) ध्रुव;
	स्थिर आकार_t अग्र_serial();
	dict<ध्रुव std::string *, IdString> auto_prefixes;
	आकार_t serial_;
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
