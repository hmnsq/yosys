#جيڪڏهن_وصف_نه_ٿيل RTLIL_TWINE_COMPAT_IMPL_H
#وصف_ڪريو RTLIL_TWINE_COMPAT_IMPL_H

#جيڪڏهن_وصف_ٿيل __GNUC__
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Winvalid-offsetof"
#جيڪڏهن_پورو

namespace RTLIL {

بنية Name_جڳهه {};
بنية Cell_Type_جڳهه {};

سانچو<typename Owner, typename جڳهه_2>
مضمن ثابت Owner *OwnedId<Owner, جڳهه_2>::owner() ثابت {
	سائيز_t وٿي;
	إذا constexpr (std::is_same_v<جڳهه_2, Name_جڳهه>) {
		وٿي = offsetof(Owner, name);
	} وإلا {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<جڳهه_2, Cell_Type_جڳهه>);
		وٿي = offsetof(Cell, type);
	}
	أرجع reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - وٿي);
}

سانچو<typename Owner, typename جڳهه_2>
مضمن ثابت TwinePool *OwnedId<Owner, جڳهه_2>::pool() ثابت {
	ثابت Owner *o = owner();
	إذا constexpr (std::is_same_v<Owner, Module>) {
		أرجع o->design ? &o->design->twines() : nullptr;
	} وإلا {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, يادگيري> || std::is_same_v<Owner, Process>);
		أرجع o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

مضمن PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

مضمن PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#جيڪڏهن_وصف_ٿيل __GNUC__
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

سانچو<typename Derived>
مضمن فراغ log_dump_val_ڪم_ڪندڙ(ثابت RTLIL::Wrapped_Id_بنياد<Derived> &name) {
	لاگرٿم("%s", static_cast<ثابت Derived &>(name).unescape());
}

#جيڪڏهن_پورو
