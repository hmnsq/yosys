#ifndef YOSYS_TWINE_H
#define YOSYS_TWINE_H

#شامل "kernel/hashcons.h"
#شامل "kernel/yosys_common.h"

#شامل <algorithm>
#شامل <bit>
#شامل <cstdint>
#شامل <cstring>
#شامل <deque>
#شامل <limits>
#شامل <span>
#شامل <string>
#شامل <string_view>
#شامل <unordered_set>
#شامل <variant>
#شامل <vector>

YOSYS_NAMESPACE_BEGIN

ساختار TwineSpec;
ساختار TwinePool;
ساختار IdString;

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

ساختار NullIdString {
	constexpr operator IdString() ثابت;
	constexpr bool operator==(IdString ref) ثابت;
};

ساختار IdString {
خصوصی:
#ifdef YOSYS_ENABLE_TWINE_PROVENANCE
	using bits_t = uint64_t;
#وگرنه
	using bits_t = uint32_t;
#endif
	bits_t مقدار;

	ایستا constexpr صحیح TWINE_WIDTH = std::numeric_limits<bits_t>::digits;
	ایستا constexpr صحیح TWINE_PUBLIC_SHIFT = TWINE_WIDTH - 1;
	ایستا constexpr bits_t TWINE_PUBLIC_BIT = bits_t{1} << TWINE_PUBLIC_SHIFT;
	ایستا constexpr bits_t TWINE_NULL_VAL = ~bits_t{0};
	ایستا constexpr صحیح TWINE_SERIAL_SHIFT = TWINE_WIDTH == 64 ? 32 : TWINE_PUBLIC_SHIFT;
	ایستا constexpr bits_t TWINE_INDEX_MASK = (bits_t{1} << TWINE_SERIAL_SHIFT) - 1;
	ایستا constexpr bits_t TWINE_SERIAL_MASK = bits_t(~(TWINE_INDEX_MASK | TWINE_PUBLIC_BIT));

عمومی:
	ایستا constexpr NullIdString Null{};

	ایستا constexpr size_t MAX_INDEX = TWINE_INDEX_MASK;
	ایستا constexpr size_t MAX_SERIAL = TWINE_SERIAL_MASK >> TWINE_SERIAL_SHIFT;

	constexpr size_t raw() ثابت {
		بازگشت مقدار == TWINE_NULL_VAL ? مقدار : (مقدار & ~TWINE_SERIAL_MASK);
	}

	// raw() with the pool serial kept
	constexpr size_t bits() ثابت { بازگشت مقدار; }

	constexpr size_t order_کلید() ثابت { بازگشت std::rotl((bits_t) eq_کلید(), 1); }

	constexpr size_t serial() ثابت {
		بازگشت مقدار == TWINE_NULL_VAL ? 0 : ((مقدار & TWINE_SERIAL_MASK) >> TWINE_SERIAL_SHIFT);
	}

	constexpr IdString stamped(size_t pool_serial) ثابت {
		بازگشت مقدار == TWINE_NULL_VAL ? *this
			: IdString((مقدار & ~TWINE_SERIAL_MASK)
					| (bits_t(pool_serial << TWINE_SERIAL_SHIFT) & TWINE_SERIAL_MASK));
	}

	constexpr IdString() : مقدار(TWINE_NULL_VAL) {}
	explicit constexpr IdString(size_t val) : مقدار((bits_t) val) {}

	constexpr size_t eq_کلید() ثابت { بازگشت مقدار & ~TWINE_SERIAL_MASK; }

	constexpr bool operator==(ثابت IdString &rhs) ثابت { بازگشت eq_کلید() == rhs.eq_کلید(); }
	constexpr bool operator!=(ثابت IdString &rhs) ثابت { بازگشت eq_کلید() != rhs.eq_کلید(); }
	constexpr std::strong_ordering operator<=>(ثابت IdString &rhs) ثابت {
		بازگشت order_کلید() <=> rhs.order_کلید();
	}

	template <typename... Args>
	constexpr bool in(ثابت Args&... args) ثابت {
		بازگشت (in_one(args) || ...);
	}
	bool in(ثابت pool<IdString> &rhs) ثابت;

	// Instead of for example std::set<Cell*>
	// use std::set<Cell*, IdString::compare_ptr_by_name<Cell>> if the order of cells in the
	// set has an influence on the algorithm.
	template<typename T> ساختار compare_ptr_by_name {
		bool operator()(ثابت T *a, ثابت T *b) ثابت {
			بازگشت (a == nullptr || b == nullptr) ? (a < b) : (a->name < b->name);
		}
	};

	// A ref is "empty" when it names nothing at all.
	constexpr bool empty() ثابت { بازگشت مقدار == TWINE_NULL_VAL; }
	constexpr تهی clear() { مقدار = TWINE_NULL_VAL; }
	constexpr bool isPublic() ثابت { بازگشت مقدار != TWINE_NULL_VAL && (مقدار & TWINE_PUBLIC_BIT); }

	constexpr IdString untag() ثابت {
		بازگشت مقدار == TWINE_NULL_VAL ? *this : IdString(مقدار & ~TWINE_PUBLIC_BIT);
	}
	constexpr IdString tag(bool pub) ثابت {
		بازگشت مقدار == TWINE_NULL_VAL ? *this : IdString(pub ? (مقدار | TWINE_PUBLIC_BIT) : (مقدار & ~TWINE_PUBLIC_BIT));
	}

	std::string handle_token() ثابت;
	ایستا size_t handle_token_prefix(std::string_view token, bool &is_public);

خصوصی:
	constexpr bool in_one(IdString rhs) ثابت { بازگشت *this == rhs; }
	bool in_one(ثابت pool<IdString> &rhs) ثابت;
};

constexpr NullIdString::operator IdString() ثابت { بازگشت IdString(); }
constexpr bool NullIdString::operator==(IdString ref) ثابت { بازگشت ref.empty(); }

namespace hashlib {
	template<>
	ساختار hash_ops<IdString> {
		ایستا درونخطی bool cmp(IdString a, IdString b) { بازگشت a == b; }
		[[nodiscard]] ایستا درونخطی Hasher hash(IdString id) {
			Hasher h;
			h.force((Hasher::hash_t) id.eq_کلید());
			بازگشت h;
		}
		[[nodiscard]] ایستا درونخطی Hasher hash_into(IdString id, Hasher h) {
			h.hash64(id.eq_کلید());
			بازگشت h;
		}
	};
}

درونخطی bool IdString::in_one(ثابت pool<IdString> &rhs) ثابت { بازگشت rhs.شمار(*this) != 0; }
درونخطی bool IdString::in(ثابت pool<IdString> &rhs) ثابت { بازگشت in_one(rhs); }


شمارش : short {
#define X(N) IDX_##N,
#شامل "kernel/constids.inc"
#undef X
	STATIC_TWINE_END
};

ساختار ID {
// Static ids are name handles: non-'$' constids were '\'-escaped publics,
// so their handles carry the publicity bit baked in at compile time.
#define X(N) ایستا constexpr IdString N = IdString(IDX_##N).tag((#N)[0] != '$');
#شامل "kernel/constids.inc"
#undef X

	ایستا constexpr ثابت نویسه* static_names[] = {
#define X(N) #N,
#شامل "kernel/constids.inc"
#undef X
	};

	ایستا constexpr IdString lookup(std::string_view name)
	{
		صحیح کم = 0, بالا = STATIC_TWINE_END;
		وقتی (بالا - کم >= 2) {
			صحیح mid = (کم + بالا) / 2;
			اگر (name < static_names[mid])
				بالا = mid;
			وگرنه
				کم = mid;
		}

		اگر (name != static_names[کم])
			throw "unknown twine id, strings that aren't statically allocated in kernel/constids.inc must be interned into a TwinePool";

		بازگشت IdString(کم).tag(name[0] != '$');
	}

	ایستا constexpr bool is_static(IdString ref) {
		بازگشت ref.untag().raw() < STATIC_TWINE_END;
	}

	ایستا constexpr IdString static_ref(size_t raw) {
		اگر (!is_static(IdString(raw)))
			throw "not a static twine id";
		بازگشت IdString(raw);
	}

	// Static IdString can be constructed without a design pointer
	ایستا std::string str(IdString ref);
	ایستا std::string unescaped_str(IdString ref);
};

template<size_t Raw> درونخطی constexpr IdString constid = ID::static_ref(Raw);

#define ID(id) (YOSYS_NAMESPACE_PREFIX constid<YOSYS_NAMESPACE_PREFIX ID::lookup(#id).raw()>)

// TwineSpec is the object that lives in the TwinePool,
// while IdString points to a TwineSpec and carries publicity information
ساختار TwineSpec {
	// deduplicates shared prefixes
	ساختار Suffix {
		IdString prefix;
		std::string tail;
		auto operator<=>(ثابت Suffix&) ثابت = پیشفرض;
	};

	// transient suffix constructed with NEW_ID and NEW_ID_SUFFIX
	// turned into a regular Suffix when added to a TwinePool
	ساختار AutoSuffix {
		ثابت std::string *prefix;
		std::string tail;
		auto operator<=>(ثابت AutoSuffix&) ثابت = پیشفرض;
	};

	// "leaf", regular deduplicated string
	ساختار Leaf {
		std::string s;
		auto operator<=>(ثابت Leaf&) ثابت = پیشفرض;
	};

	std::variant<Leaf, Suffix, AutoSuffix> data;

	TwineSpec(Leaf v);
	TwineSpec(Suffix v);
	TwineSpec(AutoSuffix v);

	// Only for the pool-free variants, Leaf and AutoSuffix
	std::string content_str() ثابت;
};

ساختار SmallString {
	ایستا constexpr uint32_t INLINE_CAP = 8;
	ایستا constexpr size_t MAX_LEN = std::numeric_limits<uint32_t>::max();

	SmallString() = پیشفرض;
	explicit SmallString(std::string_view content);
	SmallString(ثابت SmallString &other);
	SmallString(SmallString &&other) noexcept;
	SmallString &operator=(ثابت SmallString &other);
	SmallString &operator=(SmallString &&other) noexcept;
	~SmallString();

	std::string_view view() ثابت { بازگشت {len_ <= INLINE_CAP ? inl_ : ptr_, len_}; }

خصوصی:
	تهی store(std::string_view content);
	تهی release();

	اجتماع {
		نویسه inl_[INLINE_CAP];
		نویسه *ptr_;
	};
	uint32_t len_ = 0;
};

// TwineNode is the in-memory version of TwineSpec
ساختار Twine_گره {
	ایستا constexpr uint32_t NO_PREFIX = ~uint32_t{0};
	ایستا constexpr uint32_t DEAD = NO_PREFIX - 1;

	ساختار کلید_2 {
		uint32_t prefix;
		std::string_view text;
	};

	ایستا constexpr uint32_t extend_hash(uint32_t hash, std::string_view tail) {
		// Basically DJB2, but we're not doing it with hashlib.h hash64,
		// so that we get the "content hashing" identity:
		// hash(suffix(leaf("a"), "b")) == hash(leaf("ab"))
		برای (نویسه c : tail)
			hash = ((hash << 5) + hash) + static_cast<unsigned نویسه>(c);
		بازگشت hash;
	}

	Twine_گره() = پیشفرض;
	explicit Twine_گره(std::string_view content)
		: text_(content), prefix_(NO_PREFIX), hash_(extend_hash(0, content)) {}
	Twine_گره(uint32_t prefix, std::string_view tail, uint32_t hash)
		: text_(tail), prefix_(prefix), hash_(hash) {}
	Twine_گره(ثابت Twine_گره &other) = پیشفرض;
	Twine_گره(Twine_گره &&other) noexcept;
	Twine_گره &operator=(ثابت Twine_گره &other) = پیشفرض;
	Twine_گره &operator=(Twine_گره &&other) noexcept;
	~Twine_گره() = پیشفرض;

	شمارش class Kind { Dead, Leaf, Suffix };

	constexpr bool is_dead() ثابت { بازگشت prefix_ == DEAD; }
	constexpr bool is_leaf() ثابت { بازگشت prefix_ == NO_PREFIX; }
	constexpr bool is_suffix() ثابت { بازگشت prefix_ < DEAD; }

	constexpr Kind kind() ثابت {
		بازگشت prefix_ == DEAD ? Kind::Dead
			: prefix_ == NO_PREFIX ? Kind::Leaf
			: Kind::Suffix;
	}

	std::string_view text() ثابت { بازگشت text_.view(); }
	IdString prefix() ثابت { بازگشت IdString(prefix_); }
	uint32_t hash() ثابت { بازگشت hash_; }
	کلید_2 کلید() ثابت { بازگشت {prefix_, text()}; }

	bool operator==(ثابت Twine_گره &other) ثابت
		{ بازگشت prefix_ == other.prefix_ && text() == other.text(); }
	bool operator==(ثابت کلید_2 &other) ثابت
		{ بازگشت prefix_ == other.prefix && text() == other.text; }

خصوصی:
	SmallString text_;
	uint32_t prefix_ = DEAD;
	uint32_t hash_ = 0;
};

static_assert(اندازه(Twine_گره) == (اندازه(تهی *) == 8 ? 24 : 20));

ساختار StaticTwines {
	ایستا تهی init();
	ایستا ثابت Twine_گره &گره(size_t idx);
	ایستا bool ready();

خصوصی:
	ایستا std::vector<Twine_گره> nodes_;
};

بیرونی int64_t twine_gc_ns;
بیرونی صحیح twine_gc_شمار;

ساختار TwinePool : HashConsPool<TwinePool, Twine_گره, IdString> {
	ایستا constexpr size_t STATIC_شمار = STATIC_TWINE_END;

	TwinePool();
	TwinePool(ثابت TwinePool& other);
	TwinePool(TwinePool&& other);
	TwinePool& operator=(ثابت TwinePool& other);
	TwinePool& operator=(TwinePool&& other);

	ثابت Twine_گره& operator[](IdString ref) ثابت;

	ایستا ثابت Twine_گره& static_گره(size_t idx);

	ایستا تهی check_ready();
	ایستا uint32_t hash_گره(ثابت Twine_گره& t) { بازگشت t.hash(); }

	template<typename F>
	ایستا تهی for_each_child(ثابت Twine_گره& t, F&& f) {
		اگر (t.is_suffix())
			f(t.prefix());
	}

	size_t gc(pool<IdString>& roots);

	تهی dump(IdString ref, std::ostream& os = std::cout) ثابت;

	// Publicity bit provides escaping
	std::string str(IdString ref) ثابت;
	// Publicity bit ignored
	std::string unescaped_str(IdString ref) ثابت;

	size_t str_size(IdString ref) ثابت;
	bool begins_with(IdString ref, std::string_view prefix) ثابت;
	// Publicity bit provides escaping, as in str()
	bool name_equal(IdString ref, std::string_view name) ثابت;
	صحیح compare_by_name(IdString a, IdString b) ثابت;
	IdString prefix_of(IdString ref) ثابت;

	IdString find(std::string_view name) ثابت;
	IdString find(TwineSpec t) ثابت;
	// Doesn't infer publicity
	IdString add(TwineSpec t);
	IdString add(IdString prefix, std::string_view tail);
	// Infers publicity from first character
	IdString add(std::string_view s);
	IdString copy_from(ثابت TwinePool& src, IdString ref);
	// Non-mutating counterpart of copy_from
	IdString find_from(ثابت TwinePool& src, IdString ref) ثابت;
	// Opaque handle for files that never leave one run of yosys.
	// Only valid until garbage collection reuses the slot.
	std::string ref_token(IdString ref) ثابت;
	// Null unless the token names a live twine stamped by this pool
	IdString ref_from_token(std::string_view token) ثابت;
	تهی check_owned(IdString ref) ثابت;

خصوصی:
	bool owns(IdString ref) ثابت;
	IdString stamp(IdString ref) ثابت;
	تهی append_str(IdString ref, std::string& out) ثابت;
	IdString auto_prefix(ثابت std::string *prefix);
	IdString add_inner(Twine_گره t);
	IdString intern(uint32_t prefix, std::string_view text);
	uint32_t content_hash(uint32_t prefix, std::string_view text) ثابت;
	IdString find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت;
	IdString find_content(uint32_t prefix, std::string_view text) ثابت;
	ایستا size_t next_serial();
	dict<ثابت std::string *, IdString> auto_prefixes;
	size_t serial_;
};

YOSYS_NAMESPACE_END

#endif
