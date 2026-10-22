#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_TWINE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_TWINE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/hashcons.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <bit>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdint>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <deque>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits>
#ਅਨ੍ਤਰ੍ਭਾਵ <span>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <unordered_set>
#ਅਨ੍ਤਰ੍ਭਾਵ <variant>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ TwineSpec;
ਬਣਤਰ TwinePool;
ਬਣਤਰ IdString;

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

ਬਣਤਰ NullIdString {
	constexpr ਸੰਚਾਲਕ IdString() ਅਚਲ;
	constexpr bool ਸੰਚਾਲਕ==(IdString ref) ਅਚਲ;
};

ਬਣਤਰ IdString {
ਨਿਜੀ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TWINE_PROVENANCE
	using ਬਿੱਟ_t = uint64_t;
#ਨਹੀਂਤਾਂ
	using ਬਿੱਟ_t = uint32_t;
#ਜੇ_ਅੰਤ
	ਬਿੱਟ_t ਮੁੱਲ;

	ਸਥਿਰ constexpr ਪੂਰਨ TWINE_ਚੌੜਾਈ = std::numeric_limits<ਬਿੱਟ_t>::digits;
	ਸਥਿਰ constexpr ਪੂਰਨ TWINE_ਜਨਤਕ_ਖਿਸਕਾਓ = TWINE_ਚੌੜਾਈ - 1;
	ਸਥਿਰ constexpr ਬਿੱਟ_t TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ = ਬਿੱਟ_t{1} << TWINE_ਜਨਤਕ_ਖਿਸਕਾਓ;
	ਸਥਿਰ constexpr ਬਿੱਟ_t TWINE_NULL_VAL = ~ਬਿੱਟ_t{0};
	ਸਥਿਰ constexpr ਪੂਰਨ TWINE_SERIAL_ਖਿਸਕਾਓ = TWINE_ਚੌੜਾਈ == 64 ? 32 : TWINE_ਜਨਤਕ_ਖਿਸਕਾਓ;
	ਸਥਿਰ constexpr ਬਿੱਟ_t TWINE_ਸੂਚਕਾਂਕ_ਨਕਾਬ = (ਬਿੱਟ_t{1} << TWINE_SERIAL_ਖਿਸਕਾਓ) - 1;
	ਸਥਿਰ constexpr ਬਿੱਟ_t TWINE_SERIAL_ਨਕਾਬ = ਬਿੱਟ_t(~(TWINE_ਸੂਚਕਾਂਕ_ਨਕਾਬ | TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ));

ਜਨਤਕ:
	ਸਥਿਰ constexpr NullIdString Null{};

	ਸਥਿਰ constexpr ਮਾਪ_t MAX_ਸੂਚਕਾਂਕ = TWINE_ਸੂਚਕਾਂਕ_ਨਕਾਬ;
	ਸਥਿਰ constexpr ਮਾਪ_t MAX_SERIAL = TWINE_SERIAL_ਨਕਾਬ >> TWINE_SERIAL_ਖਿਸਕਾਓ;

	constexpr ਮਾਪ_t raw() ਅਚਲ {
		ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL ? ਮੁੱਲ : (ਮੁੱਲ & ~TWINE_SERIAL_ਨਕਾਬ);
	}

	// raw() with the pool serial kept
	constexpr ਮਾਪ_t ਬਿੱਟ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ; }

	constexpr ਮਾਪ_t ਕ੍ਰਮ_ਕੁੰਜੀ() ਅਚਲ { ਵਾਪਸ std::rotl((ਬਿੱਟ_t) eq_ਕੁੰਜੀ(), 1); }

	constexpr ਮਾਪ_t serial() ਅਚਲ {
		ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL ? 0 : ((ਮੁੱਲ & TWINE_SERIAL_ਨਕਾਬ) >> TWINE_SERIAL_ਖਿਸਕਾਓ);
	}

	constexpr IdString stamped(ਮਾਪ_t pool_serial) ਅਚਲ {
		ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL ? *this
			: IdString((ਮੁੱਲ & ~TWINE_SERIAL_ਨਕਾਬ)
					| (ਬਿੱਟ_t(pool_serial << TWINE_SERIAL_ਖਿਸਕਾਓ) & TWINE_SERIAL_ਨਕਾਬ));
	}

	constexpr IdString() : ਮੁੱਲ(TWINE_NULL_VAL) {}
	explicit constexpr IdString(ਮਾਪ_t val) : ਮੁੱਲ((ਬਿੱਟ_t) val) {}

	constexpr ਮਾਪ_t eq_ਕੁੰਜੀ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ & ~TWINE_SERIAL_ਨਕਾਬ; }

	constexpr bool ਸੰਚਾਲਕ==(ਅਚਲ IdString &rhs) ਅਚਲ { ਵਾਪਸ eq_ਕੁੰਜੀ() == rhs.eq_ਕੁੰਜੀ(); }
	constexpr bool ਸੰਚਾਲਕ!=(ਅਚਲ IdString &rhs) ਅਚਲ { ਵਾਪਸ eq_ਕੁੰਜੀ() != rhs.eq_ਕੁੰਜੀ(); }
	constexpr std::strong_ordering ਸੰਚਾਲਕ<=>(ਅਚਲ IdString &rhs) ਅਚਲ {
		ਵਾਪਸ ਕ੍ਰਮ_ਕੁੰਜੀ() <=> rhs.ਕ੍ਰਮ_ਕੁੰਜੀ();
	}

	ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
	constexpr bool ਵਿੱਚ(ਅਚਲ ਦਲੀਲਾਂ_3&... ਦਲੀਲਾਂ_2) ਅਚਲ {
		ਵਾਪਸ (ਵਿੱਚ_one(ਦਲੀਲਾਂ_2) || ...);
	}
	bool ਵਿੱਚ(ਅਚਲ pool<IdString> &rhs) ਅਚਲ;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	ਸਾਂਚਾ<typename T> ਬਣਤਰ compare_ptr_by_name {
		bool ਸੰਚਾਲਕ()(ਅਚਲ T *a, ਅਚਲ T *b) ਅਚਲ {
			ਵਾਪਸ (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL; }
	constexpr ਖਾਲੀ clear() { ਮੁੱਲ = TWINE_NULL_VAL; }
	constexpr bool is_ਜਨਤਕ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ != TWINE_NULL_VAL && (ਮੁੱਲ & TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ); }

	constexpr IdString untag() ਅਚਲ {
		ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL ? *this : IdString(ਮੁੱਲ & ~TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ);
	}
	constexpr IdString ਟੈਗ(bool pub) ਅਚਲ {
		ਵਾਪਸ ਮੁੱਲ == TWINE_NULL_VAL ? *this : IdString(pub ? (ਮੁੱਲ | TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ) : (ਮੁੱਲ & ~TWINE_ਜਨਤਕ_ਦੋਆਧਾਰੀ_ਅੰਕ));
	}

	std::string handle_token() ਅਚਲ;
	ਸਥਿਰ ਮਾਪ_t handle_token_ਅਗੇਤਰ(std::string_view token, bool &is_ਜਨਤਕ_2);

ਨਿਜੀ:
	constexpr bool ਵਿੱਚ_one(IdString rhs) ਅਚਲ { ਵਾਪਸ *this == rhs; }
	bool ਵਿੱਚ_one(ਅਚਲ pool<IdString> &rhs) ਅਚਲ;
};

constexpr NullIdString::ਸੰਚਾਲਕ IdString() ਅਚਲ { ਵਾਪਸ IdString(); }
constexpr bool NullIdString::ਸੰਚਾਲਕ==(IdString ref) ਅਚਲ { ਵਾਪਸ ref.ਖਾਲੀ_ਥਾਵਾਂ(); }

namespace hashlib {
	ਸਾਂਚਾ<>
	ਬਣਤਰ hash_ops<IdString> {
		ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(IdString a, IdString b) { ਵਾਪਸ a == b; }
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_ਕੁੰਜੀ());
			ਵਾਪਸ h;
		}
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_ਕੁੰਜੀ());
			ਵਾਪਸ h;
		}
	};
}

ਸਤਰਅੰਦਰ bool IdString::ਵਿੱਚ_one(ਅਚਲ pool<IdString> &rhs) ਅਚਲ { ਵਾਪਸ rhs.ਗਿਣਤੀ_2(*this) != 0; }
ਸਤਰਅੰਦਰ bool IdString::ਵਿੱਚ(ਅਚਲ pool<IdString> &rhs) ਅਚਲ { ਵਾਪਸ ਵਿੱਚ_one(rhs); }


ਗਿਣਤੀ : ਛੋਟਾ {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X(N) IDX_##N,
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/constids.inc"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ X
	STATIC_TWINE_ਸਿਰਾ
};

ਬਣਤਰ ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X(N) ਸਥਿਰ constexpr IdString N = IdString(IDX_##N).ਟੈਗ((#N)[0] != '$');
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/constids.inc"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ X

	ਸਥਿਰ constexpr ਅਚਲ ਅੱਖਰ* static_ਨਾਂ[] = {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X(N) #N,
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/constids.inc"
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ X
	};

	ਸਥਿਰ constexpr IdString lookup(std::string_view name)
	{
		ਪੂਰਨ ਨੀਵਾਂ = 0, ਉੱਚਾ = STATIC_TWINE_ਸਿਰਾ;
		ਜਦੋਂ (ਉੱਚਾ - ਨੀਵਾਂ >= 2) {
			ਪੂਰਨ mid = (ਨੀਵਾਂ + ਉੱਚਾ) / 2;
			ਜੇ (name < static_ਨਾਂ[mid])
				ਉੱਚਾ = mid;
			ਨਹੀਂਤਾਂ
				ਨੀਵਾਂ = mid;
		}

		ਜੇ (name != static_ਨਾਂ[ਨੀਵਾਂ])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		ਵਾਪਸ IdString(ਨੀਵਾਂ).ਟੈਗ(name[0] != '$');
	}

	ਸਥਿਰ constexpr bool is_static(IdString ref) {
		ਵਾਪਸ ref.untag().raw() < STATIC_TWINE_ਸਿਰਾ;
	}

	ਸਥਿਰ constexpr IdString static_ref(ਮਾਪ_t raw) {
		ਜੇ (!is_static(IdString(raw)))
			throw "not a static twine id";
		ਵਾਪਸ IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ਸਥਿਰ std::string str(IdString ref);
	ਸਥਿਰ std::string unescaped_str(IdString ref);
};

ਸਾਂਚਾ<ਮਾਪ_t Raw> ਸਤਰਅੰਦਰ constexpr IdString constid = ID::static_ref(Raw);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID(id) (YOSYS_NAMESPACE_ਅਗੇਤਰ constid<YOSYS_NAMESPACE_ਅਗੇਤਰ ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ਬਣਤਰ TwineSpec {
	// deduplicates shared prefixes
	ਬਣਤਰ Suffix {
		IdString ਅਗੇਤਰ_2;
		std::string ਪੂਛ;
		ਸਵੈਚਾਲਿਤ ਸੰਚਾਲਕ<=>(ਅਚਲ Suffix&) ਅਚਲ = ਮੂਲ;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ਬਣਤਰ AutoSuffix {
		ਅਚਲ std::string *ਅਗੇਤਰ_2;
		std::string ਪੂਛ;
		ਸਵੈਚਾਲਿਤ ਸੰਚਾਲਕ<=>(ਅਚਲ AutoSuffix&) ਅਚਲ = ਮੂਲ;
	};

	// "leaf", regular deduplicated string
	ਬਣਤਰ ਪੱਤੀ {
		std::string s;
		ਸਵੈਚਾਲਿਤ ਸੰਚਾਲਕ<=>(ਅਚਲ ਪੱਤੀ&) ਅਚਲ = ਮੂਲ;
	};

	std::variant<ਪੱਤੀ, Suffix, AutoSuffix> ਅੰਕੜੇ;

	TwineSpec(ਪੱਤੀ v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ਅਚਲ;
};

ਬਣਤਰ SmallString {
	ਸਥਿਰ constexpr uint32_t INLINE_CAP = 8;
	ਸਥਿਰ constexpr ਮਾਪ_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = ਮੂਲ;
	explicit SmallString(std::string_view content);
	SmallString(ਅਚਲ SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &ਸੰਚਾਲਕ=(ਅਚਲ SmallString &other);
	SmallString &ਸੰਚਾਲਕ=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ਅਚਲ { ਵਾਪਸ {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

ਨਿਜੀ:
	ਖਾਲੀ store(std::string_view content);
	ਖਾਲੀ release();

	ਸੰਘ {
		ਅੱਖਰ inl_[INLINE_CAP];
		ਅੱਖਰ *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ਬਣਤਰ Twine_ਗੰਢ {
	ਸਥਿਰ constexpr uint32_t NO_ਅਗੇਤਰ = ~uint32_t{0};
	ਸਥਿਰ constexpr uint32_t DEAD = NO_ਅਗੇਤਰ - 1;

	ਬਣਤਰ ਕੁੰਜੀ_2 {
		uint32_t ਅਗੇਤਰ_2;
		std::string_view ਪਾਠ;
	};

	ਸਥਿਰ constexpr uint32_t extend_hash(uint32_t hash, std::string_view ਪੂਛ) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		ਲਈ (ਅੱਖਰ c : ਪੂਛ)
			hash = ((hash << 5) + hash) + static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(c);
		ਵਾਪਸ hash;
	}

	Twine_ਗੰਢ() = ਮੂਲ;
	explicit Twine_ਗੰਢ(std::string_view content)
		: ਪਾਠ_2(content), ਅਗੇਤਰ_3(NO_ਅਗੇਤਰ), hash_(extend_hash(0, content)) {}
	Twine_ਗੰਢ(uint32_t ਅਗੇਤਰ_2, std::string_view ਪੂਛ, uint32_t hash)
		: ਪਾਠ_2(ਪੂਛ), ਅਗੇਤਰ_3(ਅਗੇਤਰ_2), hash_(hash) {}
	Twine_ਗੰਢ(ਅਚਲ Twine_ਗੰਢ &other) = ਮੂਲ;
	Twine_ਗੰਢ(Twine_ਗੰਢ &&other) noexcept;
	Twine_ਗੰਢ &ਸੰਚਾਲਕ=(ਅਚਲ Twine_ਗੰਢ &other) = ਮੂਲ;
	Twine_ਗੰਢ &ਸੰਚਾਲਕ=(Twine_ਗੰਢ &&other) noexcept;
	~Twine_ਗੰਢ() = ਮੂਲ;

	ਗਿਣਤੀ class ਕਿਸਮ_2 { Dead, ਪੱਤੀ, Suffix };

	constexpr bool is_dead() ਅਚਲ { ਵਾਪਸ ਅਗੇਤਰ_3 == DEAD; }
	constexpr bool is_ਪੱਤੀ() ਅਚਲ { ਵਾਪਸ ਅਗੇਤਰ_3 == NO_ਅਗੇਤਰ; }
	constexpr bool is_suffix() ਅਚਲ { ਵਾਪਸ ਅਗੇਤਰ_3 < DEAD; }

	constexpr ਕਿਸਮ_2 ਕਿਸਮ() ਅਚਲ {
		ਵਾਪਸ ਅਗੇਤਰ_3 == DEAD ? ਕਿਸਮ_2::Dead
			: ਅਗੇਤਰ_3 == NO_ਅਗੇਤਰ ? ਕਿਸਮ_2::ਪੱਤੀ
			: ਕਿਸਮ_2::Suffix;
	}

	std::string_view ਪਾਠ() ਅਚਲ { ਵਾਪਸ ਪਾਠ_2.view(); }
	IdString ਅਗੇਤਰ_2() ਅਚਲ { ਵਾਪਸ IdString(ਅਗੇਤਰ_3); }
	uint32_t hash() ਅਚਲ { ਵਾਪਸ hash_; }
	ਕੁੰਜੀ_2 ਕੁੰਜੀ() ਅਚਲ { ਵਾਪਸ {ਅਗੇਤਰ_3, ਪਾਠ()}; }

	bool ਸੰਚਾਲਕ==(ਅਚਲ Twine_ਗੰਢ &other) ਅਚਲ
		{ ਵਾਪਸ ਅਗੇਤਰ_3 == other.ਅਗੇਤਰ_3 && ਪਾਠ() == other.ਪਾਠ(); }
	bool ਸੰਚਾਲਕ==(ਅਚਲ ਕੁੰਜੀ_2 &other) ਅਚਲ
		{ ਵਾਪਸ ਅਗੇਤਰ_3 == other.ਅਗੇਤਰ_2 && ਪਾਠ() == other.ਪਾਠ; }

ਨਿਜੀ:
	SmallString ਪਾਠ_2;
	uint32_t ਅਗੇਤਰ_3 = DEAD;
	uint32_t hash_ = 0;
};

static_assert(ਆਕਾਰ(Twine_ਗੰਢ) == (ਆਕਾਰ(ਖਾਲੀ *) == 8 ? 24 : 20));

ਬਣਤਰ StaticTwines {
	ਸਥਿਰ ਖਾਲੀ init();
	ਸਥਿਰ ਅਚਲ Twine_ਗੰਢ &ਗੰਢ(ਮਾਪ_t idx);
	ਸਥਿਰ bool ready();

ਨਿਜੀ:
	ਸਥਿਰ std::ਸਦਿਸ਼<Twine_ਗੰਢ> nodes_;
};

ਬਾਹਰੀ int64_t twine_gc_ns;
ਬਾਹਰੀ ਪੂਰਨ twine_gc_ਗਿਣਤੀ;

ਬਣਤਰ TwinePool : HashConsPool<TwinePool, Twine_ਗੰਢ, IdString> {
	ਸਥਿਰ constexpr ਮਾਪ_t STATIC_ਗਿਣਤੀ = STATIC_TWINE_ਸਿਰਾ;

	TwinePool();
	TwinePool(ਅਚਲ TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& ਸੰਚਾਲਕ=(ਅਚਲ TwinePool& other);
	TwinePool& ਸੰਚਾਲਕ=(TwinePool&& other);

	ਅਚਲ Twine_ਗੰਢ& ਸੰਚਾਲਕ[](IdString ref) ਅਚਲ;

	ਸਥਿਰ ਅਚਲ Twine_ਗੰਢ& static_ਗੰਢ(ਮਾਪ_t idx);

	ਸਥਿਰ ਖਾਲੀ check_ready();
	ਸਥਿਰ uint32_t hash_ਗੰਢ(ਅਚਲ Twine_ਗੰਢ& t) { ਵਾਪਸ t.hash(); }

	ਸਾਂਚਾ<typename F>
	ਸਥਿਰ ਖਾਲੀ for_each_ਬੱਚਾ(ਅਚਲ Twine_ਗੰਢ& t, F&& f) {
		ਜੇ (t.is_suffix())
			f(t.ਅਗੇਤਰ_2());
	}

	ਮਾਪ_t gc(pool<IdString>& roots);

	ਖਾਲੀ dump(IdString ref, std::ostream& os = std::cout) ਅਚਲ;

	// Publicity bit provides escaping
	std::string str(IdString ref) ਅਚਲ;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ਅਚਲ;

	ਮਾਪ_t str_ਮਾਪ(IdString ref) ਅਚਲ;
	bool begins_with(IdString ref, std::string_view ਅਗੇਤਰ_2) ਅਚਲ;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ਅਚਲ;
	ਪੂਰਨ compare_by_name(IdString a, IdString b) ਅਚਲ;
	IdString ਅਗੇਤਰ_ਦਾ(IdString ref) ਅਚਲ;

	IdString ਲੱਭੋ(std::string_view name) ਅਚਲ;
	IdString ਲੱਭੋ(TwineSpec t) ਅਚਲ;
	// Doesn't infer publicity
	IdString ਜੋੜੋ(TwineSpec t);
	IdString ਜੋੜੋ(IdString ਅਗੇਤਰ_2, std::string_view ਪੂਛ);
	// Infers publicity from first character
	IdString ਜੋੜੋ(std::string_view s);
	IdString copy_from(ਅਚਲ TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString ਲੱਭੋ_from(ਅਚਲ TwinePool& src, IdString ref) ਅਚਲ;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ਅਚਲ;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ਅਚਲ;
	ਖਾਲੀ check_owned(IdString ref) ਅਚਲ;

ਨਿਜੀ:
	bool owns(IdString ref) ਅਚਲ;
	IdString stamp(IdString ref) ਅਚਲ;
	ਖਾਲੀ append_str(IdString ref, std::string& ਬਾਹਰਲਾ) ਅਚਲ;
	IdString auto_ਅਗੇਤਰ(ਅਚਲ std::string *ਅਗੇਤਰ_2);
	IdString ਜੋੜੋ_inner(Twine_ਗੰਢ t);
	IdString intern(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ);
	uint32_t content_hash(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ) ਅਚਲ;
	IdString ਲੱਭੋ_content(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ, uint32_t hash) ਅਚਲ;
	IdString ਲੱਭੋ_content(uint32_t ਅਗੇਤਰ_2, std::string_view ਪਾਠ) ਅਚਲ;
	ਸਥਿਰ ਮਾਪ_t ਅਗਲਾ_serial();
	dict<ਅਚਲ std::string *, IdString> auto_prefixes;
	ਮਾਪ_t serial_;
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
