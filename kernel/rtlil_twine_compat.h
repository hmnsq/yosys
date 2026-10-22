#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> ସଂରଚନା WrappedIdBase;
ସଂରଚନା NameSlot;
ସଂରଚନା CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> ସଂରଚନା OwnedId;
ସଂରଚନା PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

ଧାଡ଼ିଭିତରେ std::string render_escaped(ଅଚଳ TwinePool *pool, IdString id) {
	ଯଦି (id == IdString::Null)
		ଫେରନ୍ତୁ std::string();
	ଫେରନ୍ତୁ pool ? pool->str(id) : ID::str(id);
}

ଧାଡ଼ିଭିତରେ std::string render_unescaped(ଅଚଳ TwinePool *pool, IdString id) {
	ଯଦି (id == IdString::Null)
		ଫେରନ୍ତୁ std::string();
	ଫେରନ୍ତୁ pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
ସଂରଚନା WrappedIdBase {
	operator IdString() ଅଚଳ { ଫେରନ୍ତୁ self().ref(); }
	operator std::string() ଅଚଳ { ଫେରନ୍ତୁ self().escaped(); }
	std::string escaped() ଅଚଳ { ଫେରନ୍ତୁ wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() ଅଚଳ { ଫେରନ୍ତୁ wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() ଅଚଳ { ଫେରନ୍ତୁ self().ref().isPublic(); }
	bool empty() ଅଚଳ { ଫେରନ୍ତୁ self().ref() == IdString::Null; }
	std::string str() ଅଚଳ { ଫେରନ୍ତୁ self().escaped(); }
	bool begins_with(ଅଚଳ ଅକ୍ଷର *s) ଅଚଳ {
		ଅଚଳ TwinePool *pool = self().pool();
		ଫେରନ୍ତୁ pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(ଅଚଳ ଅକ୍ଷର *s) ଅଚଳ { ଫେରନ୍ତୁ str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) ଅଚଳ {
		ଫେରନ୍ତୁ self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(ଅଚଳ pool<IdString> &rhs) ଅଚଳ { ଫେରନ୍ତୁ self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) ଅଚଳ {
		ଫେରନ୍ତୁ self().escaped().substr(pos, len);
	}
	size_t size() ଅଚଳ {
		ଅଚଳ TwinePool *pool = self().pool();
		ଫେରନ୍ତୁ pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(ଅଚଳ ଅକ୍ଷର *p) ଅଚଳ { ଫେରନ୍ତୁ self().escaped().find(p) != std::string::npos; }
	ଅକ୍ଷର operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ self().escaped()[n]; }
	bool lt_by_name(ଅଚଳ Derived &rhs) ଅଚଳ {
		ଅଚଳ TwinePool *pool = self().pool();
		ଯଦି (pool == nullptr)
			ଫେରନ୍ତୁ self().escaped() < rhs.escaped();
		ଫେରନ୍ତୁ pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(ଅଚଳ Derived &lhs, ଅଚଳ Derived &rhs) { ଫେରନ୍ତୁ lhs.ref() == rhs.ref(); }
	friend bool operator==(ଅଚଳ Derived &lhs, IdString rhs) { ଫେରନ୍ତୁ lhs.ref() == rhs; }
	friend bool operator==(ଅଚଳ Derived &lhs, NullIdString) { ଫେରନ୍ତୁ lhs.ref() == IdString::Null; }
	friend bool operator==(ଅଚଳ Derived &lhs, ଅଚଳ std::string &rhs) {
		ଅଚଳ TwinePool *pool = lhs.pool();
		ଫେରନ୍ତୁ pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(ଅଚଳ Derived &lhs, ଅଚଳ Derived &rhs) { ଫେରନ୍ତୁ lhs.ref() < rhs.ref(); }
ବ୍ୟକ୍ତିଗତ:
	ଅଚଳ Derived &self() ଅଚଳ { ଫେରନ୍ତୁ *static_cast<ଅଚଳ Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
ଧାଡ଼ିଭିତରେ bool operator==(ଅଚଳ A &lhs, ଅଚଳ B &rhs) { ଫେରନ୍ତୁ lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { ଫେରନ୍ତୁ std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ଫେରନ୍ତୁ std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { ଫେରନ୍ତୁ std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
ସଂରଚନା OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	OwnedId(ଅଚଳ OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() ଅଚଳ { ଫେରନ୍ତୁ id_; }
	ଅଚଳ TwinePool *pool() ଅଚଳ;
	OwnedId &operator=(IdString id) { id_ = id; ଫେରନ୍ତୁ *this; }
	OwnedId &operator=(ଅଚଳ OwnedId &other) { ଫେରନ୍ତୁ *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { ଫେରନ୍ତୁ *this = other.ref(); }
	friend ଶୂନ୍ୟ swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
ବ୍ୟକ୍ତିଗତ:
	ଅଚଳ Owner *owner() ଅଚଳ;
	IdString id_;
};

ସଂରଚନା PooledName : WrappedIdBase<PooledName> {
	PooledName() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(ଅଚଳ TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(ଅଚଳ Design *design, IdString id);
	PooledName(ଅଚଳ Module *module, IdString id);
	template<typename D> PooledName(ଅଚଳ WrappedIdBase<D> &masq)
		: pool_(static_cast<ଅଚଳ D &>(masq).pool()),
		  id_(static_cast<ଅଚଳ D &>(masq).ref()) {}
	IdString ref() ଅଚଳ { ଫେରନ୍ତୁ id_; }
	ଅଚଳ TwinePool *pool() ଅଚଳ { ଫେରନ୍ତୁ pool_; }
ବ୍ୟକ୍ତିଗତ:
	ଅଚଳ TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	ସଂରଚନା masq_hash_ops {
		ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ T &a, ଅଚଳ T &b) { ଫେରନ୍ତୁ a == b; }
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash(ଅଚଳ T &a) {
			ଫେରନ୍ତୁ hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ T &a, Hasher h) {
			ଫେରନ୍ତୁ hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	ସଂରଚନା hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> ସଂରଚନା hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
