#که_نه_تعریف_شوی RTLIL_TWINE_COMPAT_H
#تعریف_کړه RTLIL_TWINE_COMPAT_H

namespace RTLIL {

کينډۍ<typename Derived> بنية Wrapped_Id_بنسټ;
بنية Name_خونه;
بنية Cell_Type_خونه;
کينډۍ<typename Owner, typename خونه_2 = Name_خونه> بنية OwnedId;
بنية PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using حافظه_Name_Id = OwnedId<حافظه>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_خونه>;

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

کينډۍ<typename Derived>
بنية Wrapped_Id_بنسټ {
	چلوونکی IdString() ثابت { أرجع self().ref(); }
	چلوونکی std::string() ثابت { أرجع self().escaped(); }
	std::string escaped() ثابت { أرجع wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ثابت { أرجع wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_عامه() ثابت { أرجع self().ref().is_عامه(); }
	bool تشه() ثابت { أرجع self().ref() == IdString::Null; }
	std::string str() ثابت { أرجع self().escaped(); }
	bool begins_with(ثابت حرف *s) ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool پایونه_with(ثابت حرف *s) ثابت { أرجع str().پایونه_with(s); }
	کينډۍ<typename... Ts> bool دننه(Ts &&...دلیلونه_2) ثابت {
		أرجع self().ref().دننه(std::forward<Ts>(دلیلونه_2)...);
	}
	bool دننه(ثابت pool<IdString> &rhs) ثابت { أرجع self().ref().دننه(rhs); }
	std::string substr(اندازه_t pos = 0, اندازه_t len = std::string::npos) ثابت {
		أرجع self().escaped().substr(pos, len);
	}
	اندازه_t اندازه() ثابت {
		ثابت TwinePool *pool = self().pool();
		أرجع pool ? pool->str_اندازه(self().ref()) : self().escaped().اندازه();
	}
	bool لري(ثابت حرف *p) ثابت { أرجع self().escaped().پیدا_کړه(p) != std::string::npos; }
	حرف چلوونکی[](صحيح n) ثابت { أرجع self().escaped()[n]; }
	bool lt_by_name(ثابت Derived &rhs) ثابت {
		ثابت TwinePool *pool = self().pool();
		إذا (pool == nullptr)
			أرجع self().escaped() < rhs.escaped();
		أرجع pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	ملګری bool چلوونکی==(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() == rhs.ref(); }
	ملګری bool چلوونکی==(ثابت Derived &lhs, IdString rhs) { أرجع lhs.ref() == rhs; }
	ملګری bool چلوونکی==(ثابت Derived &lhs, NullIdString) { أرجع lhs.ref() == IdString::Null; }
	ملګری bool چلوونکی==(ثابت Derived &lhs, ثابت std::string &rhs) {
		ثابت TwinePool *pool = lhs.pool();
		أرجع pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	ملګری bool چلوونکی<(ثابت Derived &lhs, ثابت Derived &rhs) { أرجع lhs.ref() < rhs.ref(); }
خاص:
	ثابت Derived &self() ثابت { أرجع *static_cast<ثابت Derived *>(this); }
};

کينډۍ<typename T>
concept IsWrappedId = std::is_بنسټ_د_v<Wrapped_Id_بنسټ<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
کينډۍ<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
مضمن bool چلوونکی==(ثابت A &lhs, ثابت B &rhs) { أرجع lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
کينډۍ<IsWrappedId A, typename B>
خپلکاری make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), std::forward<B>(b)); }
کينډۍ<typename A, IsWrappedId B>
خپلکاری make_pair(A &&a, B &&b) { أرجع std::make_pair(std::forward<A>(a), b.ref()); }
کينډۍ<IsWrappedId A, IsWrappedId B>
خپلکاری make_pair(A &&a, B &&b) { أرجع std::make_pair(a.ref(), b.ref()); }

کينډۍ<typename Owner, typename خونه_2>
بنية OwnedId : Wrapped_Id_بنسټ<OwnedId<Owner, خونه_2>> {
	OwnedId() = افتراضي;
	OwnedId(ثابت OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ثابت { أرجع id_; }
	ثابت TwinePool *pool() ثابت;
	OwnedId &چلوونکی=(IdString id) { id_ = id; أرجع *this; }
	OwnedId &چلوونکی=(ثابت OwnedId &other) { أرجع *this = other.ref(); }
	OwnedId &چلوونکی=(OwnedId &&other) { أرجع *this = other.ref(); }
	ملګری فراغ بدلون(OwnedId &a, OwnedId &b) { std::بدلون(a.id_, b.id_); }
خاص:
	ثابت Owner *owner() ثابت;
	IdString id_;
};

بنية PooledName : Wrapped_Id_بنسټ<PooledName> {
	PooledName() = افتراضي;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ثابت TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ثابت Design *design, IdString id);
	PooledName(ثابت Module *module, IdString id);
	کينډۍ<typename D> PooledName(ثابت Wrapped_Id_بنسټ<D> &masq)
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
	کينډۍ<typename T>
	بنية masq_hash_ops {
		ساكن مضمن bool cmp(ثابت T &a, ثابت T &b) { أرجع a == b; }
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت T &a) {
			أرجع hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت T &a, Hasher h) {
			أرجع hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	کينډۍ<typename Owner, typename خونه_2>
	بنية hash_ops<RTLIL::OwnedId<Owner, خونه_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, خونه_2>> {};
	کينډۍ<> بنية hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#د_که_پای
