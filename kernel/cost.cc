#ضمن "kernel/cost.h"
#ضمن "kernel/macc.h"

USING_YOSYS_NAMESPACE

بیعلامت صحيح CellCosts::get(RTLIL::Module *mod)
{
	إذا (mod_هزینه_cache.شمار(mod->name))
		أرجع mod_هزینه_cache.at(mod->name);

	بیعلامت صحيح module_هزینه = 1;
	لكل (خودکار c : mod->cells()) {
		بیعلامت صحيح new_هزینه = module_هزینه + get(c);
		module_هزینه = new_هزینه >= module_هزینه ? new_هزینه : بیشترین_صحیح;
	}

	mod_هزینه_cache[mod->name] = module_هزینه;
	أرجع module_هزینه;
}

ساكن بیعلامت صحيح y_coef(RTLIL::IdString type)
{
	إذا (
	  // equality
	  type.درون(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.درون(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.درون(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		أرجع 1;
	} وإلا إذا (type == ID($neg)) {
		أرجع 4;
	} وإلا إذا (type == ID($demux)) {
		أرجع 2;
	} وإلا إذا (type == ID($fa)) {
		أرجع 5;
	} وإلا إذا (type.درون(ID($جمع_کن), ID($sub), ID($alu))) {
		// multi-bit adders
		أرجع 8;
	} وإلا إذا (type.درون(ID($shl), ID($sshl))) {
		// left shift
		أرجع 10;
	}
	أرجع 0;
}

ساكن بیعلامت صحيح max_inp_coef(RTLIL::IdString type)
{
	إذا (
	  // binop reduce
	  type.درون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.درون(ID($logic_not), ID($pmux), ID($bmux))) {
		أرجع 1;
	} وإلا إذا (
	  // equality
	  type.درون(ID($eq), ID($ne)) ||
	  // logic
	  type.درون(ID($logic_and), ID($logic_or))) {
		أرجع 2;
	} وإلا إذا (type == ID($lcu)) {
		أرجع 5;
	} وإلا إذا (type.درون(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		أرجع 7;
	}
	أرجع 0;
}

ساكن بیعلامت صحيح جمع_coef(RTLIL::IdString type)
{
	إذا (type.درون(ID($shr), ID($sshr))) {
		// right shift
		أرجع 4;
	} وإلا إذا (type.درون(ID($لغزش), ID($shiftx))) {
		//لغزش
		أرجع 8;
	}
	أرجع 0;
}

ساكن بیعلامت صحيح is_div_mod(RTLIL::IdString type)
{
	أرجع (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساكن منطقي is_free(RTLIL::IdString type)
{
	أرجع (
	  //برچسبها
	  type.درون(ID($overwrite_برچسب), ID($مجموعه_برچسب), ID($original_برچسب), ID($get_برچسب)) ||
	  // formal
	  type.درون(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($زنده), ID($cover), ID($fair)) ||
	  type.درون(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.درون(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.درون(ID($concat), ID($برش), ID($pos)) ||
	  // specify
	  type.درون(ID($specrule), ID($specify2), ID($specify3)));
}

بیعلامت صحيح max_inp_عرض(RTLIL::Cell *cell)
{
	بیعلامت صحيح max = 0;
	RTLIL::IdString درون_داد_عرض_params[] = {
	  ID::عرض_2,
	  ID::A_عرض,
	  ID::B_عرض,
	  ID::S_عرض,
	};

	إذا (cell->type == ID($bmux))
		أرجع cell->getParam(ID::عرض_2).as_int() << cell->getParam(ID::S_عرض).as_int();

	لكل (RTLIL::IdString param : درون_داد_عرض_params)
		إذا (cell->hasParam(param))
			max = std::max(max, (بیعلامت صحيح)cell->getParam(param).as_int());
	أرجع max;
}

بیعلامت صحيح پورت_عرض_جمع(RTLIL::Cell *cell)
{
	بیعلامت صحيح جمع = 0;
	RTLIL::IdString پورت_عرض_params[] = {
	  ID::عرض_2, ID::A_عرض, ID::B_عرض, ID::S_عرض, ID::Y_عرض,
	};

	لكل (خودکار param : پورت_عرض_params)
		إذا (cell->hasParam(param))
			جمع += cell->getParam(param).as_int();

	أرجع جمع;
}

بیعلامت صحيح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	إذا (cmos_gate_هزینه().شمار(cell->type))
		أرجع 1;

	إذا (design_ && design_->module(cell->type) && cell->parameters.خالی()) {
		log_debug("%s is a module, recurse\n", cell->name);
		أرجع get(design_->module(cell->type));
	} وإلا إذا (cell->is_builtin_ff()) {
		log_assert(cell->has_پورت(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		أرجع cell->getParam(ID::عرض_2).as_int();
	} وإلا إذا (cell->type.درون(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		أرجع cell->getParam(ID::عرض_2).as_int() * cell->getParam(ID::اندازه_2).as_int();
	} وإلا إذا (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_عرض) || cell->hasParam(ID::عرض_2)) && "Unknown width");
		خودکار param = cell->hasParam(ID::Y_عرض) ? ID::Y_عرض : ID::عرض_2;
		صحيح عرض = cell->getParam(param).as_int();
		إذا (cell->type == ID($demux))
			عرض <<= cell->getParam(ID::S_عرض).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, عرض, y_coef(cell->type));
		أرجع عرض * y_coef(cell->type);
	} وإلا إذا (جمع_coef(cell->type)) {
		// linear with sum of port widths
		بیعلامت صحيح جمع = پورت_عرض_جمع(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, جمع, جمع_coef(cell->type));
		أرجع جمع * جمع_coef(cell->type);
	} وإلا إذا (max_inp_coef(cell->type)) {
		// linear with largest input width
		بیعلامت صحيح max = max_inp_عرض(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		أرجع max * max_inp_coef(cell->type);
	} وإلا إذا (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		بیعلامت صحيح جمع = پورت_عرض_جمع(cell);
		بیعلامت صحيح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, جمع * جمع);
		أرجع coef * جمع * جمع;
	} وإلا إذا (cell->type.درون(ID($macc), ID($macc_v2))) {
		// quadratic per term
		بیعلامت صحيح هزینه_جمع = 0;
		Macc macc;
		macc.from_cell(cell);
		بیعلامت صحيح y_عرض = cell->getParam(ID::Y_عرض).as_int();
		لكل (خودکار &جمله: macc.terms) {
			إذا (جمله.درون_b.خالی()) {
				// neglect addends
				استمر;
			}
			بیعلامت a_عرض = جمله.درون_a.اندازه(), b_عرض = جمله.درون_b.اندازه();
			بیعلامت صحيح جمع = a_عرض + b_عرض + std::min(y_عرض, a_عرض + b_عرض);
			هزینه_جمع += 3 * جمع * جمع;
		}
		أرجع هزینه_جمع;
	} وإلا إذا (cell->type == ID($lut)) {
		صحيح عرض = cell->getParam(ID::عرض_2).as_int();
		بیعلامت صحيح هزینه = 1U << (بیعلامت صحيح)عرض;
		log_debug("%s is 2**%d\n", cell->name, عرض);
		أرجع هزینه;
	} وإلا إذا (cell->type == ID($sop)) {
		صحيح عرض = cell->getParam(ID::عرض_2).as_int();
		صحيح عمق = cell->getParam(ID::عمق_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, عرض, عمق);
		أرجع (2 * عرض + 1) * عمق;
	} وإلا إذا (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		أرجع 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_اندازه(cell->parameters));
	أرجع 1;
}
