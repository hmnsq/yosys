#ضمن "kernel/cost.h"
#ضمن "kernel/macc.h"

USING_YOSYS_NAMESPACE

بنانشان صحيح CellCosts::get(RTLIL::Module *mod)
{
	إذا (mod_لاڳت_cache.ڳڻپ(mod->name))
		أرجع mod_لاڳت_cache.at(mod->name);

	بنانشان صحيح module_لاڳت = 1;
	لكل (خودڪار c : mod->cells()) {
		بنانشان صحيح new_لاڳت = module_لاڳت + get(c);
		module_لاڳت = new_لاڳت >= module_لاڳت ? new_لاڳت : وڌ_کان_وڌ_عدد;
	}

	mod_لاڳت_cache[mod->name] = module_لاڳت;
	أرجع module_لاڳت;
}

ساكن بنانشان صحيح y_coef(RTLIL::IdString type)
{
	إذا (
	  // equality
	  type.اندريون(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.اندريون(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.اندريون(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		أرجع 1;
	} وإلا إذا (type == ID($neg)) {
		أرجع 4;
	} وإلا إذا (type == ID($demux)) {
		أرجع 2;
	} وإلا إذا (type == ID($fa)) {
		أرجع 5;
	} وإلا إذا (type.اندريون(ID($جوڙيو), ID($sub), ID($alu))) {
		// multi-bit adders
		أرجع 8;
	} وإلا إذا (type.اندريون(ID($shl), ID($sshl))) {
		// left shift
		أرجع 10;
	}
	أرجع 0;
}

ساكن بنانشان صحيح max_inp_coef(RTLIL::IdString type)
{
	إذا (
	  // binop reduce
	  type.اندريون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.اندريون(ID($logic_not), ID($pmux), ID($bmux))) {
		أرجع 1;
	} وإلا إذا (
	  // equality
	  type.اندريون(ID($eq), ID($ne)) ||
	  // logic
	  type.اندريون(ID($logic_and), ID($logic_or))) {
		أرجع 2;
	} وإلا إذا (type == ID($lcu)) {
		أرجع 5;
	} وإلا إذا (type.اندريون(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		أرجع 7;
	}
	أرجع 0;
}

ساكن بنانشان صحيح جوڙ_coef(RTLIL::IdString type)
{
	إذا (type.اندريون(ID($shr), ID($sshr))) {
		// right shift
		أرجع 4;
	} وإلا إذا (type.اندريون(ID($سرڪايو), ID($shiftx))) {
		//سرڪايو
		أرجع 8;
	}
	أرجع 0;
}

ساكن بنانشان صحيح is_div_mod(RTLIL::IdString type)
{
	أرجع (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساكن منطقي is_free(RTLIL::IdString type)
{
	أرجع (
	  //ٽيگ_فهرست
	  type.اندريون(ID($overwrite_ٽيگ), ID($سيٽ_ٽيگ), ID($original_ٽيگ), ID($get_ٽيگ)) ||
	  // formal
	  type.اندريون(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($جيئرو), ID($cover), ID($fair)) ||
	  type.اندريون(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.اندريون(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.اندريون(ID($concat), ID($ٽڪرو), ID($pos)) ||
	  // specify
	  type.اندريون(ID($specrule), ID($specify2), ID($specify3)));
}

بنانشان صحيح max_inp_ويڪر(RTLIL::Cell *cell)
{
	بنانشان صحيح max = 0;
	RTLIL::IdString داخلا_ويڪر_params[] = {
	  ID::ويڪر_2,
	  ID::A_ويڪر,
	  ID::B_ويڪر,
	  ID::S_ويڪر,
	};

	إذا (cell->type == ID($bmux))
		أرجع cell->getParam(ID::ويڪر_2).as_int() << cell->getParam(ID::S_ويڪر).as_int();

	لكل (RTLIL::IdString param : داخلا_ويڪر_params)
		إذا (cell->hasParam(param))
			max = std::max(max, (بنانشان صحيح)cell->getParam(param).as_int());
	أرجع max;
}

بنانشان صحيح پورٽ_ويڪر_جوڙ(RTLIL::Cell *cell)
{
	بنانشان صحيح جوڙ = 0;
	RTLIL::IdString پورٽ_ويڪر_params[] = {
	  ID::ويڪر_2, ID::A_ويڪر, ID::B_ويڪر, ID::S_ويڪر, ID::Y_ويڪر,
	};

	لكل (خودڪار param : پورٽ_ويڪر_params)
		إذا (cell->hasParam(param))
			جوڙ += cell->getParam(param).as_int();

	أرجع جوڙ;
}

بنانشان صحيح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	إذا (cmos_gate_لاڳت().ڳڻپ(cell->type))
		أرجع 1;

	إذا (design_ && design_->module(cell->type) && cell->parameters.خالي_جايون()) {
		log_debug("%s is a module, recurse\n", cell->name);
		أرجع get(design_->module(cell->type));
	} وإلا إذا (cell->is_builtin_ff()) {
		log_assert(cell->has_پورٽ(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		أرجع cell->getParam(ID::ويڪر_2).as_int();
	} وإلا إذا (cell->type.اندريون(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		أرجع cell->getParam(ID::ويڪر_2).as_int() * cell->getParam(ID::سائيز_2).as_int();
	} وإلا إذا (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_ويڪر) || cell->hasParam(ID::ويڪر_2)) && "Unknown width");
		خودڪار param = cell->hasParam(ID::Y_ويڪر) ? ID::Y_ويڪر : ID::ويڪر_2;
		صحيح ويڪر = cell->getParam(param).as_int();
		إذا (cell->type == ID($demux))
			ويڪر <<= cell->getParam(ID::S_ويڪر).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, ويڪر, y_coef(cell->type));
		أرجع ويڪر * y_coef(cell->type);
	} وإلا إذا (جوڙ_coef(cell->type)) {
		// linear with sum of port widths
		بنانشان صحيح جوڙ = پورٽ_ويڪر_جوڙ(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, جوڙ, جوڙ_coef(cell->type));
		أرجع جوڙ * جوڙ_coef(cell->type);
	} وإلا إذا (max_inp_coef(cell->type)) {
		// linear with largest input width
		بنانشان صحيح max = max_inp_ويڪر(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		أرجع max * max_inp_coef(cell->type);
	} وإلا إذا (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		بنانشان صحيح جوڙ = پورٽ_ويڪر_جوڙ(cell);
		بنانشان صحيح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, جوڙ * جوڙ);
		أرجع coef * جوڙ * جوڙ;
	} وإلا إذا (cell->type.اندريون(ID($macc), ID($macc_v2))) {
		// quadratic per term
		بنانشان صحيح لاڳت_جوڙ = 0;
		Macc macc;
		macc.from_cell(cell);
		بنانشان صحيح y_ويڪر = cell->getParam(ID::Y_ويڪر).as_int();
		لكل (خودڪار &رڪن: macc.terms) {
			إذا (رڪن.اندريون_b.خالي_جايون()) {
				// neglect addends
				استمر;
			}
			بنانشان a_ويڪر = رڪن.اندريون_a.سائيز(), b_ويڪر = رڪن.اندريون_b.سائيز();
			بنانشان صحيح جوڙ = a_ويڪر + b_ويڪر + std::min(y_ويڪر, a_ويڪر + b_ويڪر);
			لاڳت_جوڙ += 3 * جوڙ * جوڙ;
		}
		أرجع لاڳت_جوڙ;
	} وإلا إذا (cell->type == ID($lut)) {
		صحيح ويڪر = cell->getParam(ID::ويڪر_2).as_int();
		بنانشان صحيح لاڳت = 1U << (بنانشان صحيح)ويڪر;
		log_debug("%s is 2**%d\n", cell->name, ويڪر);
		أرجع لاڳت;
	} وإلا إذا (cell->type == ID($sop)) {
		صحيح ويڪر = cell->getParam(ID::ويڪر_2).as_int();
		صحيح اونهائي = cell->getParam(ID::اونهائي_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, ويڪر, اونهائي);
		أرجع (2 * ويڪر + 1) * اونهائي;
	} وإلا إذا (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		أرجع 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_سائيز(cell->parameters));
	أرجع 1;
}
