#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

ढाँचा<typename Derived> संरचना WrappedIdBase;
संरचना NameSlot;
संरचना CellTypeSlot;
ढाँचा<typename Owner, typename Slot = NameSlot> संरचना OwnedId;
संरचना PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

अन्तःस्थ std::string render_escaped(ध्रुव TwinePool *pool, IdString id) {
	यदि (id == IdString::Null)
		प्रत्यागम std::string();
	प्रत्यागम pool ? pool->str(id) : ID::str(id);
}

अन्तःस्थ std::string render_unescaped(ध्रुव TwinePool *pool, IdString id) {
	यदि (id == IdString::Null)
		प्रत्यागम std::string();
	प्रत्यागम pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ढाँचा<typename Derived>
संरचना WrappedIdBase {
	चालक IdString() ध्रुव { प्रत्यागम self().ref(); }
	चालक std::string() ध्रुव { प्रत्यागम self().escaped(); }
	std::string escaped() ध्रुव { प्रत्यागम wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ध्रुव { प्रत्यागम wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ध्रुव { प्रत्यागम self().ref().isPublic(); }
	bool empty() ध्रुव { प्रत्यागम self().ref() == IdString::Null; }
	std::string str() ध्रुव { प्रत्यागम self().escaped(); }
	bool begins_with(ध्रुव वर्ण *s) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		प्रत्यागम pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ध्रुव वर्ण *s) ध्रुव { प्रत्यागम str().ends_with(s); }
	ढाँचा<typename... Ts> bool in(Ts &&...args) ध्रुव {
		प्रत्यागम self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ध्रुव pool<IdString> &rhs) ध्रुव { प्रत्यागम self().ref().in(rhs); }
	std::string substr(माप_प्रकार pos = 0, माप_प्रकार len = std::string::npos) ध्रुव {
		प्रत्यागम self().escaped().substr(pos, len);
	}
	माप_प्रकार size() ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		प्रत्यागम pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ध्रुव वर्ण *p) ध्रुव { प्रत्यागम self().escaped().find(p) != std::string::npos; }
	वर्ण चालक[](पूर्णाङ्क n) ध्रुव { प्रत्यागम self().escaped()[n]; }
	bool lt_by_name(ध्रुव Derived &rhs) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		यदि (pool == nullptr)
			प्रत्यागम self().escaped() < rhs.escaped();
		प्रत्यागम pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	मित्र bool चालक==(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { प्रत्यागम lhs.ref() == rhs.ref(); }
	मित्र bool चालक==(ध्रुव Derived &lhs, IdString rhs) { प्रत्यागम lhs.ref() == rhs; }
	मित्र bool चालक==(ध्रुव Derived &lhs, NullIdString) { प्रत्यागम lhs.ref() == IdString::Null; }
	मित्र bool चालक==(ध्रुव Derived &lhs, ध्रुव std::string &rhs) {
		ध्रुव TwinePool *pool = lhs.pool();
		प्रत्यागम pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	मित्र bool चालक<(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { प्रत्यागम lhs.ref() < rhs.ref(); }
गुप्त:
	ध्रुव Derived &self() ध्रुव { प्रत्यागम *static_cast<ध्रुव Derived *>(this); }
};

ढाँचा<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ढाँचा<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
अन्तःस्थ bool चालक==(ध्रुव A &lhs, ध्रुव B &rhs) { प्रत्यागम lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ढाँचा<IsWrappedId A, typename B>
स्वतः make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(a.ref(), std::forward<B>(b)); }
ढाँचा<typename A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(std::forward<A>(a), b.ref()); }
ढाँचा<IsWrappedId A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(a.ref(), b.ref()); }

ढाँचा<typename Owner, typename Slot>
संरचना OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = स्वभाव;
	OwnedId(ध्रुव OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ध्रुव { प्रत्यागम id_; }
	ध्रुव TwinePool *pool() ध्रुव;
	OwnedId &चालक=(IdString id) { id_ = id; प्रत्यागम *this; }
	OwnedId &चालक=(ध्रुव OwnedId &other) { प्रत्यागम *this = other.ref(); }
	OwnedId &चालक=(OwnedId &&other) { प्रत्यागम *this = other.ref(); }
	मित्र शून्य swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
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
	ढाँचा<typename D> PooledName(ध्रुव WrappedIdBase<D> &masq)
		: pool_(static_cast<ध्रुव D &>(masq).pool()),
		  id_(static_cast<ध्रुव D &>(masq).ref()) {}
	IdString ref() ध्रुव { प्रत्यागम id_; }
	ध्रुव TwinePool *pool() ध्रुव { प्रत्यागम pool_; }
गुप्त:
	ध्रुव TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ढाँचा<typename T>
	संरचना masq_hash_ops {
		स्थिर अन्तःस्थ bool cmp(ध्रुव T &a, ध्रुव T &b) { प्रत्यागम a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव T &a) {
			प्रत्यागम hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T &a, Hasher h) {
			प्रत्यागम hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ढाँचा<typename Owner, typename Slot>
	संरचना hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	ढाँचा<> संरचना hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
