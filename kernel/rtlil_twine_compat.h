#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> સંરચના WrappedIdBase;
સંરચના NameSlot;
સંરચના CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> સંરચના OwnedId;
સંરચના PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

અંતઃરેખ std::string render_escaped(અચલ TwinePool *pool, IdString id) {
	જો (id == IdString::Null)
		પાછા std::string();
	પાછા pool ? pool->str(id) : ID::str(id);
}

અંતઃરેખ std::string render_unescaped(અચલ TwinePool *pool, IdString id) {
	જો (id == IdString::Null)
		પાછા std::string();
	પાછા pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
સંરચના WrappedIdBase {
	operator IdString() અચલ { પાછા self().ref(); }
	operator std::string() અચલ { પાછા self().escaped(); }
	std::string escaped() અચલ { પાછા wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() અચલ { પાછા wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() અચલ { પાછા self().ref().isPublic(); }
	bool empty() અચલ { પાછા self().ref() == IdString::Null; }
	std::string str() અચલ { પાછા self().escaped(); }
	bool begins_with(અચલ અક્ષર *s) અચલ {
		અચલ TwinePool *pool = self().pool();
		પાછા pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(અચલ અક્ષર *s) અચલ { પાછા str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) અચલ {
		પાછા self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(અચલ pool<IdString> &rhs) અચલ { પાછા self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) અચલ {
		પાછા self().escaped().substr(pos, len);
	}
	size_t size() અચલ {
		અચલ TwinePool *pool = self().pool();
		પાછા pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(અચલ અક્ષર *p) અચલ { પાછા self().escaped().find(p) != std::string::npos; }
	અક્ષર operator[](પૂર્ણાંક n) અચલ { પાછા self().escaped()[n]; }
	bool lt_by_name(અચલ Derived &rhs) અચલ {
		અચલ TwinePool *pool = self().pool();
		જો (pool == nullptr)
			પાછા self().escaped() < rhs.escaped();
		પાછા pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(અચલ Derived &lhs, અચલ Derived &rhs) { પાછા lhs.ref() == rhs.ref(); }
	friend bool operator==(અચલ Derived &lhs, IdString rhs) { પાછા lhs.ref() == rhs; }
	friend bool operator==(અચલ Derived &lhs, NullIdString) { પાછા lhs.ref() == IdString::Null; }
	friend bool operator==(અચલ Derived &lhs, અચલ std::string &rhs) {
		અચલ TwinePool *pool = lhs.pool();
		પાછા pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(અચલ Derived &lhs, અચલ Derived &rhs) { પાછા lhs.ref() < rhs.ref(); }
ખાનગી:
	અચલ Derived &self() અચલ { પાછા *static_cast<અચલ Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
અંતઃરેખ bool operator==(અચલ A &lhs, અચલ B &rhs) { પાછા lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { પાછા std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { પાછા std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { પાછા std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
સંરચના OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = મૂળભૂત;
	OwnedId(અચલ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() અચલ { પાછા id_; }
	અચલ TwinePool *pool() અચલ;
	OwnedId &operator=(IdString id) { id_ = id; પાછા *this; }
	OwnedId &operator=(અચલ OwnedId &other) { પાછા *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { પાછા *this = other.ref(); }
	friend શૂન્ય swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
ખાનગી:
	અચલ Owner *owner() અચલ;
	IdString id_;
};

સંરચના PooledName : WrappedIdBase<PooledName> {
	PooledName() = મૂળભૂત;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(અચલ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(અચલ Design *design, IdString id);
	PooledName(અચલ Module *module, IdString id);
	template<typename D> PooledName(અચલ WrappedIdBase<D> &masq)
		: pool_(static_cast<અચલ D &>(masq).pool()),
		  id_(static_cast<અચલ D &>(masq).ref()) {}
	IdString ref() અચલ { પાછા id_; }
	અચલ TwinePool *pool() અચલ { પાછા pool_; }
ખાનગી:
	અચલ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	સંરચના masq_hash_ops {
		સ્થિર અંતઃરેખ bool cmp(અચલ T &a, અચલ T &b) { પાછા a == b; }
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash(અચલ T &a) {
			પાછા hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ T &a, Hasher h) {
			પાછા hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	સંરચના hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> સંરચના hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
