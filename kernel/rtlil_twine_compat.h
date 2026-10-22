#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> কাঠামো WrappedIdBase;
কাঠামো NameSlot;
কাঠামো CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> কাঠামো OwnedId;
কাঠামো PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

অন্তঃরেখ std::string render_escaped(ধ্রুবক TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		ফেরত std::string();
	ফেরত pool ? pool->str(id) : ID::str(id);
}

অন্তঃরেখ std::string render_unescaped(ধ্রুবক TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		ফেরত std::string();
	ফেরত pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
কাঠামো WrappedIdBase {
	operator IdString() ধ্রুবক { ফেরত self().ref(); }
	operator std::string() ধ্রুবক { ফেরত self().escaped(); }
	std::string escaped() ধ্রুবক { ফেরত wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ধ্রুবক { ফেরত wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ধ্রুবক { ফেরত self().ref().isPublic(); }
	bool empty() ধ্রুবক { ফেরত self().ref() == IdString::Null; }
	std::string str() ধ্রুবক { ফেরত self().escaped(); }
	bool begins_with(ধ্রুবক অক্ষর *s) ধ্রুবক {
		ধ্রুবক TwinePool *pool = self().pool();
		ফেরত pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ধ্রুবক অক্ষর *s) ধ্রুবক { ফেরত str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ধ্রুবক {
		ফেরত self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ধ্রুবক pool<IdString> &rhs) ধ্রুবক { ফেরত self().ref().in(rhs); }
	std::string substr(আকার_t pos = 0, আকার_t len = std::string::npos) ধ্রুবক {
		ফেরত self().escaped().substr(pos, len);
	}
	আকার_t আকার_2() ধ্রুবক {
		ধ্রুবক TwinePool *pool = self().pool();
		ফেরত pool ? pool->str_আকার(self().ref()) : self().escaped().আকার_2();
	}
	bool contains(ধ্রুবক অক্ষর *p) ধ্রুবক { ফেরত self().escaped().find(p) != std::string::npos; }
	অক্ষর operator[](পূর্ণসংখ্যা n) ধ্রুবক { ফেরত self().escaped()[n]; }
	bool lt_by_name(ধ্রুবক Derived &rhs) ধ্রুবক {
		ধ্রুবক TwinePool *pool = self().pool();
		যদি (pool == nullptr)
			ফেরত self().escaped() < rhs.escaped();
		ফেরত pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ধ্রুবক Derived &lhs, ধ্রুবক Derived &rhs) { ফেরত lhs.ref() == rhs.ref(); }
	friend bool operator==(ধ্রুবক Derived &lhs, IdString rhs) { ফেরত lhs.ref() == rhs; }
	friend bool operator==(ধ্রুবক Derived &lhs, NullIdString) { ফেরত lhs.ref() == IdString::Null; }
	friend bool operator==(ধ্রুবক Derived &lhs, ধ্রুবক std::string &rhs) {
		ধ্রুবক TwinePool *pool = lhs.pool();
		ফেরত pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ধ্রুবক Derived &lhs, ধ্রুবক Derived &rhs) { ফেরত lhs.ref() < rhs.ref(); }
ব্যক্তিগত:
	ধ্রুবক Derived &self() ধ্রুবক { ফেরত *static_cast<ধ্রুবক Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
অন্তঃরেখ bool operator==(ধ্রুবক A &lhs, ধ্রুবক B &rhs) { ফেরত lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { ফেরত std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ফেরত std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ফেরত std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
কাঠামো OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = পূর্বনির্ধারিত;
	OwnedId(ধ্রুবক OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ধ্রুবক { ফেরত id_; }
	ধ্রুবক TwinePool *pool() ধ্রুবক;
	OwnedId &operator=(IdString id) { id_ = id; ফেরত *this; }
	OwnedId &operator=(ধ্রুবক OwnedId &other) { ফেরত *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { ফেরত *this = other.ref(); }
	friend শূন্য swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
ব্যক্তিগত:
	ধ্রুবক Owner *owner() ধ্রুবক;
	IdString id_;
};

কাঠামো PooledName : WrappedIdBase<PooledName> {
	PooledName() = পূর্বনির্ধারিত;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ধ্রুবক TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ধ্রুবক Design *design, IdString id);
	PooledName(ধ্রুবক Module *module, IdString id);
	template<typename D> PooledName(ধ্রুবক WrappedIdBase<D> &masq)
		: pool_(static_cast<ধ্রুবক D &>(masq).pool()),
		  id_(static_cast<ধ্রুবক D &>(masq).ref()) {}
	IdString ref() ধ্রুবক { ফেরত id_; }
	ধ্রুবক TwinePool *pool() ধ্রুবক { ফেরত pool_; }
ব্যক্তিগত:
	ধ্রুবক TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	কাঠামো masq_hash_ops {
		স্থির অন্তঃরেখ bool cmp(ধ্রুবক T &a, ধ্রুবক T &b) { ফেরত a == b; }
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash(ধ্রুবক T &a) {
			ফেরত hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক T &a, Hasher h) {
			ফেরত hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	কাঠামো hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> কাঠামো hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
