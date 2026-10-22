#شامل "kernel/cost.h"
#شامل "kernel/macc.h"

USING_YOSYS_NAMESPACE

غیرعلامتی صحیح CellCosts::get(RTLIL::Module *mod)
{
	اگر (mod_لاگت_cache.گنتی(mod->name))
		واپس mod_لاگت_cache.at(mod->name);

	غیرعلامتی صحیح module_لاگت = 1;
	برائے (خودکار c : mod->cells()) {
		غیرعلامتی صحیح new_لاگت = module_لاگت + get(c);
		module_لاگت = new_لاگت >= module_لاگت ? new_لاگت : زیادہ_سے_زیادہ_عدد;
	}

	mod_لاگت_cache[mod->name] = module_لاگت;
	واپس module_لاگت;
}

ساکن غیرعلامتی صحیح y_coef(RTLIL::IdString type)
{
	اگر (
	  // equality
	  type.اندر(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.اندر(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.اندر(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		واپس 1;
	} ورنہ اگر (type == ID($neg)) {
		واپس 4;
	} ورنہ اگر (type == ID($demux)) {
		واپس 2;
	} ورنہ اگر (type == ID($fa)) {
		واپس 5;
	} ورنہ اگر (type.اندر(ID($جوڑو), ID($sub), ID($alu))) {
		// multi-bit adders
		واپس 8;
	} ورنہ اگر (type.اندر(ID($shl), ID($sshl))) {
		// left shift
		واپس 10;
	}
	واپس 0;
}

ساکن غیرعلامتی صحیح max_inp_coef(RTLIL::IdString type)
{
	اگر (
	  // binop reduce
	  type.اندر(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.اندر(ID($logic_not), ID($pmux), ID($bmux))) {
		واپس 1;
	} ورنہ اگر (
	  // equality
	  type.اندر(ID($eq), ID($ne)) ||
	  // logic
	  type.اندر(ID($logic_and), ID($logic_or))) {
		واپس 2;
	} ورنہ اگر (type == ID($lcu)) {
		واپس 5;
	} ورنہ اگر (type.اندر(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		واپس 7;
	}
	واپس 0;
}

ساکن غیرعلامتی صحیح مجموع_coef(RTLIL::IdString type)
{
	اگر (type.اندر(ID($shr), ID($sshr))) {
		// right shift
		واپس 4;
	} ورنہ اگر (type.اندر(ID($سرکاؤ), ID($shiftx))) {
		//سرکاؤ
		واپس 8;
	}
	واپس 0;
}

ساکن غیرعلامتی صحیح is_div_mod(RTLIL::IdString type)
{
	واپس (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساکن منطقی is_free(RTLIL::IdString type)
{
	واپس (
	  //لیبل_فہرست
	  type.اندر(ID($overwrite_لیبل), ID($سیٹ_لیبل), ID($original_لیبل), ID($get_لیبل)) ||
	  // formal
	  type.اندر(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($زندہ), ID($cover), ID($fair)) ||
	  type.اندر(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.اندر(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.اندر(ID($concat), ID($قاش), ID($pos)) ||
	  // specify
	  type.اندر(ID($specrule), ID($specify2), ID($specify3)));
}

غیرعلامتی صحیح max_inp_چوڑائی(RTLIL::Cell *cell)
{
	غیرعلامتی صحیح max = 0;
	RTLIL::IdString داخلہ_چوڑائی_params[] = {
	  ID::چوڑائی_2,
	  ID::A_چوڑائی,
	  ID::B_چوڑائی,
	  ID::S_چوڑائی,
	};

	اگر (cell->type == ID($bmux))
		واپس cell->getParam(ID::چوڑائی_2).as_int() << cell->getParam(ID::S_چوڑائی).as_int();

	برائے (RTLIL::IdString param : داخلہ_چوڑائی_params)
		اگر (cell->hasParam(param))
			max = std::max(max, (غیرعلامتی صحیح)cell->getParam(param).as_int());
	واپس max;
}

غیرعلامتی صحیح پورٹ_چوڑائی_مجموع(RTLIL::Cell *cell)
{
	غیرعلامتی صحیح مجموع = 0;
	RTLIL::IdString پورٹ_چوڑائی_params[] = {
	  ID::چوڑائی_2, ID::A_چوڑائی, ID::B_چوڑائی, ID::S_چوڑائی, ID::Y_چوڑائی,
	};

	برائے (خودکار param : پورٹ_چوڑائی_params)
		اگر (cell->hasParam(param))
			مجموع += cell->getParam(param).as_int();

	واپس مجموع;
}

غیرعلامتی صحیح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	اگر (cmos_gate_لاگت().گنتی(cell->type))
		واپس 1;

	اگر (design_ && design_->module(cell->type) && cell->parameters.خالی_خانے()) {
		log_debug("%s is a module, recurse\n", cell->name);
		واپس get(design_->module(cell->type));
	} ورنہ اگر (cell->is_builtin_ff()) {
		log_assert(cell->has_پورٹ(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		واپس cell->getParam(ID::چوڑائی_2).as_int();
	} ورنہ اگر (cell->type.اندر(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		واپس cell->getParam(ID::چوڑائی_2).as_int() * cell->getParam(ID::سائز_2).as_int();
	} ورنہ اگر (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_چوڑائی) || cell->hasParam(ID::چوڑائی_2)) && "Unknown width");
		خودکار param = cell->hasParam(ID::Y_چوڑائی) ? ID::Y_چوڑائی : ID::چوڑائی_2;
		صحیح چوڑائی = cell->getParam(param).as_int();
		اگر (cell->type == ID($demux))
			چوڑائی <<= cell->getParam(ID::S_چوڑائی).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, چوڑائی, y_coef(cell->type));
		واپس چوڑائی * y_coef(cell->type);
	} ورنہ اگر (مجموع_coef(cell->type)) {
		// linear with sum of port widths
		غیرعلامتی صحیح مجموع = پورٹ_چوڑائی_مجموع(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, مجموع, مجموع_coef(cell->type));
		واپس مجموع * مجموع_coef(cell->type);
	} ورنہ اگر (max_inp_coef(cell->type)) {
		// linear with largest input width
		غیرعلامتی صحیح max = max_inp_چوڑائی(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		واپس max * max_inp_coef(cell->type);
	} ورنہ اگر (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		غیرعلامتی صحیح مجموع = پورٹ_چوڑائی_مجموع(cell);
		غیرعلامتی صحیح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, مجموع * مجموع);
		واپس coef * مجموع * مجموع;
	} ورنہ اگر (cell->type.اندر(ID($macc), ID($macc_v2))) {
		// quadratic per term
		غیرعلامتی صحیح لاگت_مجموع = 0;
		Macc macc;
		macc.from_cell(cell);
		غیرعلامتی صحیح y_چوڑائی = cell->getParam(ID::Y_چوڑائی).as_int();
		برائے (خودکار &رکن: macc.terms) {
			اگر (رکن.اندر_b.خالی_خانے()) {
				// neglect addends
				جاری;
			}
			غیرعلامتی a_چوڑائی = رکن.اندر_a.سائز(), b_چوڑائی = رکن.اندر_b.سائز();
			غیرعلامتی صحیح مجموع = a_چوڑائی + b_چوڑائی + std::min(y_چوڑائی, a_چوڑائی + b_چوڑائی);
			لاگت_مجموع += 3 * مجموع * مجموع;
		}
		واپس لاگت_مجموع;
	} ورنہ اگر (cell->type == ID($lut)) {
		صحیح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		غیرعلامتی صحیح لاگت = 1U << (غیرعلامتی صحیح)چوڑائی;
		log_debug("%s is 2**%d\n", cell->name, چوڑائی);
		واپس لاگت;
	} ورنہ اگر (cell->type == ID($sop)) {
		صحیح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		صحیح گہرائی = cell->getParam(ID::گہرائی_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, چوڑائی, گہرائی);
		واپس (2 * چوڑائی + 1) * گہرائی;
	} ورنہ اگر (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		واپس 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_سائز(cell->parameters));
	واپس 1;
}
