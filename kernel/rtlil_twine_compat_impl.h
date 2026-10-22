#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

கட்டமைப்பு NameSlot {};
கட்டமைப்பு CellTypeSlot {};

template<typename Owner, typename Slot>
உள்வரி மாறிலி Owner *OwnedId<Owner, Slot>::owner() மாறிலி {
	size_t offset;
	எனில் constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} இல்லையெனில் {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	திரும்பு reinterpret_cast<மாறிலி Owner *>(reinterpret_cast<மாறிலி எழுத்து *>(this) - offset);
}

template<typename Owner, typename Slot>
உள்வரி மாறிலி TwinePool *OwnedId<Owner, Slot>::pool() மாறிலி {
	மாறிலி Owner *o = owner();
	எனில் constexpr (std::is_same_v<Owner, Module>) {
		திரும்பு o->design ? &o->design->twines() : nullptr;
	} இல்லையெனில் {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		திரும்பு o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

உள்வரி PooledName::PooledName(மாறிலி Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

உள்வரி PooledName::PooledName(மாறிலி Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic எடு
#endif

template<typename Derived>
உள்வரி வெறுமை log_dump_val_worker(மாறிலி RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<மாறிலி Derived &>(name).unescape());
}

#endif
