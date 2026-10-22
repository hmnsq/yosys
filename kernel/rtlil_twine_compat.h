#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> ಸಂರಚನಾ WrappedIdBase;
ಸಂರಚನಾ NameSlot;
ಸಂರಚನಾ CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> ಸಂರಚನಾ OwnedId;
ಸಂರಚನಾ PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

ಅನ್ತಃಸ್ಥ std::string render_escaped(ಧ್ರುವ TwinePool *pool, IdString id) {
	ಯದಿ (id == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ std::string();
	ಪ್ರತ್ಯಾಗಮ pool ? pool->str(id) : ID::str(id);
}

ಅನ್ತಃಸ್ಥ std::string render_unescaped(ಧ್ರುವ TwinePool *pool, IdString id) {
	ಯದಿ (id == IdString::Null)
		ಪ್ರತ್ಯಾಗಮ std::string();
	ಪ್ರತ್ಯಾಗಮ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
ಸಂರಚನಾ WrappedIdBase {
	operator IdString() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().ref(); }
	operator std::string() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().escaped(); }
	std::string escaped() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().ref().isPublic(); }
	bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().ref() == IdString::Null; }
	std::string str() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().escaped(); }
	bool begins_with(ಧ್ರುವ ವರ್ಣ *s) ಧ್ರುವ {
		ಧ್ರುವ TwinePool *pool = self().pool();
		ಪ್ರತ್ಯಾಗಮ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ಧ್ರುವ ವರ್ಣ *s) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ಧ್ರುವ pool<IdString> &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ self().escaped().substr(pos, len);
	}
	size_t size() ಧ್ರುವ {
		ಧ್ರುವ TwinePool *pool = self().pool();
		ಪ್ರತ್ಯಾಗಮ pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ಧ್ರುವ ವರ್ಣ *p) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().escaped().find(p) != std::string::npos; }
	ವರ್ಣ operator[](ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ self().escaped()[n]; }
	bool lt_by_name(ಧ್ರುವ Derived &rhs) ಧ್ರುವ {
		ಧ್ರುವ TwinePool *pool = self().pool();
		ಯದಿ (pool == nullptr)
			ಪ್ರತ್ಯಾಗಮ self().escaped() < rhs.escaped();
		ಪ್ರತ್ಯಾಗಮ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ಧ್ರುವ Derived &lhs, ಧ್ರುವ Derived &rhs) { ಪ್ರತ್ಯಾಗಮ lhs.ref() == rhs.ref(); }
	friend bool operator==(ಧ್ರುವ Derived &lhs, IdString rhs) { ಪ್ರತ್ಯಾಗಮ lhs.ref() == rhs; }
	friend bool operator==(ಧ್ರುವ Derived &lhs, NullIdString) { ಪ್ರತ್ಯಾಗಮ lhs.ref() == IdString::Null; }
	friend bool operator==(ಧ್ರುವ Derived &lhs, ಧ್ರುವ std::string &rhs) {
		ಧ್ರುವ TwinePool *pool = lhs.pool();
		ಪ್ರತ್ಯಾಗಮ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ಧ್ರುವ Derived &lhs, ಧ್ರುವ Derived &rhs) { ಪ್ರತ್ಯಾಗಮ lhs.ref() < rhs.ref(); }
ಗುಪ್ತ:
	ಧ್ರುವ Derived &self() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *static_cast<ಧ್ರುವ Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
ಅನ್ತಃಸ್ಥ bool operator==(ಧ್ರುವ A &lhs, ಧ್ರುವ B &rhs) { ಪ್ರತ್ಯಾಗಮ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { ಪ್ರತ್ಯಾಗಮ std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ಪ್ರತ್ಯಾಗಮ std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ಪ್ರತ್ಯಾಗಮ std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
ಸಂರಚನಾ OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = ಸ್ವಭಾವ;
	OwnedId(ಧ್ರುವ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ id_; }
	ಧ್ರುವ TwinePool *pool() ಧ್ರುವ;
	OwnedId &operator=(IdString id) { id_ = id; ಪ್ರತ್ಯಾಗಮ *this; }
	OwnedId &operator=(ಧ್ರುವ OwnedId &other) { ಪ್ರತ್ಯಾಗಮ *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { ಪ್ರತ್ಯಾಗಮ *this = other.ref(); }
	friend ಶೂನ್ಯ swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
ಗುಪ್ತ:
	ಧ್ರುವ Owner *owner() ಧ್ರುವ;
	IdString id_;
};

ಸಂರಚನಾ PooledName : WrappedIdBase<PooledName> {
	PooledName() = ಸ್ವಭಾವ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ಧ್ರುವ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ಧ್ರುವ Design *design, IdString id);
	PooledName(ಧ್ರುವ Module *module, IdString id);
	template<typename D> PooledName(ಧ್ರುವ WrappedIdBase<D> &masq)
		: pool_(static_cast<ಧ್ರುವ D &>(masq).pool()),
		  id_(static_cast<ಧ್ರುವ D &>(masq).ref()) {}
	IdString ref() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ id_; }
	ಧ್ರುವ TwinePool *pool() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ pool_; }
ಗುಪ್ತ:
	ಧ್ರುವ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	ಸಂರಚನಾ masq_hash_ops {
		ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ T &a, ಧ್ರುವ T &b) { ಪ್ರತ್ಯಾಗಮ a == b; }
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash(ಧ್ರುವ T &a) {
			ಪ್ರತ್ಯಾಗಮ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ T &a, Hasher h) {
			ಪ್ರತ್ಯಾಗಮ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	ಸಂರಚನಾ hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> ಸಂರಚನಾ hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
