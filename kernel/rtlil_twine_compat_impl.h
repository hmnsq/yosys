#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

ಸಂರಚನಾ NameSlot {};
ಸಂರಚನಾ CellTypeSlot {};

template<typename Owner, typename Slot>
ಅನ್ತಃಸ್ಥ ಧ್ರುವ Owner *OwnedId<Owner, Slot>::owner() ಧ್ರುವ {
	size_t offset;
	ಯದಿ constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} ಅನ್ಯಥಾ {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ Owner *>(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(this) - offset);
}

template<typename Owner, typename Slot>
ಅನ್ತಃಸ್ಥ ಧ್ರುವ TwinePool *OwnedId<Owner, Slot>::pool() ಧ್ರುವ {
	ಧ್ರುವ Owner *o = owner();
	ಯದಿ constexpr (std::is_same_v<Owner, Module>) {
		ಪ್ರತ್ಯಾಗಮ o->design ? &o->design->twines() : nullptr;
	} ಅನ್ಯಥಾ {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		ಪ್ರತ್ಯಾಗಮ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

ಅನ್ತಃಸ್ಥ PooledName::PooledName(ಧ್ರುವ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

ಅನ್ತಃಸ್ಥ PooledName::PooledName(ಧ್ರುವ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic ತೆಗೆ
#endif

template<typename Derived>
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ log_dump_val_worker(ಧ್ರುವ RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ಧ್ರುವ Derived &>(name).unescape());
}

#endif
