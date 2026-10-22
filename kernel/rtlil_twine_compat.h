#నిర్వచితం_కాకపోతే RTLIL_TWINE_COMPAT_H
#నిర్వచించు RTLIL_TWINE_COMPAT_H

namespace RTLIL {

మూస<typename Derived> నిర్మాణం Wrapped_Id_పునాది;
నిర్మాణం Name_స్థానం;
నిర్మాణం Cell_Type_స్థానం;
మూస<typename Owner, typename స్థానం_2 = Name_స్థానం> నిర్మాణం OwnedId;
నిర్మాణం PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using జ్ఞాపకం_Name_Id = OwnedId<జ్ఞాపకం>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, Cell_Type_స్థానం>;

namespace wrapped_idstring_detail {

అంతఃవరుస std::string render_escaped(స్థిరరాశి TwinePool *pool, IdString id) {
	ఒకవేళ (id == IdString::Null)
		తిరిగి std::string();
	తిరిగి pool ? pool->str(id) : ID::str(id);
}

అంతఃవరుస std::string render_unescaped(స్థిరరాశి TwinePool *pool, IdString id) {
	ఒకవేళ (id == IdString::Null)
		తిరిగి std::string();
	తిరిగి pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

మూస<typename Derived>
నిర్మాణం Wrapped_Id_పునాది {
	క్రియాకారకం IdString() స్థిరరాశి { తిరిగి self().ref(); }
	క్రియాకారకం std::string() స్థిరరాశి { తిరిగి self().escaped(); }
	std::string escaped() స్థిరరాశి { తిరిగి wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() స్థిరరాశి { తిరిగి wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool is_ప్రజా() స్థిరరాశి { తిరిగి self().ref().is_ప్రజా(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి self().ref() == IdString::Null; }
	std::string str() స్థిరరాశి { తిరిగి self().escaped(); }
	bool begins_with(స్థిరరాశి అక్షరం *s) స్థిరరాశి {
		స్థిరరాశి TwinePool *pool = self().pool();
		తిరిగి pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool చివర్లు_with(స్థిరరాశి అక్షరం *s) స్థిరరాశి { తిరిగి str().చివర్లు_with(s); }
	మూస<typename... Ts> bool లోపల(Ts &&...వాదనలు_2) స్థిరరాశి {
		తిరిగి self().ref().లోపల(std::forward<Ts>(వాదనలు_2)...);
	}
	bool లోపల(స్థిరరాశి pool<IdString> &rhs) స్థిరరాశి { తిరిగి self().ref().లోపల(rhs); }
	std::string substr(కొలత_t pos = 0, కొలత_t len = std::string::npos) స్థిరరాశి {
		తిరిగి self().escaped().substr(pos, len);
	}
	కొలత_t కొలత() స్థిరరాశి {
		స్థిరరాశి TwinePool *pool = self().pool();
		తిరిగి pool ? pool->str_కొలత(self().ref()) : self().escaped().కొలత();
	}
	bool కలిగియుంది(స్థిరరాశి అక్షరం *p) స్థిరరాశి { తిరిగి self().escaped().కనుగొను(p) != std::string::npos; }
	అక్షరం క్రియాకారకం[](పూర్ణాంకం n) స్థిరరాశి { తిరిగి self().escaped()[n]; }
	bool lt_by_name(స్థిరరాశి Derived &rhs) స్థిరరాశి {
		స్థిరరాశి TwinePool *pool = self().pool();
		ఒకవేళ (pool == nullptr)
			తిరిగి self().escaped() < rhs.escaped();
		తిరిగి pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	మిత్ర bool క్రియాకారకం==(స్థిరరాశి Derived &lhs, స్థిరరాశి Derived &rhs) { తిరిగి lhs.ref() == rhs.ref(); }
	మిత్ర bool క్రియాకారకం==(స్థిరరాశి Derived &lhs, IdString rhs) { తిరిగి lhs.ref() == rhs; }
	మిత్ర bool క్రియాకారకం==(స్థిరరాశి Derived &lhs, NullIdString) { తిరిగి lhs.ref() == IdString::Null; }
	మిత్ర bool క్రియాకారకం==(స్థిరరాశి Derived &lhs, స్థిరరాశి std::string &rhs) {
		స్థిరరాశి TwinePool *pool = lhs.pool();
		తిరిగి pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	మిత్ర bool క్రియాకారకం<(స్థిరరాశి Derived &lhs, స్థిరరాశి Derived &rhs) { తిరిగి lhs.ref() < rhs.ref(); }
అంతరంగ:
	స్థిరరాశి Derived &self() స్థిరరాశి { తిరిగి *static_cast<స్థిరరాశి Derived *>(this); }
};

మూస<typename T>
concept IsWrappedId = std::is_పునాది_యొక్క_v<Wrapped_Id_పునాది<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
మూస<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి A &lhs, స్థిరరాశి B &rhs) { తిరిగి lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
మూస<IsWrappedId A, typename B>
స్వయంచాలక make_pair(A &&a, B &&b) { తిరిగి std::make_pair(a.ref(), std::forward<B>(b)); }
మూస<typename A, IsWrappedId B>
స్వయంచాలక make_pair(A &&a, B &&b) { తిరిగి std::make_pair(std::forward<A>(a), b.ref()); }
మూస<IsWrappedId A, IsWrappedId B>
స్వయంచాలక make_pair(A &&a, B &&b) { తిరిగి std::make_pair(a.ref(), b.ref()); }

మూస<typename Owner, typename స్థానం_2>
నిర్మాణం OwnedId : Wrapped_Id_పునాది<OwnedId<Owner, స్థానం_2>> {
	OwnedId() = అప్రమేయ;
	OwnedId(స్థిరరాశి OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() స్థిరరాశి { తిరిగి id_; }
	స్థిరరాశి TwinePool *pool() స్థిరరాశి;
	OwnedId &క్రియాకారకం=(IdString id) { id_ = id; తిరిగి *this; }
	OwnedId &క్రియాకారకం=(స్థిరరాశి OwnedId &other) { తిరిగి *this = other.ref(); }
	OwnedId &క్రియాకారకం=(OwnedId &&other) { తిరిగి *this = other.ref(); }
	మిత్ర శూన్య మార్పిడి(OwnedId &a, OwnedId &b) { std::మార్పిడి(a.id_, b.id_); }
అంతరంగ:
	స్థిరరాశి Owner *owner() స్థిరరాశి;
	IdString id_;
};

నిర్మాణం PooledName : Wrapped_Id_పునాది<PooledName> {
	PooledName() = అప్రమేయ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(స్థిరరాశి TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(స్థిరరాశి Design *design, IdString id);
	PooledName(స్థిరరాశి Module *module, IdString id);
	మూస<typename D> PooledName(స్థిరరాశి Wrapped_Id_పునాది<D> &masq)
		: pool_(static_cast<స్థిరరాశి D &>(masq).pool()),
		  id_(static_cast<స్థిరరాశి D &>(masq).ref()) {}
	IdString ref() స్థిరరాశి { తిరిగి id_; }
	స్థిరరాశి TwinePool *pool() స్థిరరాశి { తిరిగి pool_; }
అంతరంగ:
	స్థిరరాశి TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	మూస<typename T>
	నిర్మాణం masq_hash_ops {
		స్థిర అంతఃవరుస bool cmp(స్థిరరాశి T &a, స్థిరరాశి T &b) { తిరిగి a == b; }
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash(స్థిరరాశి T &a) {
			తిరిగి hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి T &a, Hasher h) {
			తిరిగి hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	మూస<typename Owner, typename స్థానం_2>
	నిర్మాణం hash_ops<RTLIL::OwnedId<Owner, స్థానం_2>> : masq_hash_ops<RTLIL::OwnedId<Owner, స్థానం_2>> {};
	మూస<> నిర్మాణం hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#అయితే_ముగింపు
