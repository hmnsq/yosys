#جيڪڏهن_وصف_نه_ٿيل RTLIL_TWINE_COMPAT_H
#وصف_ڪريو RTLIL_TWINE_COMPAT_H

namespace RTLIL {

سانچو<typename Derived> بنية Wrapped_Id_بنياد;
بنية Name_جڳهه;
بنية Cell_Type_جڳهه;
سانچو<typename Owner, typename جڳهه_2 = Name_جڳهه> بنية OwnedId;
بنية PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using يادگيري_Name_Id = OwnedId<يادگيري>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_جڳهه>;

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

سانچو<typename Derived>
بنية Wrapped_Id_بنياد {
	عامل IdString() ثابت { أرجع self().ref(); }
	عامل std::string() ثابت { أرجع self().escaped(); }
	std::string escaped() ثابت { أرجع wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { أرجع wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_عوامي() ثابت { أرجع self().ref().is_عوامي(); }
	bool خالي_جايون() ثابت { أرجع self().ref() == IdString::Null; }
	std::string str() ثابت { أرجع self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool پڇاڙيون_with(ثابت حرف *s) ثابت { أرجع str().پڇاڙيون_with(s); }
	سانچو<typename... Ts> bool اندريون(Ts &&...دليلون_2) ثابت {
		أرجع self().ref().اندريون(std::forward<Ts>(دليلون_2)...);
	}
	bool اندريون(ثابت pool<IdString> &rhs) ثابت { أرجع self().ref().اندريون(rhs); }
	std::string substr(سائيز_t pos = 0, سائيز_t len = std::string::npos) ثابت {
		أرجع self().escaped().substr(pos, len);
	}
	سائيز_t سائيز() ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->str_سائيز(self().ref()) : self().escaped().سائيز();
	}
	bool شامل_آهي(ثابت حرف *p) ثابت { أرجع self().escaped().ڳوليو(p) != std::string::npos; }
	حرف عامل[](صحيح n) ثابت { أرجع self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		إذا (pool == nullptr)
			أرجع self().escaped() < rhs.escaped();
		أرجع pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	دوست bool عامل==(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() == rhs.ref(); }
	دوست bool عامل==(ثابت Derived &lhs, IdString rhs) { أرجع lhs.ref() == rhs; }
	دوست bool عامل==(ثابت Derived &lhs, NullIdString) { أرجع lhs.ref() == IdString::Null; }
	دوست bool عامل==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		أرجع pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	دوست bool عامل<(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() < rhs.ref(); }
خاص:
	ثابت Derived &self() ثابت { أرجع *static_cast<ثابت Derived *>(this); }
};

سانچو<typename T>
concept IsWrappedId = std::is_بنياد_جو_v<Wrapped_Id_بنياد<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
سانچو<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
مضمن bool عامل==(ثابت A &lhs, ثابت B &rhs) { أرجع lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
سانچو<IsWrappedId A, typename B>
خودڪار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), std::forward<B>(b)); }
سانچو<typename A, IsWrappedId B>
خودڪار make_pair(A &&a, B &&b) { أرجع std::make_pair(std::forward<A>(a), b.ref()); }
سانچو<IsWrappedId A, IsWrappedId B>
خودڪار make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), b.ref()); }

سانچو<typename Owner, typename جڳهه_2>
بنية OwnedId : Wrapped_Id_بنياد<OwnedId<Owner, جڳهه_2>> {
	OwnedId() = افتراضي;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { أرجع id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &عامل=(IdString id) { id_ = id; أرجع *this; }
	OwnedId &عامل=(ثابت OwnedId &other) { أرجع *this = other.ref(); }
	OwnedId &عامل=(OwnedId &&other) { أرجع *this = other.ref(); }
	دوست فراغ مٽا_سٽا(OwnedId &a, OwnedId &b) { std::مٽا_سٽا(a.id_, b.id_); }
خاص:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

بنية PooledName : Wrapped_Id_بنياد<PooledName> {
	PooledName() = افتراضي;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	سانچو<typename D> PooledName(ثابت Wrapped_Id_بنياد<D> &masq)
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
	سانچو<typename T>
	بنية masq_hash_ops {
		ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت T &a) {
			أرجع hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
			أرجع hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	سانچو<typename Owner, typename جڳهه_2>
	بنية hash_ops<RTLIL::OwnedId<Owner, جڳهه_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, جڳهه_2>> {};
	سانچو<> بنية hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#جيڪڏهن_پورو
