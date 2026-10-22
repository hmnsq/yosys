#اگر_تعریف_نشده RTLIL_TWINE_COMPAT_H
#تعریف_کن RTLIL_TWINE_COMPAT_H

namespace RTLIL {

قالب<typename Derived> بنية Wrapped_Id_پایه;
بنية Name_جایگاه;
بنية Cell_Type_جایگاه;
قالب<typename Owner, typename جایگاه_2 = Name_جایگاه> بنية OwnedId;
بنية PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using حافظه_Name_Id = OwnedId<حافظه>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_جایگاه>;

namespace wrapped_idstring_detail {

مضمن std::string render_escaped(ثابت TwinePool *pool, IdString id) {
	إذا (id == IdString::Null)
		أرجع std::string();
	أرجع pool ? pool->str(id) : ID::str(id);
}

مضمن std::string render_unescaped(ثابت TwinePool *pool, IdString id) {
	إذا (id == IdString::Null)
		أرجع std::string();
	أرجع pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

قالب<typename Derived>
بنية Wrapped_Id_پایه {
	عملگر IdString() ثابت { أرجع self().ref(); }
	عملگر std::string() ثابت { أرجع self().escaped(); }
	std::string escaped() ثابت { أرجع wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { أرجع wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_عمومی() ثابت { أرجع self().ref().is_عمومی(); }
	bool خالی() ثابت { أرجع self().ref() == IdString::Null; }
	std::string str() ثابت { أرجع self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool دوسر_with(ثابت حرف *s) ثابت { أرجع str().دوسر_with(s); }
	قالب<typename... Ts> bool درون(Ts &&...آرگومانها_2) ثابت {
		أرجع self().ref().درون(std::forward<Ts>(آرگومانها_2)...);
	}
	bool درون(ثابت pool<IdString> &rhs) ثابت { أرجع self().ref().درون(rhs); }
	std::string substr(اندازه_t pos = 0, اندازه_t len = std::string::npos) ثابت {
		أرجع self().escaped().substr(pos, len);
	}
	اندازه_t اندازه() ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->str_اندازه(self().ref()) : self().escaped().اندازه();
	}
	bool شامل_است(ثابت حرف *p) ثابت { أرجع self().escaped().پیدا_کن(p) != std::string::npos; }
	حرف عملگر[](صحيح n) ثابت { أرجع self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		إذا (pool == nullptr)
			أرجع self().escaped() < rhs.escaped();
		أرجع pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	دوست bool عملگر==(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() == rhs.ref(); }
	دوست bool عملگر==(ثابت Derived &lhs, IdString rhs) { أرجع lhs.ref() == rhs; }
	دوست bool عملگر==(ثابت Derived &lhs, NullIdString) { أرجع lhs.ref() == IdString::Null; }
	دوست bool عملگر==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		أرجع pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	دوست bool عملگر<(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() < rhs.ref(); }
خاص:
	ثابت Derived &self() ثابت { أرجع *static_cast<ثابت Derived *>(this); }
};

قالب<typename T>
concept IsWrappedId = std::is_پایه_از_v<Wrapped_Id_پایه<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
قالب<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
مضمن bool عملگر==(ثابت A &lhs, ثابت B &rhs) { أرجع lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
قالب<IsWrappedId A, typename B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), std::forward<B>(b)); }
قالب<typename A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(std::forward<A>(a), b.ref()); }
قالب<IsWrappedId A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), b.ref()); }

قالب<typename Owner, typename جایگاه_2>
بنية OwnedId : Wrapped_Id_پایه<OwnedId<Owner, جایگاه_2>> {
	OwnedId() = افتراضي;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { أرجع id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &عملگر=(IdString id) { id_ = id; أرجع *this; }
	OwnedId &عملگر=(ثابت OwnedId &other) { أرجع *this = other.ref(); }
	OwnedId &عملگر=(OwnedId &&other) { أرجع *this = other.ref(); }
	دوست فراغ جابجا(OwnedId &a, OwnedId &b) { std::جابجا(a.id_, b.id_); }
خاص:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

بنية PooledName : Wrapped_Id_پایه<PooledName> {
	PooledName() = افتراضي;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	قالب<typename D> PooledName(ثابت Wrapped_Id_پایه<D> &masq)
		: pool_(static_cast<ثابت D &>(masq).pool()),
		  id_(static_cast<ثابت D &>(masq).ref()) {}
	IdString ref() ثابت { أرجع id_; }
	ثابت TwinePool *pool() ثابت { أرجع pool_; }
خاص:
	ثابت TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	قالب<typename T>
	بنية masq_hash_ops {
		ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت T &a) {
			أرجع hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
			أرجع hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	قالب<typename Owner, typename جایگاه_2>
	بنية hash_ops<RTLIL::OwnedId<Owner, جایگاه_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, جایگاه_2>> {};
	قالب<> بنية hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#پایان_اگر
