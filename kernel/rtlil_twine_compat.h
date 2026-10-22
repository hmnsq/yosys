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

अन्तःस्थ std::string render_escaped(अचर TwinePool *pool, IdString id) {
	यदि (id == IdString::Null)
		फर्काऊ std::string();
	फर्काऊ pool ? pool->str(id) : ID::str(id);
}

अन्तःस्थ std::string render_unescaped(अचर TwinePool *pool, IdString id) {
	यदि (id == IdString::Null)
		फर्काऊ std::string();
	फर्काऊ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ढाँचा<typename Derived>
संरचना WrappedIdBase {
	चालक IdString() अचर { फर्काऊ self().ref(); }
	चालक std::string() अचर { फर्काऊ self().escaped(); }
	std::string escaped() अचर { फर्काऊ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() अचर { फर्काऊ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() अचर { फर्काऊ self().ref().isPublic(); }
	bool empty() अचर { फर्काऊ self().ref() == IdString::Null; }
	std::string str() अचर { फर्काऊ self().escaped(); }
	bool begins_with(अचर वर्ण *s) अचर {
		अचर TwinePool *pool = self().pool();
		फर्काऊ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(अचर वर्ण *s) अचर { फर्काऊ str().ends_with(s); }
	ढाँचा<typename... Ts> bool in(Ts &&...args) अचर {
		फर्काऊ self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(अचर pool<IdString> &rhs) अचर { फर्काऊ self().ref().in(rhs); }
	std::string substr(माप_प्रकार pos = 0, माप_प्रकार len = std::string::npos) अचर {
		फर्काऊ self().escaped().substr(pos, len);
	}
	माप_प्रकार size() अचर {
		अचर TwinePool *pool = self().pool();
		फर्काऊ pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(अचर वर्ण *p) अचर { फर्काऊ self().escaped().find(p) != std::string::npos; }
	वर्ण चालक[](पूर्णाङ्क n) अचर { फर्काऊ self().escaped()[n]; }
	bool lt_by_name(अचर Derived &rhs) अचर {
		अचर TwinePool *pool = self().pool();
		यदि (pool == nullptr)
			फर्काऊ self().escaped() < rhs.escaped();
		फर्काऊ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	मित्र bool चालक==(अचर Derived &lhs, अचर Derived &rhs) { फर्काऊ lhs.ref() == rhs.ref(); }
	मित्र bool चालक==(अचर Derived &lhs, IdString rhs) { फर्काऊ lhs.ref() == rhs; }
	मित्र bool चालक==(अचर Derived &lhs, NullIdString) { फर्काऊ lhs.ref() == IdString::Null; }
	मित्र bool चालक==(अचर Derived &lhs, अचर std::string &rhs) {
		अचर TwinePool *pool = lhs.pool();
		फर्काऊ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	मित्र bool चालक<(अचर Derived &lhs, अचर Derived &rhs) { फर्काऊ lhs.ref() < rhs.ref(); }
गुप्त:
	अचर Derived &self() अचर { फर्काऊ *static_cast<अचर Derived *>(this); }
};

ढाँचा<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ढाँचा<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
अन्तःस्थ bool चालक==(अचर A &lhs, अचर B &rhs) { फर्काऊ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ढाँचा<IsWrappedId A, typename B>
स्वतः make_pair(A &&a, B &&b) { फर्काऊ std::make_pair(a.ref(), std::forward<B>(b)); }
ढाँचा<typename A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { फर्काऊ std::make_pair(std::forward<A>(a), b.ref()); }
ढाँचा<IsWrappedId A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { फर्काऊ std::make_pair(a.ref(), b.ref()); }

ढाँचा<typename Owner, typename Slot>
संरचना OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = स्वभाव;
	OwnedId(अचर OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() अचर { फर्काऊ id_; }
	अचर TwinePool *pool() अचर;
	OwnedId &चालक=(IdString id) { id_ = id; फर्काऊ *this; }
	OwnedId &चालक=(अचर OwnedId &other) { फर्काऊ *this = other.ref(); }
	OwnedId &चालक=(OwnedId &&other) { फर्काऊ *this = other.ref(); }
	मित्र शून्य swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
गुप्त:
	अचर Owner *owner() अचर;
	IdString id_;
};

संरचना PooledName : WrappedIdBase<PooledName> {
	PooledName() = स्वभाव;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(अचर TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(अचर Design *design, IdString id);
	PooledName(अचर Module *module, IdString id);
	ढाँचा<typename D> PooledName(अचर WrappedIdBase<D> &masq)
		: pool_(static_cast<अचर D &>(masq).pool()),
		  id_(static_cast<अचर D &>(masq).ref()) {}
	IdString ref() अचर { फर्काऊ id_; }
	अचर TwinePool *pool() अचर { फर्काऊ pool_; }
गुप्त:
	अचर TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ढाँचा<typename T>
	संरचना masq_hash_ops {
		स्थिर अन्तःस्थ bool cmp(अचर T &a, अचर T &b) { फर्काऊ a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(अचर T &a) {
			फर्काऊ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(अचर T &a, Hasher h) {
			फर्काऊ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ढाँचा<typename Owner, typename Slot>
	संरचना hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	ढाँचा<> संरचना hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
