#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

ساختار NameSlot {};
ساختار CellTypeSlot {};

template<typename Owner, typename Slot>
درونخطی ثابت Owner *OwnedId<Owner, Slot>::owner() ثابت {
	size_t offset;
	اگر constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} وگرنه {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	بازگشت reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت نویسه *>(this) - offset);
}

template<typename Owner, typename Slot>
درونخطی ثابت TwinePool *OwnedId<Owner, Slot>::pool() ثابت {
	ثابت Owner *o = owner();
	اگر constexpr (std::is_same_v<Owner, Module>) {
		بازگشت o->design ? &o->design->twines() : nullptr;
	} وگرنه {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		بازگشت o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

درونخطی PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

درونخطی PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic برداشت
#endif

template<typename Derived>
درونخطی تهی log_dump_val_worker(ثابت RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ثابت Derived &>(name).unescape());
}

#endif
