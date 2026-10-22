#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> sa_nracanaa WrappedIdBase;
sa_nracanaa NameSlot;
sa_nracanaa CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> sa_nracanaa OwnedId;
sa_nracanaa PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

anta_hstha std::string render_escaped(const TwinePool *pool, IdString id) {
	ຖ້າ (id == IdString::Null)
		ສົ່ງຄືນ std::string();
	ສົ່ງຄືນ pool ? pool->str(id) : ID::str(id);
}

anta_hstha std::string render_unescaped(const TwinePool *pool, IdString id) {
	ຖ້າ (id == IdString::Null)
		ສົ່ງຄືນ std::string();
	ສົ່ງຄືນ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
sa_nracanaa WrappedIdBase {
	operator IdString() const { ສົ່ງຄືນ self().ref(); }
	operator std::string() const { ສົ່ງຄືນ self().escaped(); }
	std::string escaped() const { ສົ່ງຄືນ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() const { ສົ່ງຄືນ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() const { ສົ່ງຄືນ self().ref().isPublic(); }
	bool empty() const { ສົ່ງຄືນ self().ref() == IdString::Null; }
	std::string str() const { ສົ່ງຄືນ self().escaped(); }
	bool begins_with(const var_na *s) const {
		const TwinePool *pool = self().pool();
		ສົ່ງຄືນ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(const var_na *s) const { ສົ່ງຄືນ str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) const {
		ສົ່ງຄືນ self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(const pool<IdString> &rhs) const { ສົ່ງຄືນ self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) const {
		ສົ່ງຄືນ self().escaped().substr(pos, len);
	}
	size_t size() const {
		const TwinePool *pool = self().pool();
		ສົ່ງຄືນ pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(const var_na *p) const { ສົ່ງຄືນ self().escaped().find(p) != std::string::npos; }
	var_na operator[](ຈຳນວນເຕັມ n) const { ສົ່ງຄືນ self().escaped()[n]; }
	bool lt_by_name(const Derived &rhs) const {
		const TwinePool *pool = self().pool();
		ຖ້າ (pool == nullptr)
			ສົ່ງຄືນ self().escaped() < rhs.escaped();
		ສົ່ງຄືນ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(const Derived &lhs, const Derived &rhs) { ສົ່ງຄືນ lhs.ref() == rhs.ref(); }
	friend bool operator==(const Derived &lhs, IdString rhs) { ສົ່ງຄືນ lhs.ref() == rhs; }
	friend bool operator==(const Derived &lhs, NullIdString) { ສົ່ງຄືນ lhs.ref() == IdString::Null; }
	friend bool operator==(const Derived &lhs, const std::string &rhs) {
		const TwinePool *pool = lhs.pool();
		ສົ່ງຄືນ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(const Derived &lhs, const Derived &rhs) { ສົ່ງຄືນ lhs.ref() < rhs.ref(); }
private:
	const Derived &self() const { ສົ່ງຄືນ *static_cast<const Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
anta_hstha bool operator==(const A &lhs, const B &rhs) { ສົ່ງຄືນ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { ສົ່ງຄືນ std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ສົ່ງຄືນ std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ສົ່ງຄືນ std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
sa_nracanaa OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = default;
	OwnedId(const OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() const { ສົ່ງຄືນ id_; }
	const TwinePool *pool() const;
	OwnedId &operator=(IdString id) { id_ = id; ສົ່ງຄືນ *this; }
	OwnedId &operator=(const OwnedId &other) { ສົ່ງຄືນ *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { ສົ່ງຄືນ *this = other.ref(); }
	friend void swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
private:
	const Owner *owner() const;
	IdString id_;
};

sa_nracanaa PooledName : WrappedIdBase<PooledName> {
	PooledName() = default;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(const TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(const Design *design, IdString id);
	PooledName(const Module *module, IdString id);
	template<typename D> PooledName(const WrappedIdBase<D> &masq)
		: pool_(static_cast<const D &>(masq).pool()),
		  id_(static_cast<const D &>(masq).ref()) {}
	IdString ref() const { ສົ່ງຄືນ id_; }
	const TwinePool *pool() const { ສົ່ງຄືນ pool_; }
private:
	const TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	sa_nracanaa masq_hash_ops {
		static anta_hstha bool cmp(const T &a, const T &b) { ສົ່ງຄືນ a == b; }
		[[nodiscard]] static anta_hstha Hasher hash(const T &a) {
			ສົ່ງຄືນ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] static anta_hstha Hasher hash_into(const T &a, Hasher h) {
			ສົ່ງຄືນ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	sa_nracanaa hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> sa_nracanaa hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
