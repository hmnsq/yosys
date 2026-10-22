#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ادفع
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

بنية NameSlot {};
بنية CellTypeSlot {};

template<typename Owner, typename Slot>
مضمن ثابت Owner *OwnedId<Owner, Slot>::owner() ثابت {
	size_t offset;
	إذا constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} وإلا {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	إرجاع reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - offset);
}

template<typename Owner, typename Slot>
مضمن ثابت TwinePool *OwnedId<Owner, Slot>::pool() ثابت {
	ثابت Owner *o = owner();
	إذا constexpr (std::is_same_v<Owner, Module>) {
		إرجاع o->design ? &o->design->twines() : nullptr;
	} وإلا {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		إرجاع o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

مضمن PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

مضمن PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic اسحب
#endif

template<typename Derived>
مضمن فراغ log_dump_val_worker(ثابت RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ثابت Derived &>(name).unescape());
}

#endif
