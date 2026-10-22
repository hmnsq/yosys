#جے_بنا_تعریف RTLIL_TWINE_COMPAT_H
#تعریف_کرو RTLIL_TWINE_COMPAT_H

namespace RTLIL {

سانچا<typename Derived> بنية Wrapped_Id_ادھار;
بنية Name_تھاں;
بنية Cell_Type_تھاں;
سانچا<typename Owner, typename تھاں_2 = Name_تھاں> بنية OwnedId;
بنية PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using حافظہ_Name_Id = OwnedId<حافظہ>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_تھاں>;

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

سانچا<typename Derived>
بنية Wrapped_Id_ادھار {
	عامل IdString() ثابت { أرجع self().ref(); }
	عامل std::string() ثابت { أرجع self().escaped(); }
	std::string escaped() ثابت { أرجع wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { أرجع wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_عوامی() ثابت { أرجع self().ref().is_عوامی(); }
	bool خالی_تھاواں() ثابت { أرجع self().ref() == IdString::Null; }
	std::string str() ثابت { أرجع self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool سرے_with(ثابت حرف *s) ثابت { أرجع str().سرے_with(s); }
	سانچا<typename... Ts> bool وچ(Ts &&...دلیلاں_2) ثابت {
		أرجع self().ref().وچ(std::forward<Ts>(دلیلاں_2)...);
	}
	bool وچ(ثابت pool<IdString> &rhs) ثابت { أرجع self().ref().وچ(rhs); }
	std::string substr(ناپ_t pos = 0, ناپ_t len = std::string::npos) ثابت {
		أرجع self().escaped().substr(pos, len);
	}
	ناپ_t ناپ() ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->str_ناپ(self().ref()) : self().escaped().ناپ();
	}
	bool شامل_اے(ثابت حرف *p) ثابت { أرجع self().escaped().لبھو(p) != std::string::npos; }
	حرف عامل[](صحيح n) ثابت { أرجع self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		إذا (pool == nullptr)
			أرجع self().escaped() < rhs.escaped();
		أرجع pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	یار bool عامل==(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() == rhs.ref(); }
	یار bool عامل==(ثابت Derived &lhs, IdString rhs) { أرجع lhs.ref() == rhs; }
	یار bool عامل==(ثابت Derived &lhs, NullIdString) { أرجع lhs.ref() == IdString::Null; }
	یار bool عامل==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		أرجع pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	یار bool عامل<(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() < rhs.ref(); }
خاص:
	ثابت Derived &self() ثابت { أرجع *static_cast<ثابت Derived *>(this); }
};

سانچا<typename T>
concept IsWrappedId = std::is_ادھار_دا_v<Wrapped_Id_ادھار<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
سانچا<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
مضمن bool عامل==(ثابت A &lhs, ثابت B &rhs) { أرجع lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
سانچا<IsWrappedId A, typename B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), std::forward<B>(b)); }
سانچا<typename A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(std::forward<A>(a), b.ref()); }
سانچا<IsWrappedId A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), b.ref()); }

سانچا<typename Owner, typename تھاں_2>
بنية OwnedId : Wrapped_Id_ادھار<OwnedId<Owner, تھاں_2>> {
	OwnedId() = افتراضي;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { أرجع id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &عامل=(IdString id) { id_ = id; أرجع *this; }
	OwnedId &عامل=(ثابت OwnedId &other) { أرجع *this = other.ref(); }
	OwnedId &عامل=(OwnedId &&other) { أرجع *this = other.ref(); }
	یار فراغ وٹاندرا(OwnedId &a, OwnedId &b) { std::وٹاندرا(a.id_, b.id_); }
خاص:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

بنية PooledName : Wrapped_Id_ادھار<PooledName> {
	PooledName() = افتراضي;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	سانچا<typename D> PooledName(ثابت Wrapped_Id_ادھار<D> &masq)
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
	سانچا<typename T>
	بنية masq_hash_ops {
		ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت T &a) {
			أرجع hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
			أرجع hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	سانچا<typename Owner, typename تھاں_2>
	بنية hash_ops<RTLIL::OwnedId<Owner, تھاں_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, تھاں_2>> {};
	سانچا<> بنية hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#جے_انت
