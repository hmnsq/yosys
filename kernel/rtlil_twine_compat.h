#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> ഘടന WrappedIdBase;
ഘടന NameSlot;
ഘടന CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> ഘടന OwnedId;
ഘടന PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

വരിയിൽ std::string render_escaped(സ്ഥിരം TwinePool *pool, IdString id) {
	എങ്കിൽ (id == IdString::Null)
		തിരികെ std::string();
	തിരികെ pool ? pool->str(id) : ID::str(id);
}

വരിയിൽ std::string render_unescaped(സ്ഥിരം TwinePool *pool, IdString id) {
	എങ്കിൽ (id == IdString::Null)
		തിരികെ std::string();
	തിരികെ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
ഘടന WrappedIdBase {
	operator IdString() സ്ഥിരം { തിരികെ self().ref(); }
	operator std::string() സ്ഥിരം { തിരികെ self().escaped(); }
	std::string escaped() സ്ഥിരം { തിരികെ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() സ്ഥിരം { തിരികെ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() സ്ഥിരം { തിരികെ self().ref().isPublic(); }
	bool empty() സ്ഥിരം { തിരികെ self().ref() == IdString::Null; }
	std::string str() സ്ഥിരം { തിരികെ self().escaped(); }
	bool begins_with(സ്ഥിരം അക്ഷരം *s) സ്ഥിരം {
		സ്ഥിരം TwinePool *pool = self().pool();
		തിരികെ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(സ്ഥിരം അക്ഷരം *s) സ്ഥിരം { തിരികെ str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) സ്ഥിരം {
		തിരികെ self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(സ്ഥിരം pool<IdString> &rhs) സ്ഥിരം { തിരികെ self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) സ്ഥിരം {
		തിരികെ self().escaped().substr(pos, len);
	}
	size_t size() സ്ഥിരം {
		സ്ഥിരം TwinePool *pool = self().pool();
		തിരികെ pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(സ്ഥിരം അക്ഷരം *p) സ്ഥിരം { തിരികെ self().escaped().find(p) != std::string::npos; }
	അക്ഷരം operator[](പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ self().escaped()[n]; }
	bool lt_by_name(സ്ഥിരം Derived &rhs) സ്ഥിരം {
		സ്ഥിരം TwinePool *pool = self().pool();
		എങ്കിൽ (pool == nullptr)
			തിരികെ self().escaped() < rhs.escaped();
		തിരികെ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(സ്ഥിരം Derived &lhs, സ്ഥിരം Derived &rhs) { തിരികെ lhs.ref() == rhs.ref(); }
	friend bool operator==(സ്ഥിരം Derived &lhs, IdString rhs) { തിരികെ lhs.ref() == rhs; }
	friend bool operator==(സ്ഥിരം Derived &lhs, NullIdString) { തിരികെ lhs.ref() == IdString::Null; }
	friend bool operator==(സ്ഥിരം Derived &lhs, സ്ഥിരം std::string &rhs) {
		സ്ഥിരം TwinePool *pool = lhs.pool();
		തിരികെ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(സ്ഥിരം Derived &lhs, സ്ഥിരം Derived &rhs) { തിരികെ lhs.ref() < rhs.ref(); }
സ്വകാര്യ:
	സ്ഥിരം Derived &self() സ്ഥിരം { തിരികെ *static_cast<സ്ഥിരം Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
വരിയിൽ bool operator==(സ്ഥിരം A &lhs, സ്ഥിരം B &rhs) { തിരികെ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { തിരികെ std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { തിരികെ std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { തിരികെ std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
ഘടന OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = സ്ഥിരസ്ഥിതി;
	OwnedId(സ്ഥിരം OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() സ്ഥിരം { തിരികെ id_; }
	സ്ഥിരം TwinePool *pool() സ്ഥിരം;
	OwnedId &operator=(IdString id) { id_ = id; തിരികെ *this; }
	OwnedId &operator=(സ്ഥിരം OwnedId &other) { തിരികെ *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { തിരികെ *this = other.ref(); }
	friend ശൂന്യം swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
സ്വകാര്യ:
	സ്ഥിരം Owner *owner() സ്ഥിരം;
	IdString id_;
};

ഘടന PooledName : WrappedIdBase<PooledName> {
	PooledName() = സ്ഥിരസ്ഥിതി;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(സ്ഥിരം TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(സ്ഥിരം Design *design, IdString id);
	PooledName(സ്ഥിരം Module *module, IdString id);
	template<typename D> PooledName(സ്ഥിരം WrappedIdBase<D> &masq)
		: pool_(static_cast<സ്ഥിരം D &>(masq).pool()),
		  id_(static_cast<സ്ഥിരം D &>(masq).ref()) {}
	IdString ref() സ്ഥിരം { തിരികെ id_; }
	സ്ഥിരം TwinePool *pool() സ്ഥിരം { തിരികെ pool_; }
സ്വകാര്യ:
	സ്ഥിരം TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	ഘടന masq_hash_ops {
		സ്ഥിതം വരിയിൽ bool cmp(സ്ഥിരം T &a, സ്ഥിരം T &b) { തിരികെ a == b; }
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash(സ്ഥിരം T &a) {
			തിരികെ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] സ്ഥിതം വരിയിൽ Hasher hash_into(സ്ഥിരം T &a, Hasher h) {
			തിരികെ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	ഘടന hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> ഘടന hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
