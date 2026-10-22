#अन्तर्भाव "kernel/cost.h"
#अन्तर्भाव "kernel/macc.h"

USING_YOSYS_NAMESPACE

अचिह्नित पूर्णाङ्क CellCosts::get(RTLIL::Module *mod)
{
	यदि (mod_cost_cache_.गणनम्(mod->name))
		प्रत्यागम mod_cost_cache_.at(mod->name);

	अचिह्नित पूर्णाङ्क module_cost = 1;
	प्रति (स्वचालित c : mod->cells()) {
		अचिह्नित पूर्णाङ्क new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : अधिकतम_पूर्णाङ्कः;
	}

	mod_cost_cache_[mod->name] = module_cost;
	प्रत्यागम module_cost;
}

स्थिर अचिह्नित पूर्णाङ्क y_coef(RTLIL::IdString type)
{
	यदि (
	  // equality
	  type.अन्तर्(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.अन्तर्(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.अन्तर्(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		प्रत्यागम 1;
	} अन्यथा यदि (type == ID($neg)) {
		प्रत्यागम 4;
	} अन्यथा यदि (type == ID($demux)) {
		प्रत्यागम 2;
	} अन्यथा यदि (type == ID($fa)) {
		प्रत्यागम 5;
	} अन्यथा यदि (type.अन्तर्(ID($सङ्कलय), ID($sub), ID($alu))) {
		// multi-bit adders
		प्रत्यागम 8;
	} अन्यथा यदि (type.अन्तर्(ID($shl), ID($sshl))) {
		// left shift
		प्रत्यागम 10;
	}
	प्रत्यागम 0;
}

स्थिर अचिह्नित पूर्णाङ्क max_inp_coef(RTLIL::IdString type)
{
	यदि (
	  // binop reduce
	  type.अन्तर्(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.अन्तर्(ID($logic_not), ID($pmux), ID($bmux))) {
		प्रत्यागम 1;
	} अन्यथा यदि (
	  // equality
	  type.अन्तर्(ID($eq), ID($ne)) ||
	  // logic
	  type.अन्तर्(ID($logic_and), ID($logic_or))) {
		प्रत्यागम 2;
	} अन्यथा यदि (type == ID($lcu)) {
		प्रत्यागम 5;
	} अन्यथा यदि (type.अन्तर्(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		प्रत्यागम 7;
	}
	प्रत्यागम 0;
}

स्थिर अचिह्नित पूर्णाङ्क योग_coef(RTLIL::IdString type)
{
	यदि (type.अन्तर्(ID($shr), ID($sshr))) {
		// right shift
		प्रत्यागम 4;
	} अन्यथा यदि (type.अन्तर्(ID($सरणम्), ID($shiftx))) {
		//सरणम्
		प्रत्यागम 8;
	}
	प्रत्यागम 0;
}

स्थिर अचिह्नित पूर्णाङ्क is_div_mod(RTLIL::IdString type)
{
	प्रत्यागम (type == ID($भाजय) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

स्थिर तार्किक is_free(RTLIL::IdString type)
{
	प्रत्यागम (
	  //चिह्नकानि
	  type.अन्तर्(ID($overwrite_चिह्नकम्), ID($समुच्चयः_चिह्नकम्), ID($original_चिह्नकम्), ID($get_चिह्नकम्)) ||
	  // formal
	  type.अन्तर्(ID($check), ID($equiv), ID($initstate), ID($प्रतिज्ञा), ID($assume), ID($जीवितम्), ID($cover), ID($fair)) ||
	  type.अन्तर्(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.अन्तर्(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.अन्तर्(ID($concat), ID($खण्डिका), ID($pos)) ||
	  // specify
	  type.अन्तर्(ID($specrule), ID($specify2), ID($specify3)));
}

अचिह्नित पूर्णाङ्क max_inp_विस्तारः(RTLIL::Cell *cell)
{
	अचिह्नित पूर्णाङ्क max = 0;
	RTLIL::IdString निवेशः_विस्तारः_params[] = {
	  ID::विस्तारः_2,
	  ID::A_विस्तारः,
	  ID::B_विस्तारः,
	  ID::S_विस्तारः,
	};

	यदि (cell->type == ID($bmux))
		प्रत्यागम cell->getParam(ID::विस्तारः_2).as_int() << cell->getParam(ID::S_विस्तारः).as_int();

	प्रति (RTLIL::IdString param : निवेशः_विस्तारः_params)
		यदि (cell->hasParam(param))
			max = std::max(max, (अचिह्नित पूर्णाङ्क)cell->getParam(param).as_int());
	प्रत्यागम max;
}

अचिह्नित पूर्णाङ्क पत्तनम्_विस्तारः_योग(RTLIL::Cell *cell)
{
	अचिह्नित पूर्णाङ्क योग = 0;
	RTLIL::IdString पत्तनम्_विस्तारः_params[] = {
	  ID::विस्तारः_2, ID::A_विस्तारः, ID::B_विस्तारः, ID::S_विस्तारः, ID::Y_विस्तारः,
	};

	प्रति (स्वचालित param : पत्तनम्_विस्तारः_params)
		यदि (cell->hasParam(param))
			योग += cell->getParam(param).as_int();

	प्रत्यागम योग;
}

अचिह्नित पूर्णाङ्क CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	यदि (cmos_gate_cost().गणनम्(cell->type))
		प्रत्यागम 1;

	यदि (design_ && design_->module(cell->type) && cell->parameters.रिक्तानि()) {
		log_debug("%s is a module, recurse\n", cell->name);
		प्रत्यागम get(design_->module(cell->type));
	} अन्यथा यदि (cell->is_builtin_ff()) {
		log_assert(cell->has_पत्तनम्(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		प्रत्यागम cell->getParam(ID::विस्तारः_2).as_int();
	} अन्यथा यदि (cell->type.अन्तर्(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		प्रत्यागम cell->getParam(ID::विस्तारः_2).as_int() * cell->getParam(ID::आकार_2).as_int();
	} अन्यथा यदि (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_विस्तारः) || cell->hasParam(ID::विस्तारः_2)) && "Unknown width");
		स्वचालित param = cell->hasParam(ID::Y_विस्तारः) ? ID::Y_विस्तारः : ID::विस्तारः_2;
		पूर्णाङ्क विस्तारः = cell->getParam(param).as_int();
		यदि (cell->type == ID($demux))
			विस्तारः <<= cell->getParam(ID::S_विस्तारः).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, विस्तारः, y_coef(cell->type));
		प्रत्यागम विस्तारः * y_coef(cell->type);
	} अन्यथा यदि (योग_coef(cell->type)) {
		// linear with sum of port widths
		अचिह्नित पूर्णाङ्क योग = पत्तनम्_विस्तारः_योग(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, योग, योग_coef(cell->type));
		प्रत्यागम योग * योग_coef(cell->type);
	} अन्यथा यदि (max_inp_coef(cell->type)) {
		// linear with largest input width
		अचिह्नित पूर्णाङ्क max = max_inp_विस्तारः(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		प्रत्यागम max * max_inp_coef(cell->type);
	} अन्यथा यदि (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		अचिह्नित पूर्णाङ्क योग = पत्तनम्_विस्तारः_योग(cell);
		अचिह्नित पूर्णाङ्क coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, योग * योग);
		प्रत्यागम coef * योग * योग;
	} अन्यथा यदि (cell->type.अन्तर्(ID($macc), ID($macc_v2))) {
		// quadratic per term
		अचिह्नित पूर्णाङ्क cost_योग = 0;
		Macc macc;
		macc.from_cell(cell);
		अचिह्नित पूर्णाङ्क y_विस्तारः = cell->getParam(ID::Y_विस्तारः).as_int();
		प्रति (स्वचालित &पदांशः: macc.terms) {
			यदि (पदांशः.अन्तर्_b.रिक्तानि()) {
				// neglect addends
				अनुवर्त;
			}
			अचिह्नित a_विस्तारः = पदांशः.अन्तर्_a.आकार(), b_विस्तारः = पदांशः.अन्तर्_b.आकार();
			अचिह्नित पूर्णाङ्क योग = a_विस्तारः + b_विस्तारः + std::min(y_विस्तारः, a_विस्तारः + b_विस्तारः);
			cost_योग += 3 * योग * योग;
		}
		प्रत्यागम cost_योग;
	} अन्यथा यदि (cell->type == ID($lut)) {
		पूर्णाङ्क विस्तारः = cell->getParam(ID::विस्तारः_2).as_int();
		अचिह्नित पूर्णाङ्क cost = 1U << (अचिह्नित पूर्णाङ्क)विस्तारः;
		log_debug("%s is 2**%d\n", cell->name, विस्तारः);
		प्रत्यागम cost;
	} अन्यथा यदि (cell->type == ID($sop)) {
		पूर्णाङ्क विस्तारः = cell->getParam(ID::विस्तारः_2).as_int();
		पूर्णाङ्क गभीरता = cell->getParam(ID::गभीरता_2).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, विस्तारः, गभीरता);
		प्रत्यागम (2 * विस्तारः + 1) * गभीरता;
	} अन्यथा यदि (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		प्रत्यागम 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_आकार(cell->parameters));
	प्रत्यागम 1;
}
