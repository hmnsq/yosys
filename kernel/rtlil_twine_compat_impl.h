#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

संरचना NameSlot {};
संरचना CellTypeSlot {};

template<typename Owner, typename Slot>
अन्तःस्थ ध्रुव Owner *OwnedId<Owner, Slot>::owner() ध्रुव {
	size_t offset;
	यदि constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	प्रत्यागम reinterpret_cast<ध्रुव Owner *>(reinterpret_cast<ध्रुव वर्ण *>(this) - offset);
}

template<typename Owner, typename Slot>
अन्तःस्थ ध्रुव TwinePool *OwnedId<Owner, Slot>::pool() ध्रुव {
	ध्रुव Owner *o = owner();
	यदि constexpr (std::is_same_v<Owner, Module>) {
		प्रत्यागम o->design ? &o->design->twines() : nullptr;
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		प्रत्यागम o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

अन्तःस्थ PooledName::PooledName(ध्रुव Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

अन्तःस्थ PooledName::PooledName(ध्रुव Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic काढा
#endif

template<typename Derived>
अन्तःस्थ शून्य log_dump_val_worker(ध्रुव RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ध्रुव Derived &>(name).unescape());
}

#endif
