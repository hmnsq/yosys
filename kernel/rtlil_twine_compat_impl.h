#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

কাঠামো NameSlot {};
কাঠামো CellTypeSlot {};

template<typename Owner, typename Slot>
অন্তঃরেখ ধ্রুবক Owner *OwnedId<Owner, Slot>::owner() ধ্রুবক {
	আকার_t offset;
	যদি constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} অন্যথা {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	ফেরত reinterpret_cast<ধ্রুবক Owner *>(reinterpret_cast<ধ্রুবক অক্ষর *>(this) - offset);
}

template<typename Owner, typename Slot>
অন্তঃরেখ ধ্রুবক TwinePool *OwnedId<Owner, Slot>::pool() ধ্রুবক {
	ধ্রুবক Owner *o = owner();
	যদি constexpr (std::is_same_v<Owner, Module>) {
		ফেরত o->design ? &o->design->twines() : nullptr;
	} অন্যথা {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		ফেরত o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

অন্তঃরেখ PooledName::PooledName(ধ্রুবক Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

অন্তঃরেখ PooledName::PooledName(ধ্রুবক Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic বের_করো
#endif

template<typename Derived>
অন্তঃরেখ শূন্য log_dump_val_worker(ধ্রুবক RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ধ্রুবক Derived &>(name).unescape());
}

#endif
