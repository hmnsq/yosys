#اگر_تعریف_نشده RTLIL_TWINE_COMPAT_IMPL_H
#تعریف_کن RTLIL_TWINE_COMPAT_IMPL_H

#اگر_تعریف_شده __GNUC__
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Winvalid-offsetof"
#پایان_اگر

namespace RTLIL {

بنية Name_جایگاه {};
بنية Cell_Type_جایگاه {};

قالب<typename Owner, typename جایگاه_2>
مضمن ثابت Owner *OwnedId<Owner, جایگاه_2>::owner() ثابت {
	اندازه_t جابه_جایی;
	إذا constexpr (std::is_same_v<جایگاه_2, Name_جایگاه>) {
		جابه_جایی = offsetof(Owner, name);
	} وإلا {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<جایگاه_2, Cell_Type_جایگاه>);
		جابه_جایی = offsetof(Cell, type);
	}
	أرجع reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - جابه_جایی);
}

قالب<typename Owner, typename جایگاه_2>
مضمن ثابت TwinePool *OwnedId<Owner, جایگاه_2>::pool() ثابت {
	ثابت Owner *o = owner();
	إذا constexpr (std::is_same_v<Owner, Module>) {
		أرجع o->design ? &o->design->twines() : nullptr;
	} وإلا {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, حافظه> || std::is_same_v<Owner, Process>);
		أرجع o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

مضمن PooledName::PooledName(ثابت Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

مضمن PooledName::PooledName(ثابت Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#اگر_تعریف_شده __GNUC__
#رهنمود GCC diagnostic برداشت
#پایان_اگر

قالب<typename Derived>
مضمن فراغ log_dump_val_کارگر(ثابت RTLIL::Wrapped_Id_پایه<Derived> &name) {
	لگاریتم("%s", static_cast<ثابت Derived &>(name).unescape());
}

#پایان_اگر
