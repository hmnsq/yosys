#اگر_غیر_تعریف_شدہ RTLIL_TWINE_COMPAT_H
#تعریف_کرو RTLIL_TWINE_COMPAT_H

namespace RTLIL {

سانچہ<typename Derived> ساخت Wrapped_Id_بنیاد;
ساخت Name_خانہ;
ساخت Cell_Type_خانہ;
سانچہ<typename Owner, typename خانہ_2 = Name_خانہ> ساخت OwnedId;
ساخت PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using حافظہ_Name_Id = OwnedId<حافظہ>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_خانہ>;

namespace wrapped_idstring_detail {

اندرونی std::string render_escaped(ثابت TwinePool *pool, IdString id) {
	اگر (id == IdString::Null)
		واپس std::string();
	واپس pool ? pool->str(id) : ID::str(id);
}

اندرونی std::string render_unescaped(ثابت TwinePool *pool, IdString id) {
	اگر (id == IdString::Null)
		واپس std::string();
	واپس pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

سانچہ<typename Derived>
ساخت Wrapped_Id_بنیاد {
	عامل IdString() ثابت { واپس self().ref(); }
	عامل std::string() ثابت { واپس self().escaped(); }
	std::string escaped() ثابت { واپس wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { واپس wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_عوامی() ثابت { واپس self().ref().is_عوامی(); }
	bool خالی_خانے() ثابت { واپس self().ref() == IdString::Null; }
	std::string str() ثابت { واپس self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		واپس pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool سرے_with(ثابت حرف *s) ثابت { واپس str().سرے_with(s); }
	سانچہ<typename... Ts> bool اندر(Ts &&...دلائل_2) ثابت {
		واپس self().ref().اندر(std::forward<Ts>(دلائل_2)...);
	}
	bool اندر(ثابت pool<IdString> &rhs) ثابت { واپس self().ref().اندر(rhs); }
	std::string substr(سائز_t pos = 0, سائز_t len = std::string::npos) ثابت {
		واپس self().escaped().substr(pos, len);
	}
	سائز_t سائز() ثابت {
		ثابت TwinePool *pool = self().pool();
		واپس pool ? pool->str_سائز(self().ref()) : self().escaped().سائز();
	}
	bool شامل_ہے(ثابت حرف *p) ثابت { واپس self().escaped().ڈھونڈو(p) != std::string::npos; }
	حرف عامل[](صحیح n) ثابت { واپس self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		اگر (pool == nullptr)
			واپس self().escaped() < rhs.escaped();
		واپس pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	دوست bool عامل==(ثابت Derived &lhs, ثابت Derived &rhs) { واپس lhs.ref() == rhs.ref(); }
	دوست bool عامل==(ثابت Derived &lhs, IdString rhs) { واپس lhs.ref() == rhs; }
	دوست bool عامل==(ثابت Derived &lhs, NullIdString) { واپس lhs.ref() == IdString::Null; }
	دوست bool عامل==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		واپس pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	دوست bool عامل<(ثابت Derived &lhs, ثابت Derived &rhs) { واپس lhs.ref() < rhs.ref(); }
نجی:
	ثابت Derived &self() ثابت { واپس *static_cast<ثابت Derived *>(this); }
};

سانچہ<typename T>
concept IsWrappedId = std::is_بنیاد_کا_v<Wrapped_Id_بنیاد<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
سانچہ<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
اندرونی bool عامل==(ثابت A &lhs, ثابت B &rhs) { واپس lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
سانچہ<IsWrappedId A, typename B>
خودکار make_pair(A &&a, B &&b) { واپس std::make_pair(a.ref(), std::forward<B>(b)); }
سانچہ<typename A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { واپس std::make_pair(std::forward<A>(a), b.ref()); }
سانچہ<IsWrappedId A, IsWrappedId B>
خودکار make_pair(A &&a, B &&b) { واپس std::make_pair(a.ref(), b.ref()); }

سانچہ<typename Owner, typename خانہ_2>
ساخت OwnedId : Wrapped_Id_بنیاد<OwnedId<Owner, خانہ_2>> {
	OwnedId() = طےشدہ;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { واپس id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &عامل=(IdString id) { id_ = id; واپس *this; }
	OwnedId &عامل=(ثابت OwnedId &other) { واپس *this = other.ref(); }
	OwnedId &عامل=(OwnedId &&other) { واپس *this = other.ref(); }
	دوست خالی تبادلہ(OwnedId &a, OwnedId &b) { std::تبادلہ(a.id_, b.id_); }
نجی:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

ساخت PooledName : Wrapped_Id_بنیاد<PooledName> {
	PooledName() = طےشدہ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	سانچہ<typename D> PooledName(ثابت Wrapped_Id_بنیاد<D> &masq)
		: pool_(static_cast<ثابت D &>(masq).pool()),
		  id_(static_cast<ثابت D &>(masq).ref()) {}
	IdString ref() ثابت { واپس id_; }
	ثابت TwinePool *pool() ثابت { واپس pool_; }
نجی:
	ثابت TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	سانچہ<typename T>
	ساخت masq_hash_ops {
		ساکن اندرونی bool cmp(ثابت T &a, ثابت T &b) { واپس a == b; }
		[[nodiscard]] ساکن اندرونی Hasher hash(ثابت T &a) {
			واپس hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت T &a, Hasher h) {
			واپس hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	سانچہ<typename Owner, typename خانہ_2>
	ساخت hash_ops<RTLIL::OwnedId<Owner, خانہ_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, خانہ_2>> {};
	سانچہ<> ساخت hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#اگر_ختم
