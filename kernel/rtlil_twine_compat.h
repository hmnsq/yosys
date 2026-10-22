#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> ساختار WrappedIdBase;
ساختار NameSlot;
ساختار CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> ساختار OwnedId;
ساختار PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

درونخطی std::string render_escaped(ثابت TwinePool *pool, IdString id) {
	اگر (id == IdString::Null)
		بازگشت std::string();
	بازگشت pool ? pool->str(id) : ID::str(id);
}

درونخطی std::string render_unescaped(ثابت TwinePool *pool, IdString id) {
	اگر (id == IdString::Null)
		بازگشت std::string();
	بازگشت pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
ساختار WrappedIdBase {
	operator IdString() ثابت { بازگشت self().ref(); }
	operator std::string() ثابت { بازگشت self().escaped(); }
	std::string escaped() ثابت { بازگشت wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { بازگشت wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ثابت { بازگشت self().ref().isPublic(); }
	bool empty() ثابت { بازگشت self().ref() == IdString::Null; }
	std::string str() ثابت { بازگشت self().escaped(); }
	bool begins_with(ثابت نویسه *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		بازگشت pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ثابت نویسه *s) ثابت { بازگشت str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ثابت {
		بازگشت self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ثابت pool<IdString> &rhs) ثابت { بازگشت self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ثابت {
		بازگشت self().escaped().substr(pos, len);
	}
	size_t size() ثابت {
		ثابت TwinePool *pool = self().pool();
		بازگشت pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ثابت نویسه *p) ثابت { بازگشت self().escaped().find(p) != std::string::npos; }
	نویسه operator[](صحیح n) ثابت { بازگشت self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		اگر (pool == nullptr)
			بازگشت self().escaped() < rhs.escaped();
		بازگشت pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ثابت Derived &lhs, ثابت Derived &rhs) { بازگشت lhs.ref() == rhs.ref(); }
	friend bool operator==(ثابت Derived &lhs, IdString rhs) { بازگشت lhs.ref() == rhs; }
	friend bool operator==(ثابت Derived &lhs, NullIdString) { بازگشت lhs.ref() == IdString::Null; }
	friend bool operator==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		بازگشت pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ثابت Derived &lhs, ثابت Derived &rhs) { بازگشت lhs.ref() < rhs.ref(); }
خصوصی:
	ثابت Derived &self() ثابت { بازگشت *static_cast<ثابت Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
درونخطی bool operator==(ثابت A &lhs, ثابت B &rhs) { بازگشت lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { بازگشت std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { بازگشت std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { بازگشت std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
ساختار OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = پیشفرض;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { بازگشت id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &operator=(IdString id) { id_ = id; بازگشت *this; }
	OwnedId &operator=(ثابت OwnedId &other) { بازگشت *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { بازگشت *this = other.ref(); }
	friend تهی swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
خصوصی:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

ساختار PooledName : WrappedIdBase<PooledName> {
	PooledName() = پیشفرض;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	template<typename D> PooledName(ثابت WrappedIdBase<D> &masq)
		: pool_(static_cast<ثابت D &>(masq).pool()),
		  id_(static_cast<ثابت D &>(masq).ref()) {}
	IdString ref() ثابت { بازگشت id_; }
	ثابت TwinePool *pool() ثابت { بازگشت pool_; }
خصوصی:
	ثابت TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	ساختار masq_hash_ops {
		ایستا درونخطی bool cmp(ثابت T &a, ثابت T &b) { بازگشت a == b; }
		[[nodiscard]] ایستا درونخطی Hasher hash(ثابت T &a) {
			بازگشت hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ایستا درونخطی Hasher hash_into(ثابت T &a, Hasher h) {
			بازگشت hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	ساختار hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> ساختار hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
