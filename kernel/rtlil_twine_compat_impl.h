#यदि_अपरिभाषितम् RTLIL_TWINE_COMPAT_IMPL_H
#परिभाषय RTLIL_TWINE_COMPAT_IMPL_H

#यदि_परिभाषितम् __GNUC__
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Winvalid-offsetof"
#यदि_अन्तः

namespace RTLIL {

संरचना NameSlot {};
संरचना CellTypeSlot {};

प्रतिरूपम्<typename Owner, typename Slot>
अन्तःस्थ ध्रुव Owner *OwnedId<Owner, Slot>::owner() ध्रुव {
	आकार_t विस्थापनम्;
	यदि constexpr (std::is_same_v<Slot, NameSlot>) {
		विस्थापनम् = offsetof(Owner, name);
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		विस्थापनम् = offsetof(Cell, type);
	}
	प्रत्यागम reinterpret_cast<ध्रुव Owner *>(reinterpret_cast<ध्रुव वर्ण *>(this) - विस्थापनम्);
}

प्रतिरूपम्<typename Owner, typename Slot>
अन्तःस्थ ध्रुव TwinePool *OwnedId<Owner, Slot>::pool() ध्रुव {
	ध्रुव Owner *o = owner();
	यदि constexpr (std::is_same_v<Owner, Module>) {
		प्रत्यागम o->design ? &o->design->twines() : nullptr;
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, स्मृतिः> || std::is_same_v<Owner, Process>);
		प्रत्यागम o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

अन्तःस्थ PooledName::PooledName(ध्रुव Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

अन्तःस्थ PooledName::PooledName(ध्रुव Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#यदि_परिभाषितम् __GNUC__
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

प्रतिरूपम्<typename Derived>
अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(ध्रुव RTLIL::Wrapped_Id_आधारः<Derived> &name) {
	लघुगणकः("%s", static_cast<ध्रुव Derived &>(name).unescape());
}

#यदि_अन्तः
