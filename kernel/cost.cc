#ضمن "kernel/cost.h"
#ضمن "kernel/macc.h"

USING_YOSYS_NAMESPACE

بنانشانی صحيح CellCosts::get(RTLIL::Module *mod)
{
	إذا (mod_لاگت_cache.گنتی(mod->name))
		أرجع mod_لاگت_cache.at(mod->name);

	بنانشانی صحيح module_لاگت = 1;
	لكل (خودکار c : mod->cells()) {
		بنانشانی صحيح new_لاگت = module_لاگت + get(c);
		module_لاگت = new_لاگت >= module_لاگت ? new_لاگت : ودھ_توں_ودھ_عدد;
	}

	mod_لاگت_cache[mod->name] = module_لاگت;
	أرجع module_لاگت;
}

ساكن بنانشانی صحيح y_coef(RTLIL::IdString type)
{
	إذا (
	  // equality
	  type.وچ(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.وچ(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.وچ(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		أرجع 1;
	} وإلا إذا (type == ID($neg)) {
		أرجع 4;
	} وإلا إذا (type == ID($demux)) {
		أرجع 2;
	} وإلا إذا (type == ID($fa)) {
		أرجع 5;
	} وإلا إذا (type.وچ(ID($جوڑو), ID($sub), ID($alu))) {
		// multi-bit adders
		أرجع 8;
	} وإلا إذا (type.وچ(ID($shl), ID($sshl))) {
		// left shift
		أرجع 10;
	}
	أرجع 0;
}

ساكن بنانشانی صحيح max_inp_coef(RTLIL::IdString type)
{
	إذا (
	  // binop reduce
	  type.وچ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.وچ(ID($logic_not), ID($pmux), ID($bmux))) {
		أرجع 1;
	} وإلا إذا (
	  // equality
	  type.وچ(ID($eq), ID($ne)) ||
	  // logic
	  type.وچ(ID($logic_and), ID($logic_or))) {
		أرجع 2;
	} وإلا إذا (type == ID($lcu)) {
		أرجع 5;
	} وإلا إذا (type.وچ(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		أرجع 7;
	}
	أرجع 0;
}

ساكن بنانشانی صحيح جوڑ_coef(RTLIL::IdString type)
{
	إذا (type.وچ(ID($shr), ID($sshr))) {
		// right shift
		أرجع 4;
	} وإلا إذا (type.وچ(ID($کھسکاؤ), ID($shiftx))) {
		//کھسکاؤ
		أرجع 8;
	}
	أرجع 0;
}

ساكن بنانشانی صحيح is_div_mod(RTLIL::IdString type)
{
	أرجع (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساكن منطقي is_free(RTLIL::IdString type)
{
	أرجع (
	  //ٹیگ_سوچی
	  type.وچ(ID($overwrite_ٹیگ), ID($سیٹ_ٹیگ), ID($original_ٹیگ), ID($get_ٹیگ)) ||
	  // formal
	  type.وچ(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($جیوندا), ID($cover), ID($fair)) ||
	  type.وچ(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.وچ(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.وچ(ID($concat), ID($ٹکڑا), ID($pos)) ||
	  // specify
	  type.وچ(ID($specrule), ID($specify2), ID($specify3)));
}

بنانشانی صحيح max_inp_چوڑائی(RTLIL::Cell *cell)
{
	بنانشانی صحيح max = 0;
	RTLIL::IdString داخلہ_چوڑائی_params[] = {
	  ID::چوڑائی_2,
	  ID::A_چوڑائی,
	  ID::B_چوڑائی,
	  ID::S_چوڑائی,
	};

	إذا (cell->type == ID($bmux))
		أرجع cell->getParam(ID::چوڑائی_2).as_int() << cell->getParam(ID::S_چوڑائی).as_int();

	لكل (RTLIL::IdString param : داخلہ_چوڑائی_params)
		إذا (cell->hasParam(param))
			max = std::max(max, (بنانشانی صحيح)cell->getParam(param).as_int());
	أرجع max;
}

بنانشانی صحيح پورٹ_چوڑائی_جوڑ(RTLIL::Cell *cell)
{
	بنانشانی صحيح جوڑ = 0;
	RTLIL::IdString پورٹ_چوڑائی_params[] = {
	  ID::چوڑائی_2, ID::A_چوڑائی, ID::B_چوڑائی, ID::S_چوڑائی, ID::Y_چوڑائی,
	};

	لكل (خودکار param : پورٹ_چوڑائی_params)
		إذا (cell->hasParam(param))
			جوڑ += cell->getParam(param).as_int();

	أرجع جوڑ;
}

بنانشانی صحيح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	إذا (cmos_gate_لاگت().گنتی(cell->type))
		أرجع 1;

	إذا (design_ && design_->module(cell->type) && cell->parameters.خالی_تھاواں()) {
		log_debug("%s is a module, recurse\n", cell->name);
		أرجع get(design_->module(cell->type));
	} وإلا إذا (cell->is_builtin_ff()) {
		log_assert(cell->has_پورٹ(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		أرجع cell->getParam(ID::چوڑائی_2).as_int();
	} وإلا إذا (cell->type.وچ(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		أرجع cell->getParam(ID::چوڑائی_2).as_int() * cell->getParam(ID::ناپ_2).as_int();
	} وإلا إذا (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_چوڑائی) || cell->hasParam(ID::چوڑائی_2)) && "Unknown width");
		خودکار param = cell->hasParam(ID::Y_چوڑائی) ? ID::Y_چوڑائی : ID::چوڑائی_2;
		صحيح چوڑائی = cell->getParam(param).as_int();
		إذا (cell->type == ID($demux))
			چوڑائی <<= cell->getParam(ID::S_چوڑائی).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, چوڑائی, y_coef(cell->type));
		أرجع چوڑائی * y_coef(cell->type);
	} وإلا إذا (جوڑ_coef(cell->type)) {
		// linear with sum of port widths
		بنانشانی صحيح جوڑ = پورٹ_چوڑائی_جوڑ(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, جوڑ, جوڑ_coef(cell->type));
		أرجع جوڑ * جوڑ_coef(cell->type);
	} وإلا إذا (max_inp_coef(cell->type)) {
		// linear with largest input width
		بنانشانی صحيح max = max_inp_چوڑائی(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		أرجع max * max_inp_coef(cell->type);
	} وإلا إذا (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		بنانشانی صحيح جوڑ = پورٹ_چوڑائی_جوڑ(cell);
		بنانشانی صحيح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, جوڑ * جوڑ);
		أرجع coef * جوڑ * جوڑ;
	} وإلا إذا (cell->type.وچ(ID($macc), ID($macc_v2))) {
		// quadratic per term
		بنانشانی صحيح لاگت_جوڑ = 0;
		Macc macc;
		macc.from_cell(cell);
		بنانشانی صحيح y_چوڑائی = cell->getParam(ID::Y_چوڑائی).as_int();
		لكل (خودکار &رکن: macc.terms) {
			إذا (رکن.وچ_b.خالی_تھاواں()) {
				// neglect addends
				استمر;
			}
			بنانشانی a_چوڑائی = رکن.وچ_a.ناپ(), b_چوڑائی = رکن.وچ_b.ناپ();
			بنانشانی صحيح جوڑ = a_چوڑائی + b_چوڑائی + std::min(y_چوڑائی, a_چوڑائی + b_چوڑائی);
			لاگت_جوڑ += 3 * جوڑ * جوڑ;
		}
		أرجع لاگت_جوڑ;
	} وإلا إذا (cell->type == ID($lut)) {
		صحيح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		بنانشانی صحيح لاگت = 1U << (بنانشانی صحيح)چوڑائی;
		log_debug("%s is 2**%d\n", cell->name, چوڑائی);
		أرجع لاگت;
	} وإلا إذا (cell->type == ID($sop)) {
		صحيح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		صحيح ڈونگھائی = cell->getParam(ID::ڈونگھائی_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, چوڑائی, ڈونگھائی);
		أرجع (2 * چوڑائی + 1) * ڈونگھائی;
	} وإلا إذا (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		أرجع 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_ناپ(cell->parameters));
	أرجع 1;
}
