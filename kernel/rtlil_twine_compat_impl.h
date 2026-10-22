#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RTLIL_TWINE_COMPAT_IMPL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTLIL_TWINE_COMPAT_IMPL_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Winvalid-offsetof"
#ਜੇ_ਅੰਤ

namespace RTLIL {

ਬਣਤਰ Name_ਥਾਂ {};
ਬਣਤਰ Cell_Type_ਥਾਂ {};

ਸਾਂਚਾ<typename Owner, typename ਥਾਂ_2>
ਸਤਰਅੰਦਰ ਅਚਲ Owner *OwnedId<Owner, ਥਾਂ_2>::owner() ਅਚਲ {
	ਮਾਪ_t ਵਿੱਥ;
	ਜੇ constexpr (std::is_same_v<ਥਾਂ_2, Name_ਥਾਂ>) {
		ਵਿੱਥ = offsetof(Owner, name);
	} ਨਹੀਂਤਾਂ {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<ਥਾਂ_2, Cell_Type_ਥਾਂ>);
		ਵਿੱਥ = offsetof(Cell, type);
	}
	ਵਾਪਸ reinterpret_cast<ਅਚਲ Owner *>(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(this) - ਵਿੱਥ);
}

ਸਾਂਚਾ<typename Owner, typename ਥਾਂ_2>
ਸਤਰਅੰਦਰ ਅਚਲ TwinePool *OwnedId<Owner, ਥਾਂ_2>::pool() ਅਚਲ {
	ਅਚਲ Owner *o = owner();
	ਜੇ constexpr (std::is_same_v<Owner, Module>) {
		ਵਾਪਸ o->design ? &o->design->twines() : nullptr;
	} ਨਹੀਂਤਾਂ {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, ਸਮ੍ਰਿਤੀ> || std::is_same_v<Owner, Process>);
		ਵਾਪਸ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

ਸਤਰਅੰਦਰ PooledName::PooledName(ਅਚਲ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

ਸਤਰਅੰਦਰ PooledName::PooledName(ਅਚਲ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

ਸਾਂਚਾ<typename Derived>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਅਚਲ RTLIL::Wrapped_Id_ਅਧਾਰ<Derived> &name) {
	ਲਘੂਗਣਕ("%s", static_cast<ਅਚਲ Derived &>(name).unescape());
}

#ਜੇ_ਅੰਤ
