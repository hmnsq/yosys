#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RTLIL_TWINE_COMPAT_IMPL_H
#ವ್ಯಾಖ್ಯಾನಿಸು RTLIL_TWINE_COMPAT_IMPL_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Winvalid-offsetof"
#ಆದರೆ_ಅಂತ್ಯ

namespace RTLIL {

ರಚನೆ Name_ಸ್ಥಾನ {};
ರಚನೆ Cell_Type_ಸ್ಥಾನ {};

ಮಾದರಿ<typename Owner, typename ಸ್ಥಾನ_2>
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ Owner *OwnedId<Owner, ಸ್ಥಾನ_2>::owner() ಸ್ಥಿರಾಂಕ {
	ಗಾತ್ರ_t ಸ್ಥಾನಾಂತರ;
	ಆದರೆ constexpr (std::is_same_v<ಸ್ಥಾನ_2, Name_ಸ್ಥಾನ>) {
		ಸ್ಥಾನಾಂತರ = offsetof(Owner, name);
	} ಇಲ್ಲದಿದ್ದರೆ {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<ಸ್ಥಾನ_2, Cell_Type_ಸ್ಥಾನ>);
		ಸ್ಥಾನಾಂತರ = offsetof(Cell, type);
	}
	ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ Owner *>(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(this) - ಸ್ಥಾನಾಂತರ);
}

ಮಾದರಿ<typename Owner, typename ಸ್ಥಾನ_2>
ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ TwinePool *OwnedId<Owner, ಸ್ಥಾನ_2>::pool() ಸ್ಥಿರಾಂಕ {
	ಸ್ಥಿರಾಂಕ Owner *o = owner();
	ಆದರೆ constexpr (std::is_same_v<Owner, Module>) {
		ಹಿಂತಿರುಗಿ o->design ? &o->design->twines() : nullptr;
	} ಇಲ್ಲದಿದ್ದರೆ {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, ಸ್ಮರಣೆ> || std::is_same_v<Owner, Process>);
		ಹಿಂತಿರುಗಿ o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

ಸಾಲೊಳಗೆ PooledName::PooledName(ಸ್ಥಿರಾಂಕ Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

ಸಾಲೊಳಗೆ PooledName::PooledName(ಸ್ಥಿರಾಂಕ Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ_2
#ಆದರೆ_ಅಂತ್ಯ

ಮಾದರಿ<typename Derived>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ RTLIL::Wrapped_Id_ತಳ<Derived> &name) {
	ಲಘುಗಣಕ("%s", static_cast<ಸ್ಥಿರಾಂಕ Derived &>(name).unescape());
}

#ಆದರೆ_ಅಂತ್ಯ
