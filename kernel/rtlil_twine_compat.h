#यदि_अपरिभाषितम् RTLIL_TWINE_COMPAT_H
#परिभाषय RTLIL_TWINE_COMPAT_H

namespace RTLIL {

प्रतिरूपम्<typename Derived> संरचना Wrapped_Id_आधारः;
संरचना NameSlot;
संरचना CellTypeSlot;
प्रतिरूपम्<typename Owner, typename Slot = NameSlot> संरचना OwnedId;
संरचना PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using स्मृतिः_Name_Id = OwnedId<स्मृतिः>;
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

प्रतिरूपम्<typename Derived>
संरचना Wrapped_Id_आधारः {
	संक्रियकः IdString() ध्रुव { प्रत्यागम self().ref(); }
	संक्रियकः std::string() ध्रुव { प्रत्यागम self().escaped(); }
	std::string escaped() ध्रुव { प्रत्यागम wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ध्रुव { प्रत्यागम wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_लौकिकम्() ध्रुव { प्रत्यागम self().ref().is_लौकिकम्(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम self().ref() == IdString::Null; }
	std::string str() ध्रुव { प्रत्यागम self().escaped(); }
	bool begins_with(ध्रुव वर्ण *s) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		प्रत्यागम pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool अन्ताः_with(ध्रुव वर्ण *s) ध्रुव { प्रत्यागम str().अन्ताः_with(s); }
	प्रतिरूपम्<typename... Ts> bool अन्तर्(Ts &&...तर्काः_2) ध्रुव {
		प्रत्यागम self().ref().अन्तर्(std::forward<Ts>(तर्काः_2)...);
	}
	bool अन्तर्(ध्रुव pool<IdString> &rhs) ध्रुव { प्रत्यागम self().ref().अन्तर्(rhs); }
	std::string substr(आकार_t pos = 0, आकार_t len = std::string::npos) ध्रुव {
		प्रत्यागम self().escaped().substr(pos, len);
	}
	आकार_t आकार() ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		प्रत्यागम pool ? pool->str_आकार(self().ref()) : self().escaped().आकार();
	}
	bool अन्तर्भवति(ध्रुव वर्ण *p) ध्रुव { प्रत्यागम self().escaped().find(p) != std::string::npos; }
	वर्ण संक्रियकः[](पूर्णाङ्क n) ध्रुव { प्रत्यागम self().escaped()[n]; }
	bool lt_by_name(ध्रुव Derived &rhs) ध्रुव {
		ध्रुव TwinePool *pool = self().pool();
		यदि (pool == nullptr)
			प्रत्यागम self().escaped() < rhs.escaped();
		प्रत्यागम pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	मित्रम् bool संक्रियकः==(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { प्रत्यागम lhs.ref() == rhs.ref(); }
	मित्रम् bool संक्रियकः==(ध्रुव Derived &lhs, IdString rhs) { प्रत्यागम lhs.ref() == rhs; }
	मित्रम् bool संक्रियकः==(ध्रुव Derived &lhs, NullIdString) { प्रत्यागम lhs.ref() == IdString::Null; }
	मित्रम् bool संक्रियकः==(ध्रुव Derived &lhs, ध्रुव std::string &rhs) {
		ध्रुव TwinePool *pool = lhs.pool();
		प्रत्यागम pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	मित्रम् bool संक्रियकः<(ध्रुव Derived &lhs, ध्रुव Derived &rhs) { प्रत्यागम lhs.ref() < rhs.ref(); }
गुप्त:
	ध्रुव Derived &self() ध्रुव { प्रत्यागम *static_cast<ध्रुव Derived *>(this); }
};

प्रतिरूपम्<typename T>
concept IsWrappedId = std::is_आधारः_सम्बन्धि_v<Wrapped_Id_आधारः<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
प्रतिरूपम्<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
अन्तःस्थ bool संक्रियकः==(ध्रुव A &lhs, ध्रुव B &rhs) { प्रत्यागम lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
प्रतिरूपम्<IsWrappedId A, typename B>
स्वचालित make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(a.ref(), std::forward<B>(b)); }
प्रतिरूपम्<typename A, IsWrappedId B>
स्वचालित make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(std::forward<A>(a), b.ref()); }
प्रतिरूपम्<IsWrappedId A, IsWrappedId B>
स्वचालित make_pair(A &&a, B &&b) { प्रत्यागम std::make_pair(a.ref(), b.ref()); }

प्रतिरूपम्<typename Owner, typename Slot>
संरचना OwnedId : Wrapped_Id_आधारः<OwnedId<Owner, Slot>> {
	OwnedId() = स्वभाव;
	OwnedId(ध्रुव OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ध्रुव { प्रत्यागम id_; }
	ध्रुव TwinePool *pool() ध्रुव;
	OwnedId &संक्रियकः=(IdString id) { id_ = id; प्रत्यागम *this; }
	OwnedId &संक्रियकः=(ध्रुव OwnedId &other) { प्रत्यागम *this = other.ref(); }
	OwnedId &संक्रियकः=(OwnedId &&other) { प्रत्यागम *this = other.ref(); }
	मित्रम् शून्य swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
गुप्त:
	ध्रुव Owner *owner() ध्रुव;
	IdString id_;
};

संरचना PooledName : Wrapped_Id_आधारः<PooledName> {
	PooledName() = स्वभाव;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ध्रुव TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ध्रुव Design *design, IdString id);
	PooledName(ध्रुव Module *module, IdString id);
	प्रतिरूपम्<typename D> PooledName(ध्रुव Wrapped_Id_आधारः<D> &masq)
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
	प्रतिरूपम्<typename T>
	संरचना masq_hash_ops {
		स्थिर अन्तःस्थ bool cmp(ध्रुव T &a, ध्रुव T &b) { प्रत्यागम a == b; }
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव T &a) {
			प्रत्यागम hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव T &a, Hasher h) {
			प्रत्यागम hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	प्रतिरूपम्<typename Owner, typename Slot>
	संरचना hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	प्रतिरूपम्<> संरचना hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#यदि_अन्तः
