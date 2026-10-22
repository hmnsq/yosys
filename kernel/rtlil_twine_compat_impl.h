#ifndef RTLIL_TWINE_COMPAT_IMPL_H
#define RTLIL_TWINE_COMPAT_IMPL_H

#ifdef __GNUC__
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

namespace RTLIL {

ସଂରଚନା NameSlot {};
ସଂରଚନା CellTypeSlot {};

template<typename Owner, typename Slot>
ଧାଡ଼ିଭିତରେ ଅଚଳ Owner *OwnedId<Owner, Slot>::owner() ଅଚଳ {
	size_t offset;
	ଯଦି constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = offsetof(Owner, name);
	} ଅନ୍ୟଥା {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = offsetof(Cell, type);
	}
	ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ Owner *>(reinterpret_cast<ଅଚଳ ଅକ୍ଷର *>(this) - offset);
}

template<typename Owner, typename Slot>
ଧାଡ଼ିଭିତରେ ଅଚଳ TwinePool *OwnedId<Owner, Slot>::pool() ଅଚଳ {
	ଅଚଳ Owner *o = owner();
	ଯଦି constexpr (std::is_same_v<Owner, Module>) {
		ଫେରନ୍ତୁ o->design ? &o->design->twines() : nullptr;
	} ଅନ୍ୟଥା {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		ଫେରନ୍ତୁ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

ଧାଡ଼ିଭିତରେ PooledName::PooledName(ଅଚଳ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

ଧାଡ଼ିଭିତରେ PooledName::PooledName(ଅଚଳ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ifdef __GNUC__
#pragma GCC diagnostic କାଢ଼
#endif

template<typename Derived>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଅଚଳ RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<ଅଚଳ Derived &>(name).unescape());
}

#endif
