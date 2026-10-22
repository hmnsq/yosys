#اگر_غیر_تعریف_شدہ RTLIL_TWINE_COMPAT_IMPL_H
#تعریف_کرو RTLIL_TWINE_COMPAT_IMPL_H

#اگر_تعریف_شدہ __GNUC__
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Winvalid-offsetof"
#اگر_ختم

namespace RTLIL {

ساخت Name_خانہ {};
ساخت Cell_Type_خانہ {};

سانچہ<typename Owner, typename خانہ_2>
اندرونی ثابت Owner *OwnedId<Owner, خانہ_2>::owner() ثابت {
	سائز_t انحراف;
	اگر constexpr (std::is_same_v<خانہ_2, Name_خانہ>) {
		انحراف = offsetof(Owner, name);
	} ورنہ {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<خانہ_2, Cell_Type_خانہ>);
		انحراف = offsetof(Cell, type);
	}
	واپس reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - انحراف);
}

سانچہ<typename Owner, typename خانہ_2>
اندرونی ثابت TwinePool *OwnedId<Owner, خانہ_2>::pool() ثابت {
	ثابت Owner *o = owner();
	اگر constexpr (std::is_same_v<Owner, Module>) {
		واپس o->design ? &o->design->twines() : nullptr;
	} ورنہ {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, حافظہ> || std::is_same_v<Owner, Process>);
		واپس o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

اندرونی PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

اندرونی PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#اگر_تعریف_شدہ __GNUC__
#ہدایت GCC diagnostic نکالو
#اگر_ختم

سانچہ<typename Derived>
اندرونی خالی log_dump_val_کارکن(ثابت RTLIL::Wrapped_Id_بنیاد<Derived> &name) {
	لوگارتھم("%s", static_cast<ثابت Derived &>(name).unescape());
}

#اگر_ختم
