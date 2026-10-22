#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_TWINE_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_TWINE_H

#ಅನ್ತರ್ಭಾವ "kernel/hashcons.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <bit>
#ಅನ್ತರ್ಭಾವ <cstdint>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <limits>
#ಅನ್ತರ್ಭಾವ <span>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <unordered_set>
#ಅನ್ತರ್ಭಾವ <variant>
#ಅನ್ತರ್ಭಾವ <vector>

YOSYS_NAMESPACE_BEGIN

ರಚನೆ TwineSpec;
ರಚನೆ TwinePool;
ರಚನೆ IdString;

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

ರಚನೆ NullIdString {
	constexpr ಕ್ರಿಯಾಕಾರಕ IdString() ಸ್ಥಿರಾಂಕ;
	constexpr bool ಕ್ರಿಯಾಕಾರಕ==(IdString ref) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ IdString {
ಖಾಸಗಿ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TWINE_PROVENANCE
	using ಬಿಟ್ಟುಗಳು_t = uint64_t;
#ಇಲ್ಲದಿದ್ದರೆ
	using ಬಿಟ್ಟುಗಳು_t = uint32_t;
#ಆದರೆ_ಅಂತ್ಯ
	ಬಿಟ್ಟುಗಳು_t ಮೌಲ್ಯ;

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ TWINE_ಅಗಲ = std::numeric_limits<ಬಿಟ್ಟುಗಳು_t>::digits;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ TWINE_ಬಹಿರಂಗ_ಸರಿಸು = TWINE_ಅಗಲ - 1;
	ಸ್ಥಿರ constexpr ಬಿಟ್ಟುಗಳು_t TWINE_ಬಹಿರಂಗ_ಬಿಟ್ = ಬಿಟ್ಟುಗಳು_t{1} << TWINE_ಬಹಿರಂಗ_ಸರಿಸು;
	ಸ್ಥಿರ constexpr ಬಿಟ್ಟುಗಳು_t TWINE_NULL_VAL = ~ಬಿಟ್ಟುಗಳು_t{0};
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ TWINE_SERIAL_ಸರಿಸು = TWINE_ಅಗಲ == 64 ? 32 : TWINE_ಬಹಿರಂಗ_ಸರಿಸು;
	ಸ್ಥಿರ constexpr ಬಿಟ್ಟುಗಳು_t TWINE_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ = (ಬಿಟ್ಟುಗಳು_t{1} << TWINE_SERIAL_ಸರಿಸು) - 1;
	ಸ್ಥಿರ constexpr ಬಿಟ್ಟುಗಳು_t TWINE_SERIAL_ಮುಖವಾಡ = ಬಿಟ್ಟುಗಳು_t(~(TWINE_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ | TWINE_ಬಹಿರಂಗ_ಬಿಟ್));

ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ constexpr NullIdString Null{};

	ಸ್ಥಿರ constexpr ಗಾತ್ರ_t MAX_ಸೂಚ್ಯಂಕ = TWINE_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ;
	ಸ್ಥಿರ constexpr ಗಾತ್ರ_t MAX_SERIAL = TWINE_SERIAL_ಮುಖವಾಡ >> TWINE_SERIAL_ಸರಿಸು;

	constexpr ಗಾತ್ರ_t raw() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL ? ಮೌಲ್ಯ : (ಮೌಲ್ಯ & ~TWINE_SERIAL_ಮುಖವಾಡ);
	}

	// raw() with the pool serial kept
	constexpr ಗಾತ್ರ_t ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ; }

	constexpr ಗಾತ್ರ_t ಕ್ರಮ_ಕೀಲಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::rotl((ಬಿಟ್ಟುಗಳು_t) eq_ಕೀಲಿ(), 1); }

	constexpr ಗಾತ್ರ_t serial() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL ? 0 : ((ಮೌಲ್ಯ & TWINE_SERIAL_ಮುಖವಾಡ) >> TWINE_SERIAL_ಸರಿಸು);
	}

	constexpr IdString stamped(ಗಾತ್ರ_t pool_serial) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this
			: IdString((ಮೌಲ್ಯ & ~TWINE_SERIAL_ಮುಖವಾಡ)
					| (ಬಿಟ್ಟುಗಳು_t(pool_serial << TWINE_SERIAL_ಸರಿಸು) & TWINE_SERIAL_ಮುಖವಾಡ));
	}

	constexpr IdString() : ಮೌಲ್ಯ(TWINE_NULL_VAL) {}
	explicit constexpr IdString(ಗಾತ್ರ_t val) : ಮೌಲ್ಯ((ಬಿಟ್ಟುಗಳು_t) val) {}

	constexpr ಗಾತ್ರ_t eq_ಕೀಲಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ & ~TWINE_SERIAL_ಮುಖವಾಡ; }

	constexpr bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ IdString &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ eq_ಕೀಲಿ() == rhs.eq_ಕೀಲಿ(); }
	constexpr bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ IdString &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ eq_ಕೀಲಿ() != rhs.eq_ಕೀಲಿ(); }
	constexpr std::strong_ordering ಕ್ರಿಯಾಕಾರಕ<=>(ಸ್ಥಿರಾಂಕ IdString &rhs) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಕ್ರಮ_ಕೀಲಿ() <=> rhs.ಕ್ರಮ_ಕೀಲಿ();
	}

	ಮಾದರಿ <typename... ವಾದಗಳು_3>
	constexpr bool ಒಳ(ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3&... ವಾದಗಳು_2) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ (ಒಳ_one(ವಾದಗಳು_2) || ...);
	}
	bool ಒಳ(ಸ್ಥಿರಾಂಕ pool<IdString> &rhs) ಸ್ಥಿರಾಂಕ;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	ಮಾದರಿ<typename T> ರಚನೆ compare_ptr_by_name {
		bool ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ T *a, ಸ್ಥಿರಾಂಕ T *b) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL; }
	constexpr ಶೂನ್ಯ clear() { ಮೌಲ್ಯ = TWINE_NULL_VAL; }
	constexpr bool is_ಬಹಿರಂಗ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ != TWINE_NULL_VAL && (ಮೌಲ್ಯ & TWINE_ಬಹಿರಂಗ_ಬಿಟ್); }

	constexpr IdString untag() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this : IdString(ಮೌಲ್ಯ & ~TWINE_ಬಹಿರಂಗ_ಬಿಟ್);
	}
	constexpr IdString ಟ್ಯಾಗ್(bool pub) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ == TWINE_NULL_VAL ? *this : IdString(pub ? (ಮೌಲ್ಯ | TWINE_ಬಹಿರಂಗ_ಬಿಟ್) : (ಮೌಲ್ಯ & ~TWINE_ಬಹಿರಂಗ_ಬಿಟ್));
	}

	std::string handle_token() ಸ್ಥಿರಾಂಕ;
	ಸ್ಥಿರ ಗಾತ್ರ_t handle_token_ಪೂರ್ವಪ್ರತ್ಯಯ(std::string_view token, bool &is_ಬಹಿರಂಗ_2);

ಖಾಸಗಿ:
	constexpr bool ಒಳ_one(IdString rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *this == rhs; }
	bool ಒಳ_one(ಸ್ಥಿರಾಂಕ pool<IdString> &rhs) ಸ್ಥಿರಾಂಕ;
};

constexpr NullIdString::ಕ್ರಿಯಾಕಾರಕ IdString() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ IdString(); }
constexpr bool NullIdString::ಕ್ರಿಯಾಕಾರಕ==(IdString ref) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ref.ಖಾಲಿಗಳು(); }

namespace hashlib {
	ಮಾದರಿ<>
	ರಚನೆ hash_ops<IdString> {
		ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(IdString a, IdString b) { ಹಿಂತಿರುಗಿ a == b; }
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ಕೀಲಿ());
			ಹಿಂತಿರುಗಿ h;
		}
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ಕೀಲಿ());
			ಹಿಂತಿರುಗಿ h;
		}
	};
}

ಸಾಲೊಳಗೆ bool IdString::ಒಳ_one(ಸ್ಥಿರಾಂಕ pool<IdString> &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ rhs.ಎಣಿಕೆ_2(*this) != 0; }
ಸಾಲೊಳಗೆ bool IdString::ಒಳ(ಸ್ಥಿರಾಂಕ pool<IdString> &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಒಳ_one(rhs); }


ಎಣಿಕೆ : ಗಿಡ್ಡ {
#ವ್ಯಾಖ್ಯಾನಿಸು X(N) IDX_##N,
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು X
	STATIC_TWINE_ಕೊನೆ
};

ರಚನೆ ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#ವ್ಯಾಖ್ಯಾನಿಸು X(N) ಸ್ಥಿರ constexpr IdString N = IdString(IDX_##N).ಟ್ಯಾಗ್((#N)[0] != '$');
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು X

	ಸ್ಥಿರ constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* static_ಹೆಸರುಗಳು[] = {
#ವ್ಯಾಖ್ಯಾನಿಸು X(N) #N,
#ಅನ್ತರ್ಭಾವ "kernel/constids.inc"
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು X
	};

	ಸ್ಥಿರ constexpr IdString lookup(std::string_view name)
	{
		ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ = 0, ಮೇಲಿನ = STATIC_TWINE_ಕೊನೆ;
		ಆಗ (ಮೇಲಿನ - ಕೆಳಗಿನ >= 2) {
			ಪೂರ್ಣಾಂಕ mid = (ಕೆಳಗಿನ + ಮೇಲಿನ) / 2;
			ಆದರೆ (name < static_ಹೆಸರುಗಳು[mid])
				ಮೇಲಿನ = mid;
			ಇಲ್ಲದಿದ್ದರೆ
				ಕೆಳಗಿನ = mid;
		}

		ಆದರೆ (name != static_ಹೆಸರುಗಳು[ಕೆಳಗಿನ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		ಹಿಂತಿರುಗಿ IdString(ಕೆಳಗಿನ).ಟ್ಯಾಗ್(name[0] != '$');
	}

	ಸ್ಥಿರ constexpr bool is_static(IdString ref) {
		ಹಿಂತಿರುಗಿ ref.untag().raw() < STATIC_TWINE_ಕೊನೆ;
	}

	ಸ್ಥಿರ constexpr IdString static_ref(ಗಾತ್ರ_t raw) {
		ಆದರೆ (!is_static(IdString(raw)))
			throw "not a static twine id";
		ಹಿಂತಿರುಗಿ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ಸ್ಥಿರ std::string str(IdString ref);
	ಸ್ಥಿರ std::string unescaped_str(IdString ref);
};

ಮಾದರಿ<ಗಾತ್ರ_t Raw> ಸಾಲೊಳಗೆ constexpr IdString constid = ID::static_ref(Raw);

#ವ್ಯಾಖ್ಯಾನಿಸು ID(id) (YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ constid<YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ರಚನೆ TwineSpec {
	// deduplicates shared prefixes
	ರಚನೆ Suffix {
		IdString ಪೂರ್ವಪ್ರತ್ಯಯ_2;
		std::string ಬಾಲ;
		ಸ್ವಯಂಚಾಲಿತ ಕ್ರಿಯಾಕಾರಕ<=>(ಸ್ಥಿರಾಂಕ Suffix&) ಸ್ಥಿರಾಂಕ = ಪೂರ್ವನಿಯೋಜಿತ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ರಚನೆ AutoSuffix {
		ಸ್ಥಿರಾಂಕ std::string *ಪೂರ್ವಪ್ರತ್ಯಯ_2;
		std::string ಬಾಲ;
		ಸ್ವಯಂಚಾಲಿತ ಕ್ರಿಯಾಕಾರಕ<=>(ಸ್ಥಿರಾಂಕ AutoSuffix&) ಸ್ಥಿರಾಂಕ = ಪೂರ್ವನಿಯೋಜಿತ;
	};

	// "leaf", regular deduplicated string
	ರಚನೆ ಎಲೆ {
		std::string s;
		ಸ್ವಯಂಚಾಲಿತ ಕ್ರಿಯಾಕಾರಕ<=>(ಸ್ಥಿರಾಂಕ ಎಲೆ&) ಸ್ಥಿರಾಂಕ = ಪೂರ್ವನಿಯೋಜಿತ;
	};

	std::variant<ಎಲೆ, Suffix, AutoSuffix> ದತ್ತಾಂಶ;

	TwineSpec(ಎಲೆ v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ SmallString {
	ಸ್ಥಿರ constexpr uint32_t INLINE_CAP = 8;
	ಸ್ಥಿರ constexpr ಗಾತ್ರ_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = ಪೂರ್ವನಿಯೋಜಿತ;
	explicit SmallString(std::string_view content);
	SmallString(ಸ್ಥಿರಾಂಕ SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ SmallString &other);
	SmallString &ಕ್ರಿಯಾಕಾರಕ=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ಖಾಸಗಿ:
	ಶೂನ್ಯ store(std::string_view content);
	ಶೂನ್ಯ release();

	ಒಕ್ಕೂಟ {
		ಅಕ್ಷರ inl_[INLINE_CAP];
		ಅಕ್ಷರ *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ರಚನೆ Twine_ಗಂಟು {
	ಸ್ಥಿರ constexpr uint32_t NO_ಪೂರ್ವಪ್ರತ್ಯಯ = ~uint32_t{0};
	ಸ್ಥಿರ constexpr uint32_t DEAD = NO_ಪೂರ್ವಪ್ರತ್ಯಯ - 1;

	ರಚನೆ ಕೀಲಿ_2 {
		uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2;
		std::string_view ಪಠ್ಯ;
	};

	ಸ್ಥಿರ constexpr uint32_t extend_hash(uint32_t hash, std::string_view ಬಾಲ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		ಗಾಗಿ (ಅಕ್ಷರ c : ಬಾಲ)
			hash = ((hash << 5) + hash) + static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(c);
		ಹಿಂತಿರುಗಿ hash;
	}

	Twine_ಗಂಟು() = ಪೂರ್ವನಿಯೋಜಿತ;
	explicit Twine_ಗಂಟು(std::string_view content)
		: ಪಠ್ಯ_2(content), ಪೂರ್ವಪ್ರತ್ಯಯ_3(NO_ಪೂರ್ವಪ್ರತ್ಯಯ), hash_(extend_hash(0, content)) {}
	Twine_ಗಂಟು(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಬಾಲ, uint32_t hash)
		: ಪಠ್ಯ_2(ಬಾಲ), ಪೂರ್ವಪ್ರತ್ಯಯ_3(ಪೂರ್ವಪ್ರತ್ಯಯ_2), hash_(hash) {}
	Twine_ಗಂಟು(ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &other) = ಪೂರ್ವನಿಯೋಜಿತ;
	Twine_ಗಂಟು(Twine_ಗಂಟು &&other) noexcept;
	Twine_ಗಂಟು &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &other) = ಪೂರ್ವನಿಯೋಜಿತ;
	Twine_ಗಂಟು &ಕ್ರಿಯಾಕಾರಕ=(Twine_ಗಂಟು &&other) noexcept;
	~Twine_ಗಂಟು() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಎಣಿಕೆ class ಬಗೆ_2 { Dead, ಎಲೆ, Suffix };

	constexpr bool is_dead() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 == DEAD; }
	constexpr bool is_ಎಲೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 == NO_ಪೂರ್ವಪ್ರತ್ಯಯ; }
	constexpr bool is_suffix() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 < DEAD; }

	constexpr ಬಗೆ_2 ಬಗೆ() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 == DEAD ? ಬಗೆ_2::Dead
			: ಪೂರ್ವಪ್ರತ್ಯಯ_3 == NO_ಪೂರ್ವಪ್ರತ್ಯಯ ? ಬಗೆ_2::ಎಲೆ
			: ಬಗೆ_2::Suffix;
	}

	std::string_view ಪಠ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪಠ್ಯ_2.view(); }
	IdString ಪೂರ್ವಪ್ರತ್ಯಯ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ IdString(ಪೂರ್ವಪ್ರತ್ಯಯ_3); }
	uint32_t hash() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ hash_; }
	ಕೀಲಿ_2 ಕೀಲಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {ಪೂರ್ವಪ್ರತ್ಯಯ_3, ಪಠ್ಯ()}; }

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &other) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 == other.ಪೂರ್ವಪ್ರತ್ಯಯ_3 && ಪಠ್ಯ() == other.ಪಠ್ಯ(); }
	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &other) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ_3 == other.ಪೂರ್ವಪ್ರತ್ಯಯ_2 && ಪಠ್ಯ() == other.ಪಠ್ಯ; }

ಖಾಸಗಿ:
	SmallString ಪಠ್ಯ_2;
	uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(ಗಾತ್ರ(Twine_ಗಂಟು) == (ಗಾತ್ರ(ಶೂನ್ಯ *) == 8 ? 24 : 20));

ರಚನೆ StaticTwines {
	ಸ್ಥಿರ ಶೂನ್ಯ init();
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು &ಗಂಟು(ಗಾತ್ರ_t idx);
	ಸ್ಥಿರ bool ready();

ಖಾಸಗಿ:
	ಸ್ಥಿರ std::ಸದಿಶ<Twine_ಗಂಟು> nodes_;
};

ಬಾಹ್ಯ int64_t twine_gc_ns;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ twine_gc_ಎಣಿಕೆ;

ರಚನೆ TwinePool : HashConsPool<TwinePool, Twine_ಗಂಟು, IdString> {
	ಸ್ಥಿರ constexpr ಗಾತ್ರ_t STATIC_ಎಣಿಕೆ = STATIC_TWINE_ಕೊನೆ;

	TwinePool();
	TwinePool(ಸ್ಥಿರಾಂಕ TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ TwinePool& other);
	TwinePool& ಕ್ರಿಯಾಕಾರಕ=(TwinePool&& other);

	ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& ಕ್ರಿಯಾಕಾರಕ[](IdString ref) ಸ್ಥಿರಾಂಕ;

	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& static_ಗಂಟು(ಗಾತ್ರ_t idx);

	ಸ್ಥಿರ ಶೂನ್ಯ check_ready();
	ಸ್ಥಿರ uint32_t hash_ಗಂಟು(ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& t) { ಹಿಂತಿರುಗಿ t.hash(); }

	ಮಾದರಿ<typename F>
	ಸ್ಥಿರ ಶೂನ್ಯ for_each_ಮಗು(ಸ್ಥಿರಾಂಕ Twine_ಗಂಟು& t, F&& f) {
		ಆದರೆ (t.is_suffix())
			f(t.ಪೂರ್ವಪ್ರತ್ಯಯ_2());
	}

	ಗಾತ್ರ_t gc(pool<IdString>& roots);

	ಶೂನ್ಯ dump(IdString ref, std::ostream& os = std::cout) ಸ್ಥಿರಾಂಕ;

	// Publicity bit provides escaping
	std::string str(IdString ref) ಸ್ಥಿರಾಂಕ;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ಸ್ಥಿರಾಂಕ;

	ಗಾತ್ರ_t str_ಗಾತ್ರ(IdString ref) ಸ್ಥಿರಾಂಕ;
	bool begins_with(IdString ref, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2) ಸ್ಥಿರಾಂಕ;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ಸ್ಥಿರಾಂಕ;
	ಪೂರ್ಣಾಂಕ compare_by_name(IdString a, IdString b) ಸ್ಥಿರಾಂಕ;
	IdString ಪೂರ್ವಪ್ರತ್ಯಯ_ದ(IdString ref) ಸ್ಥಿರಾಂಕ;

	IdString ಕಂಡುಹಿಡಿ(std::string_view name) ಸ್ಥಿರಾಂಕ;
	IdString ಕಂಡುಹಿಡಿ(TwineSpec t) ಸ್ಥಿರಾಂಕ;
	// Doesn't infer publicity
	IdString ಕೂಡಿಸು(TwineSpec t);
	IdString ಕೂಡಿಸು(IdString ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಬಾಲ);
	// Infers publicity from first character
	IdString ಕೂಡಿಸು(std::string_view s);
	IdString copy_from(ಸ್ಥಿರಾಂಕ TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString ಕಂಡುಹಿಡಿ_from(ಸ್ಥಿರಾಂಕ TwinePool& src, IdString ref) ಸ್ಥಿರಾಂಕ;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ಸ್ಥಿರಾಂಕ;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ check_owned(IdString ref) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
	bool owns(IdString ref) ಸ್ಥಿರಾಂಕ;
	IdString stamp(IdString ref) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ append_str(IdString ref, std::string& ಹೊರಹೋಗು) ಸ್ಥಿರಾಂಕ;
	IdString auto_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ std::string *ಪೂರ್ವಪ್ರತ್ಯಯ_2);
	IdString ಕೂಡಿಸು_inner(Twine_ಗಂಟು t);
	IdString intern(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ);
	uint32_t content_hash(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ) ಸ್ಥಿರಾಂಕ;
	IdString ಕಂಡುಹಿಡಿ_content(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ, uint32_t hash) ಸ್ಥಿರಾಂಕ;
	IdString ಕಂಡುಹಿಡಿ_content(uint32_t ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view ಪಠ್ಯ) ಸ್ಥಿರಾಂಕ;
	ಸ್ಥಿರ ಗಾತ್ರ_t ಮುಂದಿನ_serial();
	dict<ಸ್ಥಿರಾಂಕ std::string *, IdString> auto_prefixes;
	ಗಾತ್ರ_t serial_;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
