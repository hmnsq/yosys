#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RTLIL_TWINE_COMPAT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTLIL_TWINE_COMPAT_H

namespace RTLIL {

ਸਾਂਚਾ<typename Derived> ਬਣਤਰ Wrapped_Id_ਅਧਾਰ;
ਬਣਤਰ Name_ਥਾਂ;
ਬਣਤਰ Cell_Type_ਥਾਂ;
ਸਾਂਚਾ<typename Owner, typename ਥਾਂ_2 = Name_ਥਾਂ> ਬਣਤਰ OwnedId;
ਬਣਤਰ PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using ਸਮ੍ਰਿਤੀ_Name_Id = OwnedId<ਸਮ੍ਰਿਤੀ>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_ਥਾਂ>;

namespace wrapped_idstring_detail {

ਸਤਰਅੰਦਰ std::string render_escaped(ਅਚਲ TwinePool *pool, IdString id) {
	ਜੇ (id == IdString::Null)
		ਵਾਪਸ std::string();
	ਵਾਪਸ pool ? pool->str(id) : ID::str(id);
}

ਸਤਰਅੰਦਰ std::string render_unescaped(ਅਚਲ TwinePool *pool, IdString id) {
	ਜੇ (id == IdString::Null)
		ਵਾਪਸ std::string();
	ਵਾਪਸ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ਸਾਂਚਾ<typename Derived>
ਬਣਤਰ Wrapped_Id_ਅਧਾਰ {
	ਸੰਚਾਲਕ IdString() ਅਚਲ { ਵਾਪਸ self().ref(); }
	ਸੰਚਾਲਕ std::string() ਅਚਲ { ਵਾਪਸ self().escaped(); }
	std::string escaped() ਅਚਲ { ਵਾਪਸ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ਅਚਲ { ਵਾਪਸ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_ਜਨਤਕ() ਅਚਲ { ਵਾਪਸ self().ref().is_ਜਨਤਕ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ self().ref() == IdString::Null; }
	std::string str() ਅਚਲ { ਵਾਪਸ self().escaped(); }
	bool begins_with(ਅਚਲ ਅੱਖਰ *s) ਅਚਲ {
		ਅਚਲ TwinePool *pool = self().pool();
		ਵਾਪਸ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ਸਿਰੇ_with(ਅਚਲ ਅੱਖਰ *s) ਅਚਲ { ਵਾਪਸ str().ਸਿਰੇ_with(s); }
	ਸਾਂਚਾ<typename... Ts> bool ਵਿੱਚ(Ts &&...ਦਲੀਲਾਂ_2) ਅਚਲ {
		ਵਾਪਸ self().ref().ਵਿੱਚ(std::forward<Ts>(ਦਲੀਲਾਂ_2)...);
	}
	bool ਵਿੱਚ(ਅਚਲ pool<IdString> &rhs) ਅਚਲ { ਵਾਪਸ self().ref().ਵਿੱਚ(rhs); }
	std::string substr(ਮਾਪ_t pos = 0, ਮਾਪ_t len = std::string::npos) ਅਚਲ {
		ਵਾਪਸ self().escaped().substr(pos, len);
	}
	ਮਾਪ_t ਮਾਪ() ਅਚਲ {
		ਅਚਲ TwinePool *pool = self().pool();
		ਵਾਪਸ pool ? pool->str_ਮਾਪ(self().ref()) : self().escaped().ਮਾਪ();
	}
	bool ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ ਅੱਖਰ *p) ਅਚਲ { ਵਾਪਸ self().escaped().ਲੱਭੋ(p) != std::string::npos; }
	ਅੱਖਰ ਸੰਚਾਲਕ[](ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ self().escaped()[n]; }
	bool lt_by_name(ਅਚਲ Derived &rhs) ਅਚਲ {
		ਅਚਲ TwinePool *pool = self().pool();
		ਜੇ (pool == nullptr)
			ਵਾਪਸ self().escaped() < rhs.escaped();
		ਵਾਪਸ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	ਮਿੱਤਰ bool ਸੰਚਾਲਕ==(ਅਚਲ Derived &lhs, ਅਚਲ Derived &rhs) { ਵਾਪਸ lhs.ref() == rhs.ref(); }
	ਮਿੱਤਰ bool ਸੰਚਾਲਕ==(ਅਚਲ Derived &lhs, IdString rhs) { ਵਾਪਸ lhs.ref() == rhs; }
	ਮਿੱਤਰ bool ਸੰਚਾਲਕ==(ਅਚਲ Derived &lhs, NullIdString) { ਵਾਪਸ lhs.ref() == IdString::Null; }
	ਮਿੱਤਰ bool ਸੰਚਾਲਕ==(ਅਚਲ Derived &lhs, ਅਚਲ std::string &rhs) {
		ਅਚਲ TwinePool *pool = lhs.pool();
		ਵਾਪਸ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	ਮਿੱਤਰ bool ਸੰਚਾਲਕ<(ਅਚਲ Derived &lhs, ਅਚਲ Derived &rhs) { ਵਾਪਸ lhs.ref() < rhs.ref(); }
ਨਿਜੀ:
	ਅਚਲ Derived &self() ਅਚਲ { ਵਾਪਸ *static_cast<ਅਚਲ Derived *>(this); }
};

ਸਾਂਚਾ<typename T>
concept IsWrappedId = std::is_ਅਧਾਰ_ਦਾ_v<Wrapped_Id_ਅਧਾਰ<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ਸਾਂਚਾ<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ A &lhs, ਅਚਲ B &rhs) { ਵਾਪਸ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ਸਾਂਚਾ<IsWrappedId A, typename B>
ਸਵੈਚਾਲਿਤ make_pair(A &&a, B &&b) { ਵਾਪਸ std::make_pair(a.ref(), std::forward<B>(b)); }
ਸਾਂਚਾ<typename A, IsWrappedId B>
ਸਵੈਚਾਲਿਤ make_pair(A &&a, B &&b) { ਵਾਪਸ std::make_pair(std::forward<A>(a), b.ref()); }
ਸਾਂਚਾ<IsWrappedId A, IsWrappedId B>
ਸਵੈਚਾਲਿਤ make_pair(A &&a, B &&b) { ਵਾਪਸ std::make_pair(a.ref(), b.ref()); }

ਸਾਂਚਾ<typename Owner, typename ਥਾਂ_2>
ਬਣਤਰ OwnedId : Wrapped_Id_ਅਧਾਰ<OwnedId<Owner, ਥਾਂ_2>> {
	OwnedId() = ਮੂਲ;
	OwnedId(ਅਚਲ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ਅਚਲ { ਵਾਪਸ id_; }
	ਅਚਲ TwinePool *pool() ਅਚਲ;
	OwnedId &ਸੰਚਾਲਕ=(IdString id) { id_ = id; ਵਾਪਸ *this; }
	OwnedId &ਸੰਚਾਲਕ=(ਅਚਲ OwnedId &other) { ਵਾਪਸ *this = other.ref(); }
	OwnedId &ਸੰਚਾਲਕ=(OwnedId &&other) { ਵਾਪਸ *this = other.ref(); }
	ਮਿੱਤਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(OwnedId &a, OwnedId &b) { std::ਅਦਲਾ_ਬਦਲੀ(a.id_, b.id_); }
ਨਿਜੀ:
	ਅਚਲ Owner *owner() ਅਚਲ;
	IdString id_;
};

ਬਣਤਰ PooledName : Wrapped_Id_ਅਧਾਰ<PooledName> {
	PooledName() = ਮੂਲ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ਅਚਲ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ਅਚਲ Design *design, IdString id);
	PooledName(ਅਚਲ Module *module, IdString id);
	ਸਾਂਚਾ<typename D> PooledName(ਅਚਲ Wrapped_Id_ਅਧਾਰ<D> &masq)
		: pool_(static_cast<ਅਚਲ D &>(masq).pool()),
		  id_(static_cast<ਅਚਲ D &>(masq).ref()) {}
	IdString ref() ਅਚਲ { ਵਾਪਸ id_; }
	ਅਚਲ TwinePool *pool() ਅਚਲ { ਵਾਪਸ pool_; }
ਨਿਜੀ:
	ਅਚਲ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ਸਾਂਚਾ<typename T>
	ਬਣਤਰ masq_hash_ops {
		ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ T &a, ਅਚਲ T &b) { ਵਾਪਸ a == b; }
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash(ਅਚਲ T &a) {
			ਵਾਪਸ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ T &a, Hasher h) {
			ਵਾਪਸ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ਸਾਂਚਾ<typename Owner, typename ਥਾਂ_2>
	ਬਣਤਰ hash_ops<RTLIL::OwnedId<Owner, ਥਾਂ_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, ਥਾਂ_2>> {};
	ਸਾਂਚਾ<> ਬਣਤਰ hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#ਜੇ_ਅੰਤ
