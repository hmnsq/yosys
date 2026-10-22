#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RTLIL_TWINE_COMPAT_H
#ವ್ಯಾಖ್ಯಾನಿಸು RTLIL_TWINE_COMPAT_H

namespace RTLIL {

ಮಾದರಿ<typename Derived> ರಚನೆ Wrapped_Id_ತಳ;
ರಚನೆ Name_ಸ್ಥಾನ;
ರಚನೆ Cell_Type_ಸ್ಥಾನ;
ಮಾದರಿ<typename Owner, typename ಸ್ಥಾನ_2 = Name_ಸ್ಥಾನ> ರಚನೆ OwnedId;
ರಚನೆ PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using ಸ್ಮರಣೆ_Name_Id = OwnedId<ಸ್ಮರಣೆ>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_ಸ್ಥಾನ>;

namespace wrapped_idstring_detail {

ಸಾಲೊಳಗೆ std::string render_escaped(ಸ್ಥಿರಾಂಕ TwinePool *pool, IdString id) {
	ಆದರೆ (id == IdString::Null)
		ಹಿಂತಿರುಗಿ std::string();
	ಹಿಂತಿರುಗಿ pool ? pool->str(id) : ID::str(id);
}

ಸಾಲೊಳಗೆ std::string render_unescaped(ಸ್ಥಿರಾಂಕ TwinePool *pool, IdString id) {
	ಆದರೆ (id == IdString::Null)
		ಹಿಂತಿರುಗಿ std::string();
	ಹಿಂತಿರುಗಿ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

ಮಾದರಿ<typename Derived>
ರಚನೆ Wrapped_Id_ತಳ {
	ಕ್ರಿಯಾಕಾರಕ IdString() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().ref(); }
	ಕ್ರಿಯಾಕಾರಕ std::string() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().escaped(); }
	std::string escaped() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_ಬಹಿರಂಗ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().ref().is_ಬಹಿರಂಗ(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().ref() == IdString::Null; }
	std::string str() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().escaped(); }
	bool begins_with(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಸ್ಥಿರಾಂಕ {
		ಸ್ಥಿರಾಂಕ TwinePool *pool = self().pool();
		ಹಿಂತಿರುಗಿ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ತುದಿಗಳು_with(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ str().ತುದಿಗಳು_with(s); }
	ಮಾದರಿ<typename... Ts> bool ಒಳ(Ts &&...ವಾದಗಳು_2) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ self().ref().ಒಳ(std::forward<Ts>(ವಾದಗಳು_2)...);
	}
	bool ಒಳ(ಸ್ಥಿರಾಂಕ pool<IdString> &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().ref().ಒಳ(rhs); }
	std::string substr(ಗಾತ್ರ_t pos = 0, ಗಾತ್ರ_t len = std::string::npos) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ self().escaped().substr(pos, len);
	}
	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ {
		ಸ್ಥಿರಾಂಕ TwinePool *pool = self().pool();
		ಹಿಂತಿರುಗಿ pool ? pool->str_ಗಾತ್ರ(self().ref()) : self().escaped().ಗಾತ್ರ_2();
	}
	bool ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().escaped().ಕಂಡುಹಿಡಿ(p) != std::string::npos; }
	ಅಕ್ಷರ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ self().escaped()[n]; }
	bool lt_by_name(ಸ್ಥಿರಾಂಕ Derived &rhs) ಸ್ಥಿರಾಂಕ {
		ಸ್ಥಿರಾಂಕ TwinePool *pool = self().pool();
		ಆದರೆ (pool == nullptr)
			ಹಿಂತಿರುಗಿ self().escaped() < rhs.escaped();
		ಹಿಂತಿರುಗಿ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Derived &lhs, ಸ್ಥಿರಾಂಕ Derived &rhs) { ಹಿಂತಿರುಗಿ lhs.ref() == rhs.ref(); }
	ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Derived &lhs, IdString rhs) { ಹಿಂತಿರುಗಿ lhs.ref() == rhs; }
	ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Derived &lhs, NullIdString) { ಹಿಂತಿರುಗಿ lhs.ref() == IdString::Null; }
	ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Derived &lhs, ಸ್ಥಿರಾಂಕ std::string &rhs) {
		ಸ್ಥಿರಾಂಕ TwinePool *pool = lhs.pool();
		ಹಿಂತಿರುಗಿ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Derived &lhs, ಸ್ಥಿರಾಂಕ Derived &rhs) { ಹಿಂತಿರುಗಿ lhs.ref() < rhs.ref(); }
ಖಾಸಗಿ:
	ಸ್ಥಿರಾಂಕ Derived &self() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *static_cast<ಸ್ಥಿರಾಂಕ Derived *>(this); }
};

ಮಾದರಿ<typename T>
concept IsWrappedId = std::is_ತಳ_ದ_v<Wrapped_Id_ತಳ<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
ಮಾದರಿ<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ A &lhs, ಸ್ಥಿರಾಂಕ B &rhs) { ಹಿಂತಿರುಗಿ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
ಮಾದರಿ<IsWrappedId A, typename B>
ಸ್ವಯಂಚಾಲಿತ make_pair(A &&a, B &&b) { ಹಿಂತಿರುಗಿ std::make_pair(a.ref(), std::forward<B>(b)); }
ಮಾದರಿ<typename A, IsWrappedId B>
ಸ್ವಯಂಚಾಲಿತ make_pair(A &&a, B &&b) { ಹಿಂತಿರುಗಿ std::make_pair(std::forward<A>(a), b.ref()); }
ಮಾದರಿ<IsWrappedId A, IsWrappedId B>
ಸ್ವಯಂಚಾಲಿತ make_pair(A &&a, B &&b) { ಹಿಂತಿರುಗಿ std::make_pair(a.ref(), b.ref()); }

ಮಾದರಿ<typename Owner, typename ಸ್ಥಾನ_2>
ರಚನೆ OwnedId : Wrapped_Id_ತಳ<OwnedId<Owner, ಸ್ಥಾನ_2>> {
	OwnedId() = ಪೂರ್ವನಿಯೋಜಿತ;
	OwnedId(ಸ್ಥಿರಾಂಕ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ id_; }
	ಸ್ಥಿರಾಂಕ TwinePool *pool() ಸ್ಥಿರಾಂಕ;
	OwnedId &ಕ್ರಿಯಾಕಾರಕ=(IdString id) { id_ = id; ಹಿಂತಿರುಗಿ *this; }
	OwnedId &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ OwnedId &other) { ಹಿಂತಿರುಗಿ *this = other.ref(); }
	OwnedId &ಕ್ರಿಯಾಕಾರಕ=(OwnedId &&other) { ಹಿಂತಿರುಗಿ *this = other.ref(); }
	ಮಿತ್ರ ಶೂನ್ಯ ಅದಲುಬದಲು(OwnedId &a, OwnedId &b) { std::ಅದಲುಬದಲು(a.id_, b.id_); }
ಖಾಸಗಿ:
	ಸ್ಥಿರಾಂಕ Owner *owner() ಸ್ಥಿರಾಂಕ;
	IdString id_;
};

ರಚನೆ PooledName : Wrapped_Id_ತಳ<PooledName> {
	PooledName() = ಪೂರ್ವನಿಯೋಜಿತ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ಸ್ಥಿರಾಂಕ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ಸ್ಥಿರಾಂಕ Design *design, IdString id);
	PooledName(ಸ್ಥಿರಾಂಕ Module *module, IdString id);
	ಮಾದರಿ<typename D> PooledName(ಸ್ಥಿರಾಂಕ Wrapped_Id_ತಳ<D> &masq)
		: pool_(static_cast<ಸ್ಥಿರಾಂಕ D &>(masq).pool()),
		  id_(static_cast<ಸ್ಥಿರಾಂಕ D &>(masq).ref()) {}
	IdString ref() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ id_; }
	ಸ್ಥಿರಾಂಕ TwinePool *pool() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ pool_; }
ಖಾಸಗಿ:
	ಸ್ಥಿರಾಂಕ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	ಮಾದರಿ<typename T>
	ರಚನೆ masq_hash_ops {
		ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ T &a, ಸ್ಥಿರಾಂಕ T &b) { ಹಿಂತಿರುಗಿ a == b; }
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash(ಸ್ಥಿರಾಂಕ T &a) {
			ಹಿಂತಿರುಗಿ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ T &a, Hasher h) {
			ಹಿಂತಿರುಗಿ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	ಮಾದರಿ<typename Owner, typename ಸ್ಥಾನ_2>
	ರಚನೆ hash_ops<RTLIL::OwnedId<Owner, ಸ್ಥಾನ_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, ಸ್ಥಾನ_2>> {};
	ಮಾದರಿ<> ರಚನೆ hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#ಆದರೆ_ಅಂತ್ಯ
