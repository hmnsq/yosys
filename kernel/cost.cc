#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/cost.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/macc.h"

USING_YOSYS_NAMESPACE

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ CellCosts::get(RTLIL::Module *mod)
{
	ਜੇ (mod_ਲਾਗਤ_cache.ਗਿਣਤੀ_2(mod->name))
		ਵਾਪਸ mod_ਲਾਗਤ_cache.at(mod->name);

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ module_ਲਾਗਤ = 1;
	ਲਈ (ਸਵੈਚਾਲਿਤ c : mod->cells()) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ new_ਲਾਗਤ = module_ਲਾਗਤ + get(c);
		module_ਲਾਗਤ = new_ਲਾਗਤ >= module_ਲਾਗਤ ? new_ਲਾਗਤ : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
	}

	mod_ਲਾਗਤ_cache[mod->name] = module_ਲਾਗਤ;
	ਵਾਪਸ module_ਲਾਗਤ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ y_coef(RTLIL::IdString type)
{
	ਜੇ (
	  // equality
	  type.ਵਿੱਚ(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.ਵਿੱਚ(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.ਵਿੱਚ(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ਵਾਪਸ 1;
	} ਨਹੀਂਤਾਂ ਜੇ (type == ID($neg)) {
		ਵਾਪਸ 4;
	} ਨਹੀਂਤਾਂ ਜੇ (type == ID($demux)) {
		ਵਾਪਸ 2;
	} ਨਹੀਂਤਾਂ ਜੇ (type == ID($fa)) {
		ਵਾਪਸ 5;
	} ਨਹੀਂਤਾਂ ਜੇ (type.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub), ID($alu))) {
		// multi-bit adders
		ਵਾਪਸ 8;
	} ਨਹੀਂਤਾਂ ਜੇ (type.ਵਿੱਚ(ID($shl), ID($sshl))) {
		// left shift
		ਵਾਪਸ 10;
	}
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_inp_coef(RTLIL::IdString type)
{
	ਜੇ (
	  // binop reduce
	  type.ਵਿੱਚ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.ਵਿੱਚ(ID($logic_not), ID($pmux), ID($bmux))) {
		ਵਾਪਸ 1;
	} ਨਹੀਂਤਾਂ ਜੇ (
	  // equality
	  type.ਵਿੱਚ(ID($eq), ID($ne)) ||
	  // logic
	  type.ਵਿੱਚ(ID($logic_and), ID($logic_or))) {
		ਵਾਪਸ 2;
	} ਨਹੀਂਤਾਂ ਜੇ (type == ID($lcu)) {
		ਵਾਪਸ 5;
	} ਨਹੀਂਤਾਂ ਜੇ (type.ਵਿੱਚ(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ਵਾਪਸ 7;
	}
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਜੋੜ_coef(RTLIL::IdString type)
{
	ਜੇ (type.ਵਿੱਚ(ID($shr), ID($sshr))) {
		// right shift
		ਵਾਪਸ 4;
	} ਨਹੀਂਤਾਂ ਜੇ (type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx))) {
		//ਖਿਸਕਾਓ
		ਵਾਪਸ 8;
	}
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ is_div_mod(RTLIL::IdString type)
{
	ਵਾਪਸ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ਸਥਿਰ ਤਾਰਕਿਕ is_free(RTLIL::IdString type)
{
	ਵਾਪਸ (
	  //ਟੈਗ_ਸੂਚੀ
	  type.ਵਿੱਚ(ID($overwrite_ਟੈਗ), ID($ਸੈੱਟ_ਟੈਗ), ID($original_ਟੈਗ), ID($get_ਟੈਗ)) ||
	  // formal
	  type.ਵਿੱਚ(ID($check), ID($equiv), ID($initstate), ID($ਪ੍ਰਤਿਜ੍ਞਾ), ID($assume), ID($ਜੀਵਤ), ID($cover), ID($fair)) ||
	  type.ਵਿੱਚ(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.ਵਿੱਚ(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.ਵਿੱਚ(ID($concat), ID($ਟੁਕੜਾ), ID($pos)) ||
	  // specify
	  type.ਵਿੱਚ(ID($specrule), ID($specify2), ID($specify3)));
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_inp_ਚੌੜਾਈ(RTLIL::Cell *cell)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max = 0;
	RTLIL::IdString ਨਿਵੇਸ਼_ਚੌੜਾਈ_params[] = {
	  ID::ਚੌੜਾਈ_2,
	  ID::A_ਚੌੜਾਈ,
	  ID::B_ਚੌੜਾਈ,
	  ID::S_ਚੌੜਾਈ,
	};

	ਜੇ (cell->type == ID($bmux))
		ਵਾਪਸ cell->getParam(ID::ਚੌੜਾਈ_2).as_int() << cell->getParam(ID::S_ਚੌੜਾਈ).as_int();

	ਲਈ (RTLIL::IdString param : ਨਿਵੇਸ਼_ਚੌੜਾਈ_params)
		ਜੇ (cell->hasParam(param))
			max = std::max(max, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cell->getParam(param).as_int());
	ਵਾਪਸ max;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪੋਰਟ_ਚੌੜਾਈ_ਜੋੜ(RTLIL::Cell *cell)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਜੋੜ = 0;
	RTLIL::IdString ਪੋਰਟ_ਚੌੜਾਈ_params[] = {
	  ID::ਚੌੜਾਈ_2, ID::A_ਚੌੜਾਈ, ID::B_ਚੌੜਾਈ, ID::S_ਚੌੜਾਈ, ID::Y_ਚੌੜਾਈ,
	};

	ਲਈ (ਸਵੈਚਾਲਿਤ param : ਪੋਰਟ_ਚੌੜਾਈ_params)
		ਜੇ (cell->hasParam(param))
			ਜੋੜ += cell->getParam(param).as_int();

	ਵਾਪਸ ਜੋੜ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ਜੇ (cmos_gate_ਲਾਗਤ().ਗਿਣਤੀ_2(cell->type))
		ਵਾਪਸ 1;

	ਜੇ (design_ && design_->module(cell->type) && cell->parameters.ਖਾਲੀ_ਥਾਵਾਂ()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ਵਾਪਸ get(design_->module(cell->type));
	} ਨਹੀਂਤਾਂ ਜੇ (cell->is_builtin_ff()) {
		log_assert(cell->has_ਪੋਰਟ(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ਵਾਪਸ cell->getParam(ID::ਚੌੜਾਈ_2).as_int();
	} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ਵਾਪਸ cell->getParam(ID::ਚੌੜਾਈ_2).as_int() * cell->getParam(ID::ਮਾਪ_2).as_int();
	} ਨਹੀਂਤਾਂ ਜੇ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_ਚੌੜਾਈ) || cell->hasParam(ID::ਚੌੜਾਈ_2)) && "Unknown width");
		ਸਵੈਚਾਲਿਤ param = cell->hasParam(ID::Y_ਚੌੜਾਈ) ? ID::Y_ਚੌੜਾਈ : ID::ਚੌੜਾਈ_2;
		ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(param).as_int();
		ਜੇ (cell->type == ID($demux))
			ਚੌੜਾਈ <<= cell->getParam(ID::S_ਚੌੜਾਈ).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, ਚੌੜਾਈ, y_coef(cell->type));
		ਵਾਪਸ ਚੌੜਾਈ * y_coef(cell->type);
	} ਨਹੀਂਤਾਂ ਜੇ (ਜੋੜ_coef(cell->type)) {
		// linear with sum of port widths
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਜੋੜ = ਪੋਰਟ_ਚੌੜਾਈ_ਜੋੜ(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, ਜੋੜ, ਜੋੜ_coef(cell->type));
		ਵਾਪਸ ਜੋੜ * ਜੋੜ_coef(cell->type);
	} ਨਹੀਂਤਾਂ ਜੇ (max_inp_coef(cell->type)) {
		// linear with largest input width
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max = max_inp_ਚੌੜਾਈ(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ਵਾਪਸ max * max_inp_coef(cell->type);
	} ਨਹੀਂਤਾਂ ਜੇ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਜੋੜ = ਪੋਰਟ_ਚੌੜਾਈ_ਜੋੜ(cell);
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, ਜੋੜ * ਜੋੜ);
		ਵਾਪਸ coef * ਜੋੜ * ਜੋੜ;
	} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($macc), ID($macc_v2))) {
		// quadratic per term
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲਾਗਤ_ਜੋੜ = 0;
		Macc macc;
		macc.from_cell(cell);
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ y_ਚੌੜਾਈ = cell->getParam(ID::Y_ਚੌੜਾਈ).as_int();
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪਦ: macc.terms) {
			ਜੇ (ਪਦ.ਵਿੱਚ_b.ਖਾਲੀ_ਥਾਵਾਂ()) {
				// neglect addends
				ਜਾਰੀ;
			}
			ਬਿਨਾਨਿਸ਼ਾਨੀ a_ਚੌੜਾਈ = ਪਦ.ਵਿੱਚ_a.ਮਾਪ(), b_ਚੌੜਾਈ = ਪਦ.ਵਿੱਚ_b.ਮਾਪ();
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਜੋੜ = a_ਚੌੜਾਈ + b_ਚੌੜਾਈ + std::min(y_ਚੌੜਾਈ, a_ਚੌੜਾਈ + b_ਚੌੜਾਈ);
			ਲਾਗਤ_ਜੋੜ += 3 * ਜੋੜ * ਜੋੜ;
		}
		ਵਾਪਸ ਲਾਗਤ_ਜੋੜ;
	} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($lut)) {
		ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(ID::ਚੌੜਾਈ_2).as_int();
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲਾਗਤ = 1U << (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਚੌੜਾਈ;
		log_debug("%s is 2**%d\n", cell->name, ਚੌੜਾਈ);
		ਵਾਪਸ ਲਾਗਤ;
	} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($sop)) {
		ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(ID::ਚੌੜਾਈ_2).as_int();
		ਪੂਰਨ ਡੂੰਘਾਈ = cell->getParam(ID::ਡੂੰਘਾਈ_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, ਚੌੜਾਈ, ਡੂੰਘਾਈ);
		ਵਾਪਸ (2 * ਚੌੜਾਈ + 1) * ਡੂੰਘਾਈ;
	} ਨਹੀਂਤਾਂ ਜੇ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ਵਾਪਸ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_ਮਾਪ(cell->parameters));
	ਵਾਪਸ 1;
}
