#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

संरचना NameSlot {};
संरचना CellTypeSlot {};

ढाँचा<typename Owner, typename Slot>
ओळीत स्थिरांक Owner *OwnedId<Owner, Slot>::owner() स्थिरांक {
	माप_प्रकार offset;
	जर constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = दुरत्व(Owner, name);
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = दुरत्व(Cell, type);
	}
	परत reinterpret_cast<स्थिरांक Owner *>(reinterpret_cast<स्थिरांक अक्षर *>(this) - offset);
}

ढाँचा<typename Owner, typename Slot>
ओळीत स्थिरांक TwinePool *OwnedId<Owner, Slot>::pool() स्थिरांक {
	स्थिरांक Owner *o = owner();
	जर constexpr (std::is_same_v<Owner, Module>) {
		परत o->design ? &o->design->twines() : nullptr;
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		परत o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

ओळीत PooledName::PooledName(स्थिरांक Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

ओळीत PooledName::PooledName(स्थिरांक Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic काढा
#endif

ढाँचा<typename Derived>
ओळीत शून्य log_dump_val_worker(स्थिरांक RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<स्थिरांक Derived &>(name).unescape());
}

#endif
