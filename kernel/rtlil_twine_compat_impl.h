#جے_بنا_تعریف RTLIL_TWINE_COMPAT_IMPL_H
#تعریف_کرو RTLIL_TWINE_COMPAT_IMPL_H

#جے_تعریف_شدہ __GNUC__
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Winvalid-offsetof"
#جے_انت

namespace RTLIL {

بنية Name_تھاں {};
بنية Cell_Type_تھاں {};

سانچا<typename Owner, typename تھاں_2>
مضمن ثابت Owner *OwnedId<Owner, تھاں_2>::owner() ثابت {
	ناپ_t وتھ;
	إذا constexpr (std::is_same_v<تھاں_2, Name_تھاں>) {
		وتھ = offsetof(Owner, name);
	} وإلا {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<تھاں_2, Cell_Type_تھاں>);
		وتھ = offsetof(Cell, type);
	}
	أرجع reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - وتھ);
}

سانچا<typename Owner, typename تھاں_2>
مضمن ثابت TwinePool *OwnedId<Owner, تھاں_2>::pool() ثابت {
	ثابت Owner *o = owner();
	إذا constexpr (std::is_same_v<Owner, Module>) {
		أرجع o->design ? &o->design->twines() : nullptr;
	} وإلا {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, حافظہ> || std::is_same_v<Owner, Process>);
		أرجع o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

مضمن PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

مضمن PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#جے_تعریف_شدہ __GNUC__
#ہدایت GCC diagnostic نکالو
#جے_انت

سانچا<typename Derived>
مضمن فراغ log_dump_val_کامہ(ثابت RTLIL::Wrapped_Id_ادھار<Derived> &name) {
	لوگارتھم("%s", static_cast<ثابت Derived &>(name).unescape());
}

#جے_انت
