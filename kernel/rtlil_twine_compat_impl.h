#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

সংরচনা NameSlot {};
সংরচনা CellTypeSlot {};

template<typename Owner, typename Slot>
অন্তঃস্থ ধ্রুৱ Owner *OwnedId<Owner, Slot>::owner() ধ্রুৱ {
	size_t offset;
	যদি constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} অন্যথা {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	ঘূৰাই reinterpret_cast<ধ্রুৱ Owner *>(reinterpret_cast<ধ্রুৱ ৱর্ণ *>(this) - offset);
}

template<typename Owner, typename Slot>
অন্তঃস্থ ধ্রুৱ TwinePool *OwnedId<Owner, Slot>::pool() ধ্রুৱ {
	ধ্রুৱ Owner *o = owner();
	যদি constexpr (std::is_same_v<Owner, Module>) {
		ঘূৰাই o->design ? &o->design->twines() : nullptr;
	} অন্যথা {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		ঘূৰাই o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

অন্তঃস্থ PooledName::PooledName(ধ্রুৱ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

অন্তঃস্থ PooledName::PooledName(ধ্রুৱ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic বেৰ_কৰো
#endif

template<typename Derived>
অন্তঃস্থ শূন্য log_dump_val_worker(ধ্রুৱ RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ধ্রুৱ Derived &>(name).unescape());
}

#endif
