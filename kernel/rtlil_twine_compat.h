#अगर_अघोषित RTLIL_TWINE_COMPAT_H
#घोषणा RTLIL_TWINE_COMPAT_H

namespace RTLIL {

ढाँचा<typename Derived> काष्ठा WrappedIdBase;
काष्ठा NameSlot;
काष्ठा CellTypeSlot;
ढाँचा<typename Owner, typename Slot = NameSlot> काष्ठा OwnedId;
काष्ठा PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

अंतरभूत std::string render_escaped(स्थिर TwinePool *pool, IdString id) {
	अगर (id == IdString::Null)
		वापस std::string();
	वापस pool ? pool->str(id) : ID::str(id);
}

अंतरभूत std::string render_unescaped(स्थिर TwinePool *pool, IdString id) {
	अगर (id == IdString::Null)
		वापस std::string();
	वापस pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ढाँचा<typename Derived>
काष्ठा WrappedIdBase {
	चालक IdString() स्थिर { वापस self().ref(); }
	चालक std::string() स्थिर { वापस self().escaped(); }
	std::string escaped() स्थिर { वापस wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() स्थिर { वापस wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() स्थिर { वापस self().ref().isPublic(); }
	bool empty() स्थिर { वापस self().ref() == IdString::Null; }
	std::string str() स्थिर { वापस self().escaped(); }
	bool begins_with(स्थिर अक्षर *s) स्थिर {
		स्थिर TwinePool *pool = self().pool();
		वापस pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(स्थिर अक्षर *s) स्थिर { वापस str().ends_with(s); }
	ढाँचा<typename... Ts> bool in(Ts &&...args) स्थिर {
		वापस self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(स्थिर pool<IdString> &rhs) स्थिर { वापस self().ref().in(rhs); }
	std::string substr(माप_प्रकार pos = 0, माप_प्रकार len = std::string::npos) स्थिर {
		वापस self().escaped().substr(pos, len);
	}
	माप_प्रकार आकार() स्थिर {
		स्थिर TwinePool *pool = self().pool();
		वापस pool ? pool->str_आकार(self().ref()) : self().escaped().आकार();
	}
	bool contains(स्थिर अक्षर *p) स्थिर { वापस self().escaped().find(p) != std::string::npos; }
	अक्षर चालक[](पूर्णांक n) स्थिर { वापस self().escaped()[n]; }
	bool lt_by_name(स्थिर Derived &rhs) स्थिर {
		स्थिर TwinePool *pool = self().pool();
		अगर (pool == nullptr)
			वापस self().escaped() < rhs.escaped();
		वापस pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	मित्र bool चालक==(स्थिर Derived &lhs, स्थिर Derived &rhs) { वापस lhs.ref() == rhs.ref(); }
	मित्र bool चालक==(स्थिर Derived &lhs, IdString rhs) { वापस lhs.ref() == rhs; }
	मित्र bool चालक==(स्थिर Derived &lhs, NullIdString) { वापस lhs.ref() == IdString::Null; }
	मित्र bool चालक==(स्थिर Derived &lhs, स्थिर std::string &rhs) {
		स्थिर TwinePool *pool = lhs.pool();
		वापस pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	मित्र bool चालक<(स्थिर Derived &lhs, स्थिर Derived &rhs) { वापस lhs.ref() < rhs.ref(); }
निजी:
	स्थिर Derived &self() स्थिर { वापस *static_cast<स्थिर Derived *>(this); }
};

ढाँचा<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ढाँचा<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
अंतरभूत bool चालक==(स्थिर A &lhs, स्थिर B &rhs) { वापस lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ढाँचा<IsWrappedId A, typename B>
स्वतः make_pair(A &&a, B &&b) { वापस std::make_pair(a.ref(), std::forward<B>(b)); }
ढाँचा<typename A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { वापस std::make_pair(std::forward<A>(a), b.ref()); }
ढाँचा<IsWrappedId A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { वापस std::make_pair(a.ref(), b.ref()); }

ढाँचा<typename Owner, typename Slot>
काष्ठा OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = शेष;
	OwnedId(स्थिर OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() स्थिर { वापस id_; }
	स्थिर TwinePool *pool() स्थिर;
	OwnedId &चालक=(IdString id) { id_ = id; वापस *this; }
	OwnedId &चालक=(स्थिर OwnedId &other) { वापस *this = other.ref(); }
	OwnedId &चालक=(OwnedId &&other) { वापस *this = other.ref(); }
	मित्र व्योम swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
निजी:
	स्थिर Owner *owner() स्थिर;
	IdString id_;
};

काष्ठा PooledName : WrappedIdBase<PooledName> {
	PooledName() = शेष;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(स्थिर TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(स्थिर Design *design, IdString id);
	PooledName(स्थिर Module *module, IdString id);
	ढाँचा<typename D> PooledName(स्थिर WrappedIdBase<D> &masq)
		: pool_(static_cast<स्थिर D &>(masq).pool()),
		  id_(static_cast<स्थिर D &>(masq).ref()) {}
	IdString ref() स्थिर { वापस id_; }
	स्थिर TwinePool *pool() स्थिर { वापस pool_; }
निजी:
	स्थिर TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ढाँचा<typename T>
	काष्ठा masq_hash_ops {
		अटल अंतरभूत bool cmp(स्थिर T &a, स्थिर T &b) { वापस a == b; }
		[[nodiscard]] अटल अंतरभूत Hasher hash(स्थिर T &a) {
			वापस hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर T &a, Hasher h) {
			वापस hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ढाँचा<typename Owner, typename Slot>
	काष्ठा hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	ढाँचा<> काष्ठा hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#पूर्ण_अगर
