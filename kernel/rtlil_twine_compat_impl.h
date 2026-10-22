#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic थप
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

संरचना NameSlot {};
संरचना CellTypeSlot {};

ढाँचा<typename Owner, typename Slot>
अन्तःस्थ अचर Owner *OwnedId<Owner, Slot>::owner() अचर {
	माप_प्रकार offset;
	यदि constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = दुरत्व(Owner, name);
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = दुरत्व(Cell, type);
	}
	फर्काऊ reinterpret_cast<अचर Owner *>(reinterpret_cast<अचर वर्ण *>(this) - offset);
}

ढाँचा<typename Owner, typename Slot>
अन्तःस्थ अचर TwinePool *OwnedId<Owner, Slot>::pool() अचर {
	अचर Owner *o = owner();
	यदि constexpr (std::is_same_v<Owner, Module>) {
		फर्काऊ o->design ? &o->design->twines() : nullptr;
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		फर्काऊ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

अन्तःस्थ PooledName::PooledName(अचर Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

अन्तःस्थ PooledName::PooledName(अचर Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic झिक
#endif

ढाँचा<typename Derived>
अन्तःस्थ शून्य log_dump_val_worker(अचर RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<अचर Derived &>(name).unescape());
}

#endif
