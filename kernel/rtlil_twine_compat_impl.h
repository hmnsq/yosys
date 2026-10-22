#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

struct NameSlot {};
struct CellTypeSlot {};

template<typename Owner, typename Slot>
inline const Owner *OwnedId<Owner, Slot>::owner() const {
	size_t offset;
	만약 constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} 아니면 {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	반환 reinterpret_cast<const Owner *>(reinterpret_cast<const char *>(this) - offset);
}

template<typename Owner, typename Slot>
inline const TwinePool *OwnedId<Owner, Slot>::pool() const {
	const Owner *o = owner();
	만약 constexpr (std::is_same_v<Owner, Module>) {
		반환 o->design ? &o->design->twines() : nullptr;
	} 아니면 {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		반환 o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

inline PooledName::PooledName(const Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

inline PooledName::PooledName(const Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template<typename Derived>
inline void log_dump_val_worker(const RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<const Derived &>(name).unescape());
}

#endif
