#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> بنية WrappedIdBase;
بنية NameSlot;
بنية CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> بنية OwnedId;
بنية PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

مضمن std::string render_escaped(ثابت TwinePool *pool, IdString id) {
	אם (id == IdString::Null)
		החזר std::string();
	החזר pool ? pool->str(id) : ID::str(id);
}

مضمن std::string render_unescaped(ثابت TwinePool *pool, IdString id) {
	אם (id == IdString::Null)
		החזר std::string();
	החזר pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
بنية WrappedIdBase {
	operator IdString() ثابت { החזר self().ref(); }
	operator std::string() ثابت { החזר self().escaped(); }
	std::string escaped() ثابت { החזר wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { החזר wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ثابت { החזר self().ref().isPublic(); }
	bool empty() ثابت { החזר self().ref() == IdString::Null; }
	std::string str() ثابت { החזר self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		החזר pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ثابت حرف *s) ثابت { החזר str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ثابت {
		החזר self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ثابت pool<IdString> &rhs) ثابت { החזר self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ثابت {
		החזר self().escaped().substr(pos, len);
	}
	size_t size() ثابت {
		ثابت TwinePool *pool = self().pool();
		החזר pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ثابت حرف *p) ثابت { החזר self().escaped().find(p) != std::string::npos; }
	حرف operator[](صحيح n) ثابت { החזר self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		אם (pool == nullptr)
			החזר self().escaped() < rhs.escaped();
		החזר pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ثابت Derived &lhs, ثابت Derived &rhs) { החזר lhs.ref() == rhs.ref(); }
	friend bool operator==(ثابت Derived &lhs, IdString rhs) { החזר lhs.ref() == rhs; }
	friend bool operator==(ثابت Derived &lhs, NullIdString) { החזר lhs.ref() == IdString::Null; }
	friend bool operator==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		החזר pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ثابت Derived &lhs, ثابت Derived &rhs) { החזר lhs.ref() < rhs.ref(); }
خاص:
	ثابت Derived &self() ثابت { החזר *static_cast<ثابت Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
مضمن bool operator==(ثابت A &lhs, ثابت B &rhs) { החזר lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { החזר std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { החזר std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { החזר std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
بنية OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = افتراضي;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { החזר id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &operator=(IdString id) { id_ = id; החזר *this; }
	OwnedId &operator=(ثابت OwnedId &other) { החזר *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { החזר *this = other.ref(); }
	friend فراغ swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
خاص:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

بنية PooledName : WrappedIdBase<PooledName> {
	PooledName() = افتراضي;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	template<typename D> PooledName(ثابت WrappedIdBase<D> &masq)
		: pool_(static_cast<ثابت D &>(masq).pool()),
		  id_(static_cast<ثابت D &>(masq).ref()) {}
	IdString ref() ثابت { החזר id_; }
	ثابت TwinePool *pool() ثابت { החזר pool_; }
خاص:
	ثابت TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	بنية masq_hash_ops {
		ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) { החזר a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت T &a) {
			החזר hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
			החזר hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	بنية hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> بنية hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
