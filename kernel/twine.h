#నిర్వచితం_కాకపోతే YOSYS_TWINE_H
#నిర్వచించు YOSYS_TWINE_H

#అన్తర్భావ "kernel/hashcons.h"
#అన్తర్భావ "kernel/yosys_common.h"

#అన్తర్భావ <algorithm>
#అన్తర్భావ <bit>
#అన్తర్భావ <cstdint>
#అన్తర్భావ <cstring>
#అన్తర్భావ <deque>
#అన్తర్భావ <limits>
#అన్తర్భావ <span>
#అన్తర్భావ <string>
#అన్తర్భావ <string_view>
#అన్తర్భావ <unordered_set>
#అన్తర్భావ <variant>
#అన్తర్భావ <vector>

YOSYS_NAMESPACE_BEGIN

నిర్మాణం TwineSpec;
నిర్మాణం TwinePool;
నిర్మాణం IdString;

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

నిర్మాణం NullIdString {
	constexpr క్రియాకారకం IdString() స్థిరరాశి;
	constexpr bool క్రియాకారకం==(IdString ref) స్థిరరాశి;
};

నిర్మాణం IdString {
అంతరంగ:
#నిర్వచితమైతే YOSYS_ENABLE_TWINE_PROVENANCE
	using బిట్లు_t = uint64_t;
#లేకపోతే
	using బిట్లు_t = uint32_t;
#అయితే_ముగింపు
	బిట్లు_t విలువ;

	స్థిర constexpr పూర్ణాంకం TWINE_వెడల్పు = std::numeric_limits<బిట్లు_t>::digits;
	స్థిర constexpr పూర్ణాంకం TWINE_ప్రజా_జరుపు = TWINE_వెడల్పు - 1;
	స్థిర constexpr బిట్లు_t TWINE_ప్రజా_బిట్టు = బిట్లు_t{1} << TWINE_ప్రజా_జరుపు;
	స్థిర constexpr బిట్లు_t TWINE_NULL_VAL = ~బిట్లు_t{0};
	స్థిర constexpr పూర్ణాంకం TWINE_SERIAL_జరుపు = TWINE_వెడల్పు == 64 ? 32 : TWINE_ప్రజా_జరుపు;
	స్థిర constexpr బిట్లు_t TWINE_సూచీ_ముసుగు = (బిట్లు_t{1} << TWINE_SERIAL_జరుపు) - 1;
	స్థిర constexpr బిట్లు_t TWINE_SERIAL_ముసుగు = బిట్లు_t(~(TWINE_సూచీ_ముసుగు | TWINE_ప్రజా_బిట్టు));

బహిరంగ:
	స్థిర constexpr NullIdString Null{};

	స్థిర constexpr కొలత_t MAX_సూచీ = TWINE_సూచీ_ముసుగు;
	స్థిర constexpr కొలత_t MAX_SERIAL = TWINE_SERIAL_ముసుగు >> TWINE_SERIAL_జరుపు;

	constexpr కొలత_t raw() స్థిరరాశి {
		తిరిగి విలువ == TWINE_NULL_VAL ? విలువ : (విలువ & ~TWINE_SERIAL_ముసుగు);
	}

	// raw() with the pool serial kept
	constexpr కొలత_t బిట్లు() స్థిరరాశి { తిరిగి విలువ; }

	constexpr కొలత_t క్రమం_కీలకం() స్థిరరాశి { తిరిగి std::rotl((బిట్లు_t) eq_కీలకం(), 1); }

	constexpr కొలత_t serial() స్థిరరాశి {
		తిరిగి విలువ == TWINE_NULL_VAL ? 0 : ((విలువ & TWINE_SERIAL_ముసుగు) >> TWINE_SERIAL_జరుపు);
	}

	constexpr IdString stamped(కొలత_t pool_serial) స్థిరరాశి {
		తిరిగి విలువ == TWINE_NULL_VAL ? *this
			: IdString((విలువ & ~TWINE_SERIAL_ముసుగు)
					| (బిట్లు_t(pool_serial << TWINE_SERIAL_జరుపు) & TWINE_SERIAL_ముసుగు));
	}

	constexpr IdString() : విలువ(TWINE_NULL_VAL) {}
	explicit constexpr IdString(కొలత_t val) : విలువ((బిట్లు_t) val) {}

	constexpr కొలత_t eq_కీలకం() స్థిరరాశి { తిరిగి విలువ & ~TWINE_SERIAL_ముసుగు; }

	constexpr bool క్రియాకారకం==(స్థిరరాశి IdString &rhs) స్థిరరాశి { తిరిగి eq_కీలకం() == rhs.eq_కీలకం(); }
	constexpr bool క్రియాకారకం!=(స్థిరరాశి IdString &rhs) స్థిరరాశి { తిరిగి eq_కీలకం() != rhs.eq_కీలకం(); }
	constexpr std::strong_ordering క్రియాకారకం<=>(స్థిరరాశి IdString &rhs) స్థిరరాశి {
		తిరిగి క్రమం_కీలకం() <=> rhs.క్రమం_కీలకం();
	}

	మూస <typename... వాదనలు_3>
	constexpr bool లోపల(స్థిరరాశి వాదనలు_3&... వాదనలు_2) స్థిరరాశి {
		తిరిగి (లోపల_one(వాదనలు_2) || ...);
	}
	bool లోపల(స్థిరరాశి pool<IdString> &rhs) స్థిరరాశి;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	మూస<typename T> నిర్మాణం compare_ptr_by_name {
		bool క్రియాకారకం()(స్థిరరాశి T *a, స్థిరరాశి T *b) స్థిరరాశి {
			తిరిగి (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool ఖాళీలు() స్థిరరాశి { తిరిగి విలువ == TWINE_NULL_VAL; }
	constexpr శూన్య clear() { విలువ = TWINE_NULL_VAL; }
	constexpr bool is_ప్రజా() స్థిరరాశి { తిరిగి విలువ != TWINE_NULL_VAL && (విలువ & TWINE_ప్రజా_బిట్టు); }

	constexpr IdString untag() స్థిరరాశి {
		తిరిగి విలువ == TWINE_NULL_VAL ? *this : IdString(విలువ & ~TWINE_ప్రజా_బిట్టు);
	}
	constexpr IdString ట్యాగ్(bool pub) స్థిరరాశి {
		తిరిగి విలువ == TWINE_NULL_VAL ? *this : IdString(pub ? (విలువ | TWINE_ప్రజా_బిట్టు) : (విలువ & ~TWINE_ప్రజా_బిట్టు));
	}

	std::string handle_token() స్థిరరాశి;
	స్థిర కొలత_t handle_token_ఉపసర్గ(std::string_view token, bool &is_ప్రజా_2);

అంతరంగ:
	constexpr bool లోపల_one(IdString rhs) స్థిరరాశి { తిరిగి *this == rhs; }
	bool లోపల_one(స్థిరరాశి pool<IdString> &rhs) స్థిరరాశి;
};

constexpr NullIdString::క్రియాకారకం IdString() స్థిరరాశి { తిరిగి IdString(); }
constexpr bool NullIdString::క్రియాకారకం==(IdString ref) స్థిరరాశి { తిరిగి ref.ఖాళీలు(); }

namespace hashlib {
	మూస<>
	నిర్మాణం hash_ops<IdString> {
		స్థిర అంతఃవరుస bool cmp(IdString a, IdString b) { తిరిగి a == b; }
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_కీలకం());
			తిరిగి h;
		}
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_కీలకం());
			తిరిగి h;
		}
	};
}

అంతఃవరుస bool IdString::లోపల_one(స్థిరరాశి pool<IdString> &rhs) స్థిరరాశి { తిరిగి rhs.లెక్క(*this) != 0; }
అంతఃవరుస bool IdString::లోపల(స్థిరరాశి pool<IdString> &rhs) స్థిరరాశి { తిరిగి లోపల_one(rhs); }


గణన : పొట్టి {
#నిర్వచించు X(N) IDX_##N,
#అన్తర్భావ "kernel/constids.inc"
#నిర్వచనం_రద్దు X
	STATIC_TWINE_చివర
};

నిర్మాణం ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#నిర్వచించు X(N) స్థిర constexpr IdString N = IdString(IDX_##N).ట్యాగ్((#N)[0] != '$');
#అన్తర్భావ "kernel/constids.inc"
#నిర్వచనం_రద్దు X

	స్థిర constexpr స్థిరరాశి అక్షరం* static_పేర్లు[] = {
#నిర్వచించు X(N) #N,
#అన్తర్భావ "kernel/constids.inc"
#నిర్వచనం_రద్దు X
	};

	స్థిర constexpr IdString lookup(std::string_view name)
	{
		పూర్ణాంకం దిగువ = 0, ఎగువ = STATIC_TWINE_చివర;
		అయితే (ఎగువ - దిగువ >= 2) {
			పూర్ణాంకం mid = (దిగువ + ఎగువ) / 2;
			ఒకవేళ (name < static_పేర్లు[mid])
				ఎగువ = mid;
			లేకపోతే
				దిగువ = mid;
		}

		ఒకవేళ (name != static_పేర్లు[దిగువ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		తిరిగి IdString(దిగువ).ట్యాగ్(name[0] != '$');
	}

	స్థిర constexpr bool is_static(IdString ref) {
		తిరిగి ref.untag().raw() < STATIC_TWINE_చివర;
	}

	స్థిర constexpr IdString static_ref(కొలత_t raw) {
		ఒకవేళ (!is_static(IdString(raw)))
			throw "not a static twine id";
		తిరిగి IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	స్థిర std::string str(IdString ref);
	స్థిర std::string unescaped_str(IdString ref);
};

మూస<కొలత_t Raw> అంతఃవరుస constexpr IdString constid = ID::static_ref(Raw);

#నిర్వచించు ID(id) (YOSYS_NAMESPACE_ఉపసర్గ constid<YOSYS_NAMESPACE_ఉపసర్గ ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
నిర్మాణం TwineSpec {
	// deduplicates shared prefixes
	నిర్మాణం Suffix {
		IdString ఉపసర్గ_2;
		std::string తోక;
		స్వయంచాలక క్రియాకారకం<=>(స్థిరరాశి Suffix&) స్థిరరాశి = అప్రమేయ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	నిర్మాణం AutoSuffix {
		స్థిరరాశి std::string *ఉపసర్గ_2;
		std::string తోక;
		స్వయంచాలక క్రియాకారకం<=>(స్థిరరాశి AutoSuffix&) స్థిరరాశి = అప్రమేయ;
	};

	// "leaf", regular deduplicated string
	నిర్మాణం ఆకు {
		std::string s;
		స్వయంచాలక క్రియాకారకం<=>(స్థిరరాశి ఆకు&) స్థిరరాశి = అప్రమేయ;
	};

	std::variant<ఆకు, Suffix, AutoSuffix> దత్తాంశం;

	TwineSpec(ఆకు v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() స్థిరరాశి;
};

నిర్మాణం SmallString {
	స్థిర constexpr uint32_t INLINE_CAP = 8;
	స్థిర constexpr కొలత_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = అప్రమేయ;
	explicit SmallString(std::string_view content);
	SmallString(స్థిరరాశి SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &క్రియాకారకం=(స్థిరరాశి SmallString &other);
	SmallString &క్రియాకారకం=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() స్థిరరాశి { తిరిగి {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

అంతరంగ:
	శూన్య store(std::string_view content);
	శూన్య release();

	సంఘం {
		అక్షరం inl_[INLINE_CAP];
		అక్షరం *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
నిర్మాణం Twine_కణుపు {
	స్థిర constexpr uint32_t NO_ఉపసర్గ = ~uint32_t{0};
	స్థిర constexpr uint32_t DEAD = NO_ఉపసర్గ - 1;

	నిర్మాణం కీలకం_2 {
		uint32_t ఉపసర్గ_2;
		std::string_view పాఠ్యం;
	};

	స్థిర constexpr uint32_t extend_hash(uint32_t hash, std::string_view తోక) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		కొరకు (అక్షరం c : తోక)
			hash = ((hash << 5) + hash) + static_cast<గుర్తులేని అక్షరం>(c);
		తిరిగి hash;
	}

	Twine_కణుపు() = అప్రమేయ;
	explicit Twine_కణుపు(std::string_view content)
		: పాఠ్యం_2(content), ఉపసర్గ_3(NO_ఉపసర్గ), hash_(extend_hash(0, content)) {}
	Twine_కణుపు(uint32_t ఉపసర్గ_2, std::string_view తోక, uint32_t hash)
		: పాఠ్యం_2(తోక), ఉపసర్గ_3(ఉపసర్గ_2), hash_(hash) {}
	Twine_కణుపు(స్థిరరాశి Twine_కణుపు &other) = అప్రమేయ;
	Twine_కణుపు(Twine_కణుపు &&other) noexcept;
	Twine_కణుపు &క్రియాకారకం=(స్థిరరాశి Twine_కణుపు &other) = అప్రమేయ;
	Twine_కణుపు &క్రియాకారకం=(Twine_కణుపు &&other) noexcept;
	~Twine_కణుపు() = అప్రమేయ;

	గణన class రకం_2 { Dead, ఆకు, Suffix };

	constexpr bool is_dead() స్థిరరాశి { తిరిగి ఉపసర్గ_3 == DEAD; }
	constexpr bool is_ఆకు() స్థిరరాశి { తిరిగి ఉపసర్గ_3 == NO_ఉపసర్గ; }
	constexpr bool is_suffix() స్థిరరాశి { తిరిగి ఉపసర్గ_3 < DEAD; }

	constexpr రకం_2 రకం() స్థిరరాశి {
		తిరిగి ఉపసర్గ_3 == DEAD ? రకం_2::Dead
			: ఉపసర్గ_3 == NO_ఉపసర్గ ? రకం_2::ఆకు
			: రకం_2::Suffix;
	}

	std::string_view పాఠ్యం() స్థిరరాశి { తిరిగి పాఠ్యం_2.view(); }
	IdString ఉపసర్గ_2() స్థిరరాశి { తిరిగి IdString(ఉపసర్గ_3); }
	uint32_t hash() స్థిరరాశి { తిరిగి hash_; }
	కీలకం_2 కీలకం() స్థిరరాశి { తిరిగి {ఉపసర్గ_3, పాఠ్యం()}; }

	bool క్రియాకారకం==(స్థిరరాశి Twine_కణుపు &other) స్థిరరాశి
		{ తిరిగి ఉపసర్గ_3 == other.ఉపసర్గ_3 && పాఠ్యం() == other.పాఠ్యం(); }
	bool క్రియాకారకం==(స్థిరరాశి కీలకం_2 &other) స్థిరరాశి
		{ తిరిగి ఉపసర్గ_3 == other.ఉపసర్గ_2 && పాఠ్యం() == other.పాఠ్యం; }

అంతరంగ:
	SmallString పాఠ్యం_2;
	uint32_t ఉపసర్గ_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(పరిమాణం(Twine_కణుపు) == (పరిమాణం(శూన్య *) == 8 ? 24 : 20));

నిర్మాణం StaticTwines {
	స్థిర శూన్య init();
	స్థిర స్థిరరాశి Twine_కణుపు &కణుపు(కొలత_t idx);
	స్థిర bool ready();

అంతరంగ:
	స్థిర std::సదిశ<Twine_కణుపు> nodes_;
};

బాహ్య int64_t twine_gc_ns;
బాహ్య పూర్ణాంకం twine_gc_లెక్క;

నిర్మాణం TwinePool : HashConsPool<TwinePool, Twine_కణుపు, IdString> {
	స్థిర constexpr కొలత_t STATIC_లెక్క = STATIC_TWINE_చివర;

	TwinePool();
	TwinePool(స్థిరరాశి TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& క్రియాకారకం=(స్థిరరాశి TwinePool& other);
	TwinePool& క్రియాకారకం=(TwinePool&& other);

	స్థిరరాశి Twine_కణుపు& క్రియాకారకం[](IdString ref) స్థిరరాశి;

	స్థిర స్థిరరాశి Twine_కణుపు& static_కణుపు(కొలత_t idx);

	స్థిర శూన్య check_ready();
	స్థిర uint32_t hash_కణుపు(స్థిరరాశి Twine_కణుపు& t) { తిరిగి t.hash(); }

	మూస<typename F>
	స్థిర శూన్య for_each_బిడ్డ(స్థిరరాశి Twine_కణుపు& t, F&& f) {
		ఒకవేళ (t.is_suffix())
			f(t.ఉపసర్గ_2());
	}

	కొలత_t gc(pool<IdString>& roots);

	శూన్య dump(IdString ref, std::ostream& os = std::cout) స్థిరరాశి;

	// Publicity bit provides escaping
	std::string str(IdString ref) స్థిరరాశి;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) స్థిరరాశి;

	కొలత_t str_కొలత(IdString ref) స్థిరరాశి;
	bool begins_with(IdString ref, std::string_view ఉపసర్గ_2) స్థిరరాశి;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) స్థిరరాశి;
	పూర్ణాంకం compare_by_name(IdString a, IdString b) స్థిరరాశి;
	IdString ఉపసర్గ_యొక్క(IdString ref) స్థిరరాశి;

	IdString కనుగొను(std::string_view name) స్థిరరాశి;
	IdString కనుగొను(TwineSpec t) స్థిరరాశి;
	// Doesn't infer publicity
	IdString కూడు(TwineSpec t);
	IdString కూడు(IdString ఉపసర్గ_2, std::string_view తోక);
	// Infers publicity from first character
	IdString కూడు(std::string_view s);
	IdString copy_from(స్థిరరాశి TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString కనుగొను_from(స్థిరరాశి TwinePool& src, IdString ref) స్థిరరాశి;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) స్థిరరాశి;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) స్థిరరాశి;
	శూన్య check_owned(IdString ref) స్థిరరాశి;

అంతరంగ:
	bool owns(IdString ref) స్థిరరాశి;
	IdString stamp(IdString ref) స్థిరరాశి;
	శూన్య append_str(IdString ref, std::string& బహిర్గతం) స్థిరరాశి;
	IdString auto_ఉపసర్గ(స్థిరరాశి std::string *ఉపసర్గ_2);
	IdString కూడు_inner(Twine_కణుపు t);
	IdString intern(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం);
	uint32_t content_hash(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం) స్థిరరాశి;
	IdString కనుగొను_content(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం, uint32_t hash) స్థిరరాశి;
	IdString కనుగొను_content(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం) స్థిరరాశి;
	స్థిర కొలత_t తదుపరి_serial();
	dict<స్థిరరాశి std::string *, IdString> auto_prefixes;
	కొలత_t serial_;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
