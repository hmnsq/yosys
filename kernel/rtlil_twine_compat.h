#ifndef RTLIL_TWINE_COMPAT_H
#define RTLIL_TWINE_COMPAT_H

namespace RTLIL {

template<typename Derived> கட்டமைப்பு WrappedIdBase;
கட்டமைப்பு NameSlot;
கட்டமைப்பு CellTypeSlot;
template<typename Owner, typename Slot = NameSlot> கட்டமைப்பு OwnedId;
கட்டமைப்பு PooledName;

using ModuleNameId = OwnedId<Module>;
using WireNameId = OwnedId<Wire>;
using CellNameId = OwnedId<Cell>;
using MemoryNameId = OwnedId<Memory>;
using ProcessNameId = OwnedId<Process>;
using CellTypeId = OwnedId<Cell, CellTypeSlot>;

namespace wrapped_idstring_detail {

உள்வரி std::string render_escaped(மாறிலி TwinePool *pool, IdString id) {
	எனில் (id == IdString::Null)
		திரும்பு std::string();
	திரும்பு pool ? pool->str(id) : ID::str(id);
}

உள்வரி std::string render_unescaped(மாறிலி TwinePool *pool, IdString id) {
	எனில் (id == IdString::Null)
		திரும்பு std::string();
	திரும்பு pool ? pool->unescaped_str(id) : ID::unescaped_str(id);
}

}

template<typename Derived>
கட்டமைப்பு WrappedIdBase {
	operator IdString() மாறிலி { திரும்பு self().ref(); }
	operator std::string() மாறிலி { திரும்பு self().escaped(); }
	std::string escaped() மாறிலி { திரும்பு wrapped_idstring_detail::render_escaped(self().pool(), self().ref()); }
	std::string unescape() மாறிலி { திரும்பு wrapped_idstring_detail::render_unescaped(self().pool(), self().ref()); }
	bool isPublic() மாறிலி { திரும்பு self().ref().isPublic(); }
	bool empty() மாறிலி { திரும்பு self().ref() == IdString::Null; }
	std::string str() மாறிலி { திரும்பு self().escaped(); }
	bool begins_with(மாறிலி எழுத்து *s) மாறிலி {
		மாறிலி TwinePool *pool = self().pool();
		திரும்பு pool ? pool->begins_with(self().ref(), s) : str().starts_with(s);
	}
	bool ends_with(மாறிலி எழுத்து *s) மாறிலி { திரும்பு str().ends_with(s); }
	template<typename... Ts> bool in(Ts &&...args) மாறிலி {
		திரும்பு self().ref().in(std::forward<Ts>(args)...);
	}
	bool in(மாறிலி pool<IdString> &rhs) மாறிலி { திரும்பு self().ref().in(rhs); }
	std::string substr(size_t pos = 0, size_t len = std::string::npos) மாறிலி {
		திரும்பு self().escaped().substr(pos, len);
	}
	size_t size() மாறிலி {
		மாறிலி TwinePool *pool = self().pool();
		திரும்பு pool ? pool->str_size(self().ref()) : self().escaped().size();
	}
	bool contains(மாறிலி எழுத்து *p) மாறிலி { திரும்பு self().escaped().find(p) != std::string::npos; }
	எழுத்து operator[](முழு n) மாறிலி { திரும்பு self().escaped()[n]; }
	bool lt_by_name(மாறிலி Derived &rhs) மாறிலி {
		மாறிலி TwinePool *pool = self().pool();
		எனில் (pool == nullptr)
			திரும்பு self().escaped() < rhs.escaped();
		திரும்பு pool->compare_by_name(self().ref(), rhs.ref()) < 0;
	}
	friend bool operator==(மாறிலி Derived &lhs, மாறிலி Derived &rhs) { திரும்பு lhs.ref() == rhs.ref(); }
	friend bool operator==(மாறிலி Derived &lhs, IdString rhs) { திரும்பு lhs.ref() == rhs; }
	friend bool operator==(மாறிலி Derived &lhs, NullIdString) { திரும்பு lhs.ref() == IdString::Null; }
	friend bool operator==(மாறிலி Derived &lhs, மாறிலி std::string &rhs) {
		மாறிலி TwinePool *pool = lhs.pool();
		திரும்பு pool ? pool->name_equal(lhs.ref(), rhs) : lhs.escaped() == rhs;
	}
	friend bool operator<(மாறிலி Derived &lhs, மாறிலி Derived &rhs) { திரும்பு lhs.ref() < rhs.ref(); }
தனிப்பட்ட:
	மாறிலி Derived &self() மாறிலி { திரும்பு *static_cast<மாறிலி Derived *>(this); }
};

template<typename T>
concept IsWrappedId = std::is_base_of_v<WrappedIdBase<std::decay_t<T>>, std::decay_t<T>>;

// Two wrapped IdStrings can be compared for equality by comparing .ref()
template<IsWrappedId A, IsWrappedId B>
requires (!std::is_same_v<A, B>)
உள்வரி bool operator==(மாறிலி A &lhs, மாறிலி B &rhs) { திரும்பு lhs.ref() == rhs.ref(); }

// A pair can be created from wrapped IdStrings by unwrapping them to IdString
// otherwise, you'd try to copy an OwnedId into the arguments of std::make_pair
// which would error out as its copy constructor is deleted
template<IsWrappedId A, typename B>
auto make_pair(A &&a, B &&b) { திரும்பு std::make_pair(a.ref(), std::forward<B>(b)); }
template<typename A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { திரும்பு std::make_pair(std::forward<A>(a), b.ref()); }
template<IsWrappedId A, IsWrappedId B>
auto make_pair(A &&a, B &&b) { திரும்பு std::make_pair(a.ref(), b.ref()); }

template<typename Owner, typename Slot>
கட்டமைப்பு OwnedId : WrappedIdBase<OwnedId<Owner, Slot>> {
	OwnedId() = இயல்பு;
	OwnedId(மாறிலி OwnedId &) = delete;
	OwnedId(OwnedId &&) = delete;
	IdString ref() மாறிலி { திரும்பு id_; }
	மாறிலி TwinePool *pool() மாறிலி;
	OwnedId &operator=(IdString id) { id_ = id; திரும்பு *this; }
	OwnedId &operator=(மாறிலி OwnedId &other) { திரும்பு *this = other.ref(); }
	OwnedId &operator=(OwnedId &&other) { திரும்பு *this = other.ref(); }
	friend வெறுமை swap(OwnedId &a, OwnedId &b) { std::swap(a.id_, b.id_); }
தனிப்பட்ட:
	மாறிலி Owner *owner() மாறிலி;
	IdString id_;
};

கட்டமைப்பு PooledName : WrappedIdBase<PooledName> {
	PooledName() = இயல்பு;
	explicit PooledName(IdString id) : id_(id) {}
	PooledName(மாறிலி TwinePool *pool, IdString id) : pool_(pool), id_(id) {}
	PooledName(மாறிலி Design *design, IdString id);
	PooledName(மாறிலி Module *module, IdString id);
	template<typename D> PooledName(மாறிலி WrappedIdBase<D> &masq)
		: pool_(static_cast<மாறிலி D &>(masq).pool()),
		  id_(static_cast<மாறிலி D &>(masq).ref()) {}
	IdString ref() மாறிலி { திரும்பு id_; }
	மாறிலி TwinePool *pool() மாறிலி { திரும்பு pool_; }
தனிப்பட்ட:
	மாறிலி TwinePool *pool_ = nullptr;
	IdString id_;
};

}

namespace hashlib {
	template<typename T>
	கட்டமைப்பு masq_hash_ops {
		நிலையான உள்வரி bool cmp(மாறிலி T &a, மாறிலி T &b) { திரும்பு a == b; }
		[[nodiscard]] நிலையான உள்வரி Hasher hash(மாறிலி T &a) {
			திரும்பு hash_ops<IdString>::hash(a.ref());
		}
		[[nodiscard]] நிலையான உள்வரி Hasher hash_into(மாறிலி T &a, Hasher h) {
			திரும்பு hash_ops<IdString>::hash_into(a.ref(), h);
		}
	};

	template<typename Owner, typename Slot>
	கட்டமைப்பு hash_ops<RTLIL::OwnedId<Owner, Slot>> : masq_hash_ops<RTLIL::OwnedId<Owner, Slot>> {};
	template<> கட்டமைப்பு hash_ops<RTLIL::PooledName> : masq_hash_ops<RTLIL::PooledName> {};
}

#endif
