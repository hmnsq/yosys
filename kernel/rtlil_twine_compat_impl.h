#నిర్వచితం_కాకపోతే RTLIL_TWINE_COMPAT_IMPL_H
#నిర్వచించు RTLIL_TWINE_COMPAT_IMPL_H

#నిర్వచితమైతే __GNUC__
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Winvalid-offsetof"
#అయితే_ముగింపు

namespace RTLIL {

నిర్మాణం Name_స్థానం {};
నిర్మాణం Cell_Type_స్థానం {};

మూస<typename Owner, typename స్థానం_2>
అంతఃవరుస స్థిరరాశి Owner *OwnedId<Owner, స్థానం_2>::owner() స్థిరరాశి {
	కొలత_t స్థానభ్రంశం;
	ఒకవేళ constexpr (std::is_same_v<స్థానం_2, Name_స్థానం>) {
		స్థానభ్రంశం = offsetof(Owner, name);
	} లేకపోతే {
		static_assert(std::is_same_v<Owner, Cell> && std::is_same_v<స్థానం_2, Cell_Type_స్థానం>);
		స్థానభ్రంశం = offsetof(Cell, type);
	}
	తిరిగి reinterpret_cast<స్థిరరాశి Owner *>(reinterpret_cast<స్థిరరాశి అక్షరం *>(this) - స్థానభ్రంశం);
}

మూస<typename Owner, typename స్థానం_2>
అంతఃవరుస స్థిరరాశి TwinePool *OwnedId<Owner, స్థానం_2>::pool() స్థిరరాశి {
	స్థిరరాశి Owner *o = owner();
	ఒకవేళ constexpr (std::is_same_v<Owner, Module>) {
		తిరిగి o->design ? &o->design->twines() : nullptr;
	} లేకపోతే {
		static_assert(std::is_same_v<Owner, Wire> || std::is_same_v<Owner, Cell>
				|| std::is_same_v<Owner, జ్ఞాపకం> || std::is_same_v<Owner, Process>);
		తిరిగి o->module && o->module->design ? &o->module->design->twines() : nullptr;
	}
}

అంతఃవరుస PooledName::PooledName(స్థిరరాశి Design *design, IdString id)
	: pool_(design ? &design->twines() : nullptr), id_(id) { }

అంతఃవరుస PooledName::PooledName(స్థిరరాశి Module *module, IdString id)
	: PooledName(module ? module->design : nullptr, id) { }

}

#నిర్వచితమైతే __GNUC__
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

మూస<typename Derived>
అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(స్థిరరాశి RTLIL::Wrapped_Id_పునాది<Derived> &name) {
	సంవర్గమానం("%s", static_cast<స్థిరరాశి Derived &>(name).unescape());
}

#అయితే_ముగింపు
