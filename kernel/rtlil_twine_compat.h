#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> সংরচনা WrappedIdBase;
সংরচনা NameSlot;
সংরচনা CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> সংরচনা OwnedId;
সংরচনা PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

অন্তঃস্থ std::string render_escaped(ধ্রুৱ TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		ঘূৰাই std::string();
	ঘূৰাই pool ? pool->str(id) : ID::str(id);
}

অন্তঃস্থ std::string render_unescaped(ধ্রুৱ TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		ঘূৰাই std::string();
	ঘূৰাই pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
সংরচনা WrappedIdBase {
	operator IdString() ধ্রুৱ { ঘূৰাই self().ref(); }
	operator std::string() ধ্রুৱ { ঘূৰাই self().escaped(); }
	std::string escaped() ধ্রুৱ { ঘূৰাই wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ধ্রুৱ { ঘূৰাই wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ধ্রুৱ { ঘূৰাই self().ref().isPublic(); }
	bool empty() ধ্রুৱ { ঘূৰাই self().ref() == IdString::Null; }
	std::string str() ধ্রুৱ { ঘূৰাই self().escaped(); }
	bool begins_with(ধ্রুৱ ৱর্ণ *s) ধ্রুৱ {
		ধ্রুৱ TwinePool *pool = self().pool();
		ঘূৰাই pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ধ্রুৱ ৱর্ণ *s) ধ্রুৱ { ঘূৰাই str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ধ্রুৱ {
		ঘূৰাই self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ধ্রুৱ pool<IdString> &rhs) ধ্রুৱ { ঘূৰাই self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ধ্রুৱ {
		ঘূৰাই self().escaped().substr(pos, len);
	}
	size_t size() ধ্রুৱ {
		ধ্রুৱ TwinePool *pool = self().pool();
		ঘূৰাই pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ধ্রুৱ ৱর্ণ *p) ধ্রুৱ { ঘূৰাই self().escaped().find(p) != std::string::npos; }
	ৱর্ণ operator[](পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই self().escaped()[n]; }
	bool lt_by_name(ধ্রুৱ Derived &rhs) ধ্রুৱ {
		ধ্রুৱ TwinePool *pool = self().pool();
		যদি (pool == nullptr)
			ঘূৰাই self().escaped() < rhs.escaped();
		ঘূৰাই pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ধ্রুৱ Derived &lhs, ধ্রুৱ Derived &rhs) { ঘূৰাই lhs.ref() == rhs.ref(); }
	friend bool operator==(ধ্রুৱ Derived &lhs, IdString rhs) { ঘূৰাই lhs.ref() == rhs; }
	friend bool operator==(ধ্রুৱ Derived &lhs, NullIdString) { ঘূৰাই lhs.ref() == IdString::Null; }
	friend bool operator==(ধ্রুৱ Derived &lhs, ধ্রুৱ std::string &rhs) {
		ধ্রুৱ TwinePool *pool = lhs.pool();
		ঘূৰাই pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ধ্রুৱ Derived &lhs, ধ্রুৱ Derived &rhs) { ঘূৰাই lhs.ref() < rhs.ref(); }
গুপ্ত:
	ধ্রুৱ Derived &self() ধ্রুৱ { ঘূৰাই *static_cast<ধ্রুৱ Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
অন্তঃস্থ bool operator==(ধ্রুৱ A &lhs, ধ্রুৱ B &rhs) { ঘূৰাই lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { ঘূৰাই std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ঘূৰাই std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ঘূৰাই std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
সংরচনা OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = স্ৱভাৱ;
	OwnedId(ধ্রুৱ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ধ্রুৱ { ঘূৰাই id_; }
	ধ্রুৱ TwinePool *pool() ধ্রুৱ;
	OwnedId &operator=(IdString id) { id_ = id; ঘূৰাই *this; }
	OwnedId &operator=(ধ্রুৱ OwnedId &other) { ঘূৰাই *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { ঘূৰাই *this = other.ref(); }
	friend শূন্য swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
গুপ্ত:
	ধ্রুৱ Owner *owner() ধ্রুৱ;
	IdString id_;
};

সংরচনা PooledName : WrappedIdBase<PooledName> {
	PooledName() = স্ৱভাৱ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ধ্রুৱ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ধ্রুৱ Design *design, IdString id);
	PooledName(ধ্রুৱ Module *module, IdString id);
	template<typename D> PooledName(ধ্রুৱ WrappedIdBase<D> &masq)
		: pool_(static_cast<ধ্রুৱ D &>(masq).pool()),
		  id_(static_cast<ধ্রুৱ D &>(masq).ref()) {}
	IdString ref() ধ্রুৱ { ঘূৰাই id_; }
	ধ্রুৱ TwinePool *pool() ধ্রুৱ { ঘূৰাই pool_; }
গুপ্ত:
	ধ্রুৱ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	সংরচনা masq_hash_ops {
		স্থির অন্তঃস্থ bool cmp(ধ্রুৱ T &a, ধ্রুৱ T &b) { ঘূৰাই a == b; }
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash(ধ্রুৱ T &a) {
			ঘূৰাই hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুৱ T &a, Hasher h) {
			ঘূৰাই hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	সংরচনা hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> সংরচনা hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
