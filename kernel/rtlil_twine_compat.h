#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> struct WrappedIdBase;
struct NameSlot;
struct CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> struct OwnedId;
struct PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

inline std::string render_escaped(const TwinePool *pool, IdString id) {
	якщо (id == IdString::Null)
		повернути std::string();
	повернути pool ? pool->str(id) : ID::str(id);
}

inline std::string render_unescaped(const TwinePool *pool, IdString id) {
	якщо (id == IdString::Null)
		повернути std::string();
	повернути pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
struct WrappedIdBase {
	operator IdString() const { повернути self().ref(); }
	operator std::string() const { повернути self().escaped(); }
	std::string escaped() const { повернути wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() const { повернути wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() const { повернути self().ref().isPublic(); }
	bool empty() const { повернути self().ref() == IdString::Null; }
	std::string str() const { повернути self().escaped(); }
	bool begins_with(const char *s) const {
		const TwinePool *pool = self().pool();
		повернути pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(const char *s) const { повернути str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) const {
		повернути self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(const pool<IdString> &rhs) const { повернути self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) const {
		повернути self().escaped().substr(pos, len);
	}
	size_t size() const {
		const TwinePool *pool = self().pool();
		повернути pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(const char *p) const { повернути self().escaped().find(p) != std::string::npos; }
	char operator[](ціле n) const { повернути self().escaped()[n]; }
	bool lt_by_name(const Derived &rhs) const {
		const TwinePool *pool = self().pool();
		якщо (pool == nullptr)
			повернути self().escaped() < rhs.escaped();
		повернути pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(const Derived &lhs, const Derived &rhs) { повернути lhs.ref() == rhs.ref(); }
	friend bool operator==(const Derived &lhs, IdString rhs) { повернути lhs.ref() == rhs; }
	friend bool operator==(const Derived &lhs, NullIdString) { повернути lhs.ref() == IdString::Null; }
	friend bool operator==(const Derived &lhs, const std::string &rhs) {
		const TwinePool *pool = lhs.pool();
		повернути pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(const Derived &lhs, const Derived &rhs) { повернути lhs.ref() < rhs.ref(); }
private:
	const Derived &self() const { повернути *static_cast<const Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
inline bool operator==(const A &lhs, const B &rhs) { повернути lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { повернути std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { повернути std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { повернути std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
struct OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = default;
	OwnedId(const OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() const { повернути id_; }
	const TwinePool *pool() const;
	OwnedId &operator=(IdString id) { id_ = id; повернути *this; }
	OwnedId &operator=(const OwnedId &other) { повернути *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { повернути *this = other.ref(); }
	friend void swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
private:
	const Owner *owner() const;
	IdString id_;
};

struct PooledName : WrappedIdBase<PooledName> {
	PooledName() = default;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(const TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(const Design *design, IdString id);
	PooledName(const Module *module, IdString id);
	template<typename D> PooledName(const WrappedIdBase<D> &masq)
		: pool_(static_cast<const D &>(masq).pool()),
		  id_(static_cast<const D &>(masq).ref()) {}
	IdString ref() const { повернути id_; }
	const TwinePool *pool() const { повернути pool_; }
private:
	const TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	struct masq_hash_ops {
		static inline bool cmp(const T &a, const T &b) { повернути a == b; }
		[[nodiscard]] static inline Hasher hash(const T &a) {
			повернути hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] static inline Hasher hash_into(const T &a, Hasher h) {
			повернути hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	struct hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> struct hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
