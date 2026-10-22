#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#include "kernel/hashcons.h"
#include "kernel/yosys_common.h"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstring>
#include <deque>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>
#include <vector>

YOSYS_NAMESPACE_BEGIN

struct TwineSpec;
struct TwinePool;
struct IdString;

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

struct NullIdString {
	constexpr operator IdString() const;
	constexpr bool operator==(IdString ref) const;
};

struct IdString {
private:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#else
	using bits_t = uint32_t;
#endif
	bits_t हालागा;

	static constexpr int TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	static constexpr int TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	static constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	static constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	static constexpr int TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	static constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	static constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

public:
	static constexpr NullIdString Null{};

	static constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	static constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() const {
		return हालागा == TWINE_NULL_VAL ? हालागा : (हालागा & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() const { return हालागा; }

	constexpr size_t order_सुसि() const { return std::rotl((bits_t) eq_सुसि(), 1); }

	constexpr size_t serial() const {
		return हालागा == TWINE_NULL_VAL ? 0 : ((हालागा & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) const {
		return हालागा == TWINE_NULL_VAL ? *this
			: IdString((हालागा & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : हालागा(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : हालागा((bits_t) val) {}

	constexpr size_t eq_सुसि() const { return हालागा & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(const IdString &rhs) const { return eq_सुसि() == rhs.eq_सुसि(); }
	constexpr bool operator!=(const IdString &rhs) const { return eq_सुसि() != rhs.eq_सुसि(); }
	constexpr std::strong_ordering operator<=>(const IdString &rhs) const {
		return order_सुसि() <=> rhs.order_सुसि();
	}

	template <typename... मगा_कातविरान_3>
	constexpr bool in(const मगा_कातविरान_3&... मगा_कातविरान_2) const {
		return (in_one(मगा_कातविरान_2) || ...);
	}
	bool in(const pool<IdString> &rhs) const;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> struct compare_ptr_by_name {
		bool operator()(const T *a, const T *b) const {
			return (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() const { return हालागा == TWINE_NULL_VAL; }
	constexpr void clear() { हालागा = TWINE_NULL_VAL; }
	constexpr bool isPublic() const { return हालागा != TWINE_NULL_VAL && (हालागा & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() const {
		return हालागा == TWINE_NULL_VAL ? *this : IdString(हालागा & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) const {
		return हालागा == TWINE_NULL_VAL ? *this : IdString(pub ? (हालागा | TWINE_PUBLIC_BIT) : (हालागा & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() const;
	static size_t handle_token_prefix(std::string_view token, bool &is_public);

private:
	constexpr bool in_one(IdString rhs) const { return *this == rhs; }
	bool in_one(const pool<IdString> &rhs) const;
};

constexpr NullIdString::operator IdString() const { return IdString(); }
constexpr bool NullIdString::operator==(IdString ref) const { return ref.empty(); }

namespace hashlib {
	template<>
	struct hash_ops<IdString> {
		static inline bool cmp(IdString a, IdString b) { return a == b; }
		[[nodiscard]] static inline Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_सुसि());
			return h;
		}
		[[nodiscard]] static inline Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_सुसि());
			return h;
		}
	};
}

inline bool IdString::in_one(const pool<IdString> &rhs) const { return rhs.बिलांग(*this) != 0; }
inline bool IdString::in(const pool<IdString> &rhs) const { return in_one(rhs); }


enum : short {
#define X(N) IDX_##N,
#include "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

struct ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) static constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#include "kernel/constids.inc"
#undef X

	static constexpr const char* static_names[] = {
#define X(N) #N,
#include "kernel/constids.inc"
#undef X
	};

	static constexpr IdString lookup(std::string_view name)
	{
		int माबाबा = 0, माताअस = STATIC_TWINE_END;
		while (माताअस - माबाबा >= 2) {
			int mid = (माबाबा + माताअस) / 2;
			if (name < static_names[mid])
				माताअस = mid;
			else
				माबाबा = mid;
		}

		if (name != static_names[माबाबा])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		return IdString(माबाबा).tag(name[0] != '$');
	}

	static constexpr bool is_static(IdString ref) {
		return ref.untag().raw() < STATIC_TWINE_END;
	}

	static constexpr IdString static_ref(size_t raw) {
		if (!is_static(IdString(raw)))
			throw "not a static twine id";
		return IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	static std::string str(IdString ref);
	static std::string unescaped_str(IdString ref);
};

template<size_t Raw> inline constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
struct TwineSpec {
	// deduplicates shared prefixes
	struct Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(const Suffix&) const = default;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	struct AutoSuffix {
		const std::string *prefix;
		std::string tail;
		auto operator<=>(const AutoSuffix&) const = default;
	};

	// "leaf", regular deduplicated string
	struct Leaf {
		std::string s;
		auto operator<=>(const Leaf&) const = default;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() const;
};

struct SmallString {
	static constexpr uint32_t INLINE_CAP = 8;
	static constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = default;
	explicit SmallString(std::string_view content);
	SmallString(const SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(const SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() const { return {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

private:
	void store(std::string_view content);
	void release();

	union {
		char inl_[INLINE_CAP];
		char *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
struct Twine_बुहोल {
	static constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	static constexpr uint32_t DEAD = NO_PREFIX - 1;

	struct सुसि_2 {
		uint32_t prefix;
		std::string_view text;
	};

	static constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		for (char c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
		return hash;
	}

	Twine_बुहोल() = default;
	explicit Twine_बुहोल(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_बुहोल(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_बुहोल(const Twine_बुहोल &other) = default;
	Twine_बुहोल(Twine_बुहोल &&other) noexcept;
	Twine_बुहोल &operator=(const Twine_बुहोल &other) = default;
	Twine_बुहोल &operator=(Twine_बुहोल &&other) noexcept;
	~Twine_बुहोल() = default;

	enum class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() const { return prefix_ == DEAD; }
	constexpr bool is_leaf() const { return prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() const { return prefix_ < DEAD; }

	constexpr Kind kind() const {
		return prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() const { return text_.view(); }
	IdString prefix() const { return IdString(prefix_); }
	uint32_t hash() const { return hash_; }
	सुसि_2 सुसि() const { return {prefix_, text()}; }

	bool operator==(const Twine_बुहोल &other) const
		{ return prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(const सुसि_2 &other) const
		{ return prefix_ == other.prefix && text() == other.text; }

private:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(sizeof(Twine_बुहोल) == (sizeof(void *) == 8 ? 24 : 20));

struct StaticTwines {
	static void init();
	static const Twine_बुहोल &बुहोल(size_t idx);
	static bool ready();

private:
	static std::vector<Twine_बुहोल> nodes_;
};

extern int64_t twine_gc_ns;
extern int twine_gc_बिलांग;

struct TwinePool : HashConsPool<TwinePool, Twine_बुहोल, IdString> {
	static constexpr size_t STATIC_बिलांग = STATIC_TWINE_END;

	TwinePool();
	TwinePool(const TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(const TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	const Twine_बुहोल& operator[](IdString ref) const;

	static const Twine_बुहोल& static_बुहोल(size_t idx);

	static void check_ready();
	static uint32_t hash_बुहोल(const Twine_बुहोल& t) { return t.hash(); }

	template<typename F>
	static void for_each_child(const Twine_बुहोल& t, F&& f) {
		if (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	void dump(IdString ref, std::ostream& os = std::cout) const;

	// Publicity bit provides escaping
	std::string str(IdString ref) const;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) const;

	size_t str_size(IdString ref) const;
	bool begins_with(IdString ref, std::string_view prefix) const;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) const;
	int compare_by_name(IdString a, IdString b) const;
	IdString prefix_of(IdString ref) const;

	IdString find(std::string_view name) const;
	IdString find(TwineSpec t) const;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(const TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(const TwinePool& src, IdString ref) const;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) const;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) const;
	void check_owned(IdString ref) const;

private:
	bool owns(IdString ref) const;
	IdString stamp(IdString ref) const;
	void append_str(IdString ref, std::string& out) const;
	IdString auto_prefix(const std::string *prefix);
	IdString add_inner(Twine_बुहोल t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) const;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) const;
	IdString find_content(uint32_t prefix, std::string_view text) const;
	static size_t next_serial();
	dict<const std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
