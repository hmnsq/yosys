#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> संरचना WrappedIdBase;
संरचना NameSlot;
संरचना CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> संरचना OwnedId;
संरचना PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

अन्तःस्थ std::string render_escaped(ध्रुव TwinePool *pool, IdString id) {
	सचे (id == IdString::Null)
		पटिनिवत्त std::string();
	पटिनिवत्त pool ? pool->str(id) : ID::str(id);
}

अन्तःस्थ std::string render_unescaped(ध्रुव TwinePool *pool, IdString id) {
	सचे (id == IdString::Null)
		पटिनिवत्त std::string();
	पटिनिवत्त pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
संरचना WrappedIdBase {
	operator IdString() ध्रुव { पटिनिवत्त self().ref(); }
	operator std::string() ध्रुव { पटिनिवत्त self().escaped(); }
	std::string escaped() ध्रुव { पटिनिवत्त wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ध्रुव { पटिनिवत्त wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ध्रुव { पटिनिवत्त self().ref().isPublic(); }
	bool empty() ध्रुव { पटिनिवत्त self().ref() == IdString::Null; }
	std::string str() ध्रुव { पटिनिवत्त self().escaped(); }
	bool begins_with(ध्रुव वर्ण *s) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		पटिनिवत्त pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ध्रुव वर्ण *s) ध्रुव { पटिनिवत्त str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ध्रुव {
		पटिनिवत्त self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ध्रुव pool<IdString> &rhs) ध्रुव { पटिनिवत्त self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ध्रुव {
		पटिनिवत्त self().escaped().substr(pos, len);
	}
	size_t size() ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		पटिनिवत्त pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ध्रुव वर्ण *p) ध्रुव { पटिनिवत्त self().escaped().find(p) != std::string::npos; }
	वर्ण operator[](पूर्णाङ्क n) ध्रुव { पटिनिवत्त self().escaped()[n]; }
	bool lt_by_name(ध्रुव Derived &rhs) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		सचे (pool == nullptr)
			पटिनिवत्त self().escaped() < rhs.escaped();
		पटिनिवत्त pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { पटिनिवत्त lhs.ref() == rhs.ref(); }
	friend bool operator==(ध्रुव Derived &lhs, IdString rhs) { पटिनिवत्त lhs.ref() == rhs; }
	friend bool operator==(ध्रुव Derived &lhs, NullIdString) { पटिनिवत्त lhs.ref() == IdString::Null; }
	friend bool operator==(ध्रुव Derived &lhs, ध्रुव std::string &rhs) {
		ध्रुव TwinePool *pool = lhs.pool();
		पटिनिवत्त pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { पटिनिवत्त lhs.ref() < rhs.ref(); }
गुप्त:
	ध्रुव Derived &self() ध्रुव { पटिनिवत्त *static_cast<ध्रुव Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
अन्तःस्थ bool operator==(ध्रुव A &lhs, ध्रुव B &rhs) { पटिनिवत्त lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { पटिनिवत्त std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { पटिनिवत्त std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { पटिनिवत्त std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
संरचना OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = स्वभाव;
	OwnedId(ध्रुव OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ध्रुव { पटिनिवत्त id_; }
	ध्रुव TwinePool *pool() ध्रुव;
	OwnedId &operator=(IdString id) { id_ = id; पटिनिवत्त *this; }
	OwnedId &operator=(ध्रुव OwnedId &other) { पटिनिवत्त *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { पटिनिवत्त *this = other.ref(); }
	friend शून्य swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
गुप्त:
	ध्रुव Owner *owner() ध्रुव;
	IdString id_;
};

संरचना PooledName : WrappedIdBase<PooledName> {
	PooledName() = स्वभाव;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ध्रुव TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ध्रुव Design *design, IdString id);
	PooledName(ध्रुव Module *module, IdString id);
	template<typename D> PooledName(ध्रुव WrappedIdBase<D> &masq)
		: pool_(static_cast<ध्रुव D &>(masq).pool()),
		  id_(static_cast<ध्रुव D &>(masq).ref()) {}
	IdString ref() ध्रुव { पटिनिवत्त id_; }
	ध्रुव TwinePool *pool() ध्रुव { पटिनिवत्त pool_; }
गुप्त:
	ध्रुव TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	संरचना masq_hash_ops {
		स्थिर अन्तःस्थ bool cmp(ध्रुव T &a, ध्रुव T &b) { पटिनिवत्त a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव T &a) {
			पटिनिवत्त hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T &a, Hasher h) {
			पटिनिवत्त hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	संरचना hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> संरचना hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
