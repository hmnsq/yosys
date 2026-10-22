#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

sa_nracanaa NameSlot {};
sa_nracanaa CellTypeSlot {};

template<typename Owner, typename Slot>
anta_hstha const Owner *OwnedId<Owner, Slot>::owner() const {
	size_t offset;
	གལ་སྲིད constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} དེ་མིན {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	ལོག reinterpret_cast<const Owner *>(reinterpret_cast<const var_na *>(this) - offset);
}

template<typename Owner, typename Slot>
anta_hstha const TwinePool *OwnedId<Owner, Slot>::pool() const {
	const Owner *o = owner();
	གལ་སྲིད constexpr (std::is_same_v<Owner, Module>) {
		ལོག o->design ? &o->design->twines() : nullptr;
	} དེ་མིན {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		ལོག o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

anta_hstha PooledName::PooledName(const Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

anta_hstha PooledName::PooledName(const Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template<typename Derived>
anta_hstha void log_dump_val_worker(const RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<const Derived &>(name).unescape());
}

#endif
