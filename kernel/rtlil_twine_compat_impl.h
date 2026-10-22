#अगर_अघोषित RTLIL_TWINE_COMPAT_IMPL_H
#घोषणा RTLIL_TWINE_COMPAT_IMPL_H

#अगर_घोषित __GNUC__
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Winvalid-offsetof"
#पूर्ण_अगर

namespace RTLIL {

काष्ठा NameSlot {};
काष्ठा CellTypeSlot {};

ढाँचा<typename Owner, typename Slot>
अंतरभूत स्थिर Owner *OwnedId<Owner, Slot>::owner() स्थिर {
	माप_प्रकार offset;
	अगर constexpr (std::is_same_v<Slot, NameSlot>) {
		offset = दुरत्व(Owner, name);
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<Slot, CellTypeSlot>);
		offset = दुरत्व(Cell, type);
	}
	वापस reinterpret_cast<स्थिर Owner *>(reinterpret_cast<स्थिर अक्षर *>(this) - offset);
}

ढाँचा<typename Owner, typename Slot>
अंतरभूत स्थिर TwinePool *OwnedId<Owner, Slot>::pool() स्थिर {
	स्थिर Owner *o = owner();
	अगर constexpr (std::is_same_v<Owner, Module>) {
		वापस o->design ? &o->design->twines() : nullptr;
	} अन्यथा {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, Memory> || std::is_same_v<Owner, Process>);
		वापस o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

अंतरभूत PooledName::PooledName(स्थिर Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

अंतरभूत PooledName::PooledName(स्थिर Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#अगर_घोषित __GNUC__
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

ढाँचा<typename Derived>
अंतरभूत व्योम log_dump_val_worker(स्थिर RTLIL::WrappedIdBase<Derived> &name) {
	log("%s", static_cast<स्थिर Derived &>(name).unescape());
}

#पूर्ण_अगर
