#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

સંરચના NameSlot {};
સંરચના CellTypeSlot {};

template<typename Owner, typename Slot>
અંતઃરેખ અચલ Owner *OwnedId<Owner, Slot>::owner() અચલ {
	size_t offset;
	જો constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} નહીંતર {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	પાછા reinterpret_cast<અચલ Owner *>(reinterpret_cast<અચલ અક્ષર *>(this) - offset);
}

template<typename Owner, typename Slot>
અંતઃરેખ અચલ TwinePool *OwnedId<Owner, Slot>::pool() અચલ {
	અચલ Owner *o = owner();
	જો constexpr (std::is_same_v<Owner, Module>) {
		પાછા o->design ? &o->design->twines() : nullptr;
	} નહીંતર {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		પાછા o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

અંતઃરેખ PooledName::PooledName(અચલ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

અંતઃરેખ PooledName::PooledName(અચલ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic કાઢો
#endif

template<typename Derived>
અંતઃરેખ શૂન્ય log_dump_val_worker(અચલ RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<અચલ Derived &>(name).unescape());
}

#endif
