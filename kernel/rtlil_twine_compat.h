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

অন্তঃস্থ std::string render_escaped(ধ্রুব TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		প্রত্যাগম std::string();
	প্রত্যাগম pool ? pool->str(id) : ID::str(id);
}

অন্তঃস্থ std::string render_unescaped(ধ্রুব TwinePool *pool, IdString id) {
	যদি (id == IdString::Null)
		প্রত্যাগম std::string();
	প্রত্যাগম pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
সংরচনা WrappedIdBase {
	operator IdString() ধ্রুব { প্রত্যাগম self().ref(); }
	operator std::string() ধ্রুব { প্রত্যাগম self().escaped(); }
	std::string escaped() ধ্রুব { প্রত্যাগম wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ধ্রুব { প্রত্যাগম wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ধ্রুব { প্রত্যাগম self().ref().isPublic(); }
	bool empty() ধ্রুব { প্রত্যাগম self().ref() == IdString::Null; }
	std::string str() ধ্রুব { প্রত্যাগম self().escaped(); }
	bool begins_with(ধ্রুব বর্ণ *s) ধ্রুব {
		ধ্রুব TwinePool *pool = self().pool();
		প্রত্যাগম pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ধ্রুব বর্ণ *s) ধ্রুব { প্রত্যাগম str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ধ্রুব {
		প্রত্যাগম self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ধ্রুব pool<IdString> &rhs) ধ্রুব { প্রত্যাগম self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ধ্রুব {
		প্রত্যাগম self().escaped().substr(pos, len);
	}
	size_t size() ধ্রুব {
		ধ্রুব TwinePool *pool = self().pool();
		প্রত্যাগম pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ধ্রুব বর্ণ *p) ধ্রুব { প্রত্যাগম self().escaped().find(p) != std::string::npos; }
	বর্ণ operator[](পূর্ণাঙ্ক n) ধ্রুব { প্রত্যাগম self().escaped()[n]; }
	bool lt_by_name(ধ্রুব Derived &rhs) ধ্রুব {
		ধ্রুব TwinePool *pool = self().pool();
		যদি (pool == nullptr)
			প্রত্যাগম self().escaped() < rhs.escaped();
		প্রত্যাগম pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ধ্রুব Derived &lhs, ধ্রুব Derived &rhs) { প্রত্যাগম lhs.ref() == rhs.ref(); }
	friend bool operator==(ধ্রুব Derived &lhs, IdString rhs) { প্রত্যাগম lhs.ref() == rhs; }
	friend bool operator==(ধ্রুব Derived &lhs, NullIdString) { প্রত্যাগম lhs.ref() == IdString::Null; }
	friend bool operator==(ধ্রুব Derived &lhs, ধ্রুব std::string &rhs) {
		ধ্রুব TwinePool *pool = lhs.pool();
		প্রত্যাগম pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ধ্রুব Derived &lhs, ধ্রুব Derived &rhs) { প্রত্যাগম lhs.ref() < rhs.ref(); }
গুপ্ত:
	ধ্রুব Derived &self() ধ্রুব { প্রত্যাগম *static_cast<ধ্রুব Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
অন্তঃস্থ bool operator==(ধ্রুব A &lhs, ধ্রুব B &rhs) { প্রত্যাগম lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { প্রত্যাগম std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { প্রত্যাগম std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { প্রত্যাগম std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
সংরচনা OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = স্বভাব;
	OwnedId(ধ্রুব OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ধ্রুব { প্রত্যাগম id_; }
	ধ্রুব TwinePool *pool() ধ্রুব;
	OwnedId &operator=(IdString id) { id_ = id; প্রত্যাগম *this; }
	OwnedId &operator=(ধ্রুব OwnedId &other) { প্রত্যাগম *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { প্রত্যাগম *this = other.ref(); }
	friend শূন্য swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
গুপ্ত:
	ধ্রুব Owner *owner() ধ্রুব;
	IdString id_;
};

সংরচনা PooledName : WrappedIdBase<PooledName> {
	PooledName() = স্বভাব;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ধ্রুব TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ধ্রুব Design *design, IdString id);
	PooledName(ধ্রুব Module *module, IdString id);
	template<typename D> PooledName(ধ্রুব WrappedIdBase<D> &masq)
		: pool_(static_cast<ধ্রুব D &>(masq).pool()),
		  id_(static_cast<ধ্রুব D &>(masq).ref()) {}
	IdString ref() ধ্রুব { প্রত্যাগম id_; }
	ধ্রুব TwinePool *pool() ধ্রুব { প্রত্যাগম pool_; }
গুপ্ত:
	ধ্রুব TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	সংরচনা masq_hash_ops {
		স্থির অন্তঃস্থ bool cmp(ধ্রুব T &a, ধ্রুব T &b) { প্রত্যাগম a == b; }
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash(ধ্রুব T &a) {
			প্রত্যাগম hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুব T &a, Hasher h) {
			প্রত্যাগম hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	সংরচনা hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> সংরচনা hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
