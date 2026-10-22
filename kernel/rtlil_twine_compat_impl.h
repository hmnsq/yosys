#که_نه_تعریف_شوی RTLIL_TWINE_COMPAT_IMPL_H
#تعریف_کړه RTLIL_TWINE_COMPAT_IMPL_H

#که_تعریف_شوی __GNUC__
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Winvalid-offsetof"
#د_که_پای

namespace RTLIL {

بنية Name_خونه {};
بنية Cell_Type_خونه {};

کينډۍ<typename Owner, typename خونه_2>
مضمن ثابت Owner *OwnedId<Owner, خونه_2>::owner() ثابت {
	اندازه_t دننه_واټن;
	إذا constexpr (std::is_same_v<خونه_2, Name_خونه>) {
		دننه_واټن = offsetof(Owner, name);
	} وإلا {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<خونه_2, Cell_Type_خونه>);
		دننه_واټن = offsetof(Cell, type);
	}
	أرجع reinterpret_cast<ثابت Owner *>(reinterpret_cast<ثابت حرف *>(this) - دننه_واټن);
}

کينډۍ<typename Owner, typename خونه_2>
مضمن ثابت TwinePool *OwnedId<Owner, خونه_2>::pool() ثابت {
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

#که_تعریف_شوی __GNUC__
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

کينډۍ<typename Derived>
مضمن فراغ log_dump_val_کارګر(ثابت RTLIL::Wrapped_Id_بنسټ<Derived> &name) {
	لوګاریتم("%s", static_cast<ثابت Derived &>(name).unescape());
}

#د_که_پای
