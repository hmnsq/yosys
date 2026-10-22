#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

ഘടന NameSlot {};
ഘടന CellTypeSlot {};

template<typename Owner, typename Slot>
വരിയിൽ സ്ഥിരം Owner *OwnedId<Owner, Slot>::owner() സ്ഥിരം {
	size_t offset;
	എങ്കിൽ constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} അല്ലെങ്കിൽ {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	തിരികെ reinterpret_cast<സ്ഥിരം Owner *>(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(this) - offset);
}

template<typename Owner, typename Slot>
വരിയിൽ സ്ഥിരം TwinePool *OwnedId<Owner, Slot>::pool() സ്ഥിരം {
	സ്ഥിരം Owner *o = owner();
	എങ്കിൽ constexpr (std::is_same_v<Owner, Module>) {
		തിരികെ o->design ? &o->design->twines() : nullptr;
	} അല്ലെങ്കിൽ {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		തിരികെ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

വരിയിൽ PooledName::PooledName(സ്ഥിരം Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

വരിയിൽ PooledName::PooledName(സ്ഥിരം Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic എടുക്കുക
#endif

template<typename Derived>
വരിയിൽ ശൂന്യം log_dump_val_worker(സ്ഥിരം RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<സ്ഥിരം Derived &>(name).unescape());
}

#endif
