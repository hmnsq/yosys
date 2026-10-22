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

ओळीत std::string render_escaped(स्थिरांक TwinePool *pool, IdString id) {
	जर (id == IdString::Null)
		परत std::string();
	परत pool ? pool->str(id) : ID::str(id);
}

ओळीत std::string render_unescaped(स्थिरांक TwinePool *pool, IdString id) {
	जर (id == IdString::Null)
		परत std::string();
	परत pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ढाँचा<typename Derived>
संरचना WrappedIdBase {
	चालक IdString() स्थिरांक { परत self().ref(); }
	चालक std::string() स्थिरांक { परत self().escaped(); }
	std::string escaped() स्थिरांक { परत wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() स्थिरांक { परत wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() स्थिरांक { परत self().ref().isPublic(); }
	bool empty() स्थिरांक { परत self().ref() == IdString::Null; }
	std::string str() स्थिरांक { परत self().escaped(); }
	bool begins_with(स्थिरांक अक्षर *s) स्थिरांक {
		स्थिरांक TwinePool *pool = self().pool();
		परत pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(स्थिरांक अक्षर *s) स्थिरांक { परत str().ends_with(s); }
	ढाँचा<typename... Ts> bool in(Ts &&...args) स्थिरांक {
		परत self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(स्थिरांक pool<IdString> &rhs) स्थिरांक { परत self().ref().in(rhs); }
	std::string substr(माप_प्रकार pos = 0, माप_प्रकार len = std::string::npos) स्थिरांक {
		परत self().escaped().substr(pos, len);
	}
	माप_प्रकार size() स्थिरांक {
		स्थिरांक TwinePool *pool = self().pool();
		परत pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(स्थिरांक अक्षर *p) स्थिरांक { परत self().escaped().find(p) != std::string::npos; }
	अक्षर चालक[](पूर्णांक n) स्थिरांक { परत self().escaped()[n]; }
	bool lt_by_name(स्थिरांक Derived &rhs) स्थिरांक {
		स्थिरांक TwinePool *pool = self().pool();
		जर (pool == nullptr)
			परत self().escaped() < rhs.escaped();
		परत pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	मित्र bool चालक==(स्थिरांक Derived &lhs, स्थिरांक Derived &rhs) { परत lhs.ref() == rhs.ref(); }
	मित्र bool चालक==(स्थिरांक Derived &lhs, IdString rhs) { परत lhs.ref() == rhs; }
	मित्र bool चालक==(स्थिरांक Derived &lhs, NullIdString) { परत lhs.ref() == IdString::Null; }
	मित्र bool चालक==(स्थिरांक Derived &lhs, स्थिरांक std::string &rhs) {
		स्थिरांक TwinePool *pool = lhs.pool();
		परत pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	मित्र bool चालक<(स्थिरांक Derived &lhs, स्थिरांक Derived &rhs) { परत lhs.ref() < rhs.ref(); }
खाजगी:
	स्थिरांक Derived &self() स्थिरांक { परत *static_cast<स्थिरांक Derived *>(this); }
};

ढाँचा<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ढाँचा<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
ओळीत bool चालक==(स्थिरांक A &lhs, स्थिरांक B &rhs) { परत lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ढाँचा<IsWrappedId A, typename B>
स्वतः make_pair(A &&a, B &&b) { परत std::make_pair(a.ref(), std::forward<B>(b)); }
ढाँचा<typename A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { परत std::make_pair(std::forward<A>(a), b.ref()); }
ढाँचा<IsWrappedId A, IsWrappedId B>
स्वतः make_pair(A &&a, B &&b) { परत std::make_pair(a.ref(), b.ref()); }

ढाँचा<typename Owner, typename Slot>
संरचना OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = पूर्वनिर्धारित;
	OwnedId(स्थिरांक OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() स्थिरांक { परत id_; }
	स्थिरांक TwinePool *pool() स्थिरांक;
	OwnedId &चालक=(IdString id) { id_ = id; परत *this; }
	OwnedId &चालक=(स्थिरांक OwnedId &other) { परत *this = other.ref(); }
	OwnedId &चालक=(OwnedId &&other) { परत *this = other.ref(); }
	मित्र शून्य swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
खाजगी:
	स्थिरांक Owner *owner() स्थिरांक;
	IdString id_;
};

संरचना PooledName : WrappedIdBase<PooledName> {
	PooledName() = पूर्वनिर्धारित;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(स्थिरांक TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(स्थिरांक Design *design, IdString id);
	PooledName(स्थिरांक Module *module, IdString id);
	ढाँचा<typename D> PooledName(स्थिरांक WrappedIdBase<D> &masq)
		: pool_(static_cast<स्थिरांक D &>(masq).pool()),
		  id_(static_cast<स्थिरांक D &>(masq).ref()) {}
	IdString ref() स्थिरांक { परत id_; }
	स्थिरांक TwinePool *pool() स्थिरांक { परत pool_; }
खाजगी:
	स्थिरांक TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ढाँचा<typename T>
	संरचना masq_hash_ops {
		स्थिर ओळीत bool cmp(स्थिरांक T &a, स्थिरांक T &b) { परत a == b; }
		[[nodiscard]] स्थिर ओळीत Hasher hash(स्थिरांक T &a) {
			परत hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक T &a, Hasher h) {
			परत hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ढाँचा<typename Owner, typename Slot>
	संरचना hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	ढाँचा<> संरचना hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
