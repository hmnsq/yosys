#अन्तर्भाव "kernel/cost.h"
#अन्तर्भाव "kernel/macc.h"

USING_YOSYS_NAMESPACE

अचिन्हित पूर्णाङ्क CellCosts::get(RTLIL::Module *mod)
{
	यदि (mod_cost_cache_.गन्ती(mod->name))
		फर्काऊ mod_cost_cache_.at(mod->name);

	अचिन्हित पूर्णाङ्क module_cost = 1;
	लागि (स्वतः c : mod->cells()) {
		अचिन्हित पूर्णाङ्क new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : पूर्णांक_उच्च;
	}

	mod_cost_cache_[mod->name] = module_cost;
	फर्काऊ module_cost;
}

स्थिर अचिन्हित पूर्णाङ्क y_coef(RTLIL::IdString type)
{
	यदि (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		फर्काऊ 1;
	} अन्यथा यदि (type == ID($neg)) {
		फर्काऊ 4;
	} अन्यथा यदि (type == ID($demux)) {
		फर्काऊ 2;
	} अन्यथा यदि (type == ID($fa)) {
		फर्काऊ 5;
	} अन्यथा यदि (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		फर्काऊ 8;
	} अन्यथा यदि (type.in(ID($shl), ID($sshl))) {
		// left shift
		फर्काऊ 10;
	}
	फर्काऊ 0;
}

स्थिर अचिन्हित पूर्णाङ्क max_inp_coef(RTLIL::IdString type)
{
	यदि (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		फर्काऊ 1;
	} अन्यथा यदि (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		फर्काऊ 2;
	} अन्यथा यदि (type == ID($lcu)) {
		फर्काऊ 5;
	} अन्यथा यदि (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		फर्काऊ 7;
	}
	फर्काऊ 0;
}

स्थिर अचिन्हित पूर्णाङ्क sum_coef(RTLIL::IdString type)
{
	यदि (type.in(ID($shr), ID($sshr))) {
		// right shift
		फर्काऊ 4;
	} अन्यथा यदि (type.in(ID($shift), ID($shiftx))) {
		// shift
		फर्काऊ 8;
	}
	फर्काऊ 0;
}

स्थिर अचिन्हित पूर्णाङ्क is_div_mod(RTLIL::IdString type)
{
	फर्काऊ (type == ID($भाग) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

स्थिर तार्किक is_free(RTLIL::IdString type)
{
	फर्काऊ (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($प्रतिज्ञा), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

अचिन्हित पूर्णाङ्क max_inp_width(RTLIL::Cell *cell)
{
	अचिन्हित पूर्णाङ्क max = 0;
	RTLIL::IdString आगत_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	यदि (cell->type == ID($bmux))
		फर्काऊ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	लागि (RTLIL::IdString param : आगत_width_params)
		यदि (cell->hasParam(param))
			max = std::max(max, (अचिन्हित पूर्णाङ्क)cell->getParam(param).as_int());
	फर्काऊ max;
}

अचिन्हित पूर्णाङ्क port_width_sum(RTLIL::Cell *cell)
{
	अचिन्हित पूर्णाङ्क sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	लागि (स्वतः param : port_width_params)
		यदि (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	फर्काऊ sum;
}

अचिन्हित पूर्णाङ्क CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	यदि (cmos_gate_cost().गन्ती(cell->type))
		फर्काऊ 1;

	यदि (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		फर्काऊ get(design_->module(cell->type));
	} अन्यथा यदि (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		फर्काऊ cell->getParam(ID::WIDTH).as_int();
	} अन्यथा यदि (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		फर्काऊ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} अन्यथा यदि (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		स्वतः param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		पूर्णाङ्क width = cell->getParam(param).as_int();
		यदि (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		फर्काऊ width * y_coef(cell->type);
	} अन्यथा यदि (sum_coef(cell->type)) {
		// linear with sum of port widths
		अचिन्हित पूर्णाङ्क sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		फर्काऊ sum * sum_coef(cell->type);
	} अन्यथा यदि (max_inp_coef(cell->type)) {
		// linear with largest input width
		अचिन्हित पूर्णाङ्क max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		फर्काऊ max * max_inp_coef(cell->type);
	} अन्यथा यदि (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		अचिन्हित पूर्णाङ्क sum = port_width_sum(cell);
		अचिन्हित पूर्णाङ्क coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		फर्काऊ coef * sum * sum;
	} अन्यथा यदि (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		अचिन्हित पूर्णाङ्क cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		अचिन्हित पूर्णाङ्क y_width = cell->getParam(ID::Y_WIDTH).as_int();
		लागि (स्वतः &term: macc.terms) {
			यदि (term.in_b.empty()) {
				// neglect addends
				जारी;
			}
			अचिन्हित a_width = term.in_a.size(), b_width = term.in_b.size();
			अचिन्हित पूर्णाङ्क sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		फर्काऊ cost_sum;
	} अन्यथा यदि (cell->type == ID($lut)) {
		पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();
		अचिन्हित पूर्णाङ्क cost = 1U << (अचिन्हित पूर्णाङ्क)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		फर्काऊ cost;
	} अन्यथा यदि (cell->type == ID($sop)) {
		पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();
		पूर्णाङ्क depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		फर्काऊ (2 * width + 1) * depth;
	} अन्यथा यदि (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		फर्काऊ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	फर्काऊ 1;
}
