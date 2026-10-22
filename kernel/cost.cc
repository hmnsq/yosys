#समावेश "kernel/cost.h"
#समावेश "kernel/macc.h"

USING_YOSYS_NAMESPACE

अचिन्हित पूर्णांक CellCosts::लो(RTLIL::Module *mod)
{
	अगर (mod_cost_cache_.गिनती(mod->name))
		वापस mod_cost_cache_.at(mod->name);

	अचिन्हित पूर्णांक module_cost = 1;
	क्रम (स्वतः c : mod->cells()) {
		अचिन्हित पूर्णांक new_cost = module_cost + लो(c);
		module_cost = new_cost >= module_cost ? new_cost : पूर्णांक_उच्च;
	}

	mod_cost_cache_[mod->name] = module_cost;
	वापस module_cost;
}

अटल अचिन्हित पूर्णांक y_coef(RTLIL::IdString type)
{
	अगर (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		वापस 1;
	} अन्यथा अगर (type == ID($neg)) {
		वापस 4;
	} अन्यथा अगर (type == ID($demux)) {
		वापस 2;
	} अन्यथा अगर (type == ID($fa)) {
		वापस 5;
	} अन्यथा अगर (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		वापस 8;
	} अन्यथा अगर (type.in(ID($shl), ID($sshl))) {
		// left shift
		वापस 10;
	}
	वापस 0;
}

अटल अचिन्हित पूर्णांक max_inp_coef(RTLIL::IdString type)
{
	अगर (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		वापस 1;
	} अन्यथा अगर (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		वापस 2;
	} अन्यथा अगर (type == ID($lcu)) {
		वापस 5;
	} अन्यथा अगर (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		वापस 7;
	}
	वापस 0;
}

अटल अचिन्हित पूर्णांक योग_coef(RTLIL::IdString type)
{
	अगर (type.in(ID($shr), ID($sshr))) {
		// right shift
		वापस 4;
	} अन्यथा अगर (type.in(ID($shift), ID($shiftx))) {
		// shift
		वापस 8;
	}
	वापस 0;
}

अटल अचिन्हित पूर्णांक is_div_mod(RTLIL::IdString type)
{
	वापस (type == ID($भाग) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

अटल bool is_free(RTLIL::IdString type)
{
	वापस (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($निश्चित), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

अचिन्हित पूर्णांक max_inp_width(RTLIL::Cell *cell)
{
	अचिन्हित पूर्णांक max = 0;
	RTLIL::IdString निवेश_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	अगर (cell->type == ID($bmux))
		वापस cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	क्रम (RTLIL::IdString param : निवेश_width_params)
		अगर (cell->hasParam(param))
			max = std::max(max, (अचिन्हित पूर्णांक)cell->getParam(param).as_int());
	वापस max;
}

अचिन्हित पूर्णांक port_width_योग(RTLIL::Cell *cell)
{
	अचिन्हित पूर्णांक योग = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	क्रम (स्वतः param : port_width_params)
		अगर (cell->hasParam(param))
			योग += cell->getParam(param).as_int();

	वापस योग;
}

अचिन्हित पूर्णांक CellCosts::लो(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	अगर (cmos_gate_cost().गिनती(cell->type))
		वापस 1;

	अगर (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		वापस लो(design_->module(cell->type));
	} अन्यथा अगर (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		वापस cell->getParam(ID::WIDTH).as_int();
	} अन्यथा अगर (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		वापस cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::आकार_2).as_int();
	} अन्यथा अगर (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		स्वतः param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		पूर्णांक width = cell->getParam(param).as_int();
		अगर (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		वापस width * y_coef(cell->type);
	} अन्यथा अगर (योग_coef(cell->type)) {
		// linear with sum of port widths
		अचिन्हित पूर्णांक योग = port_width_योग(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, योग, योग_coef(cell->type));
		वापस योग * योग_coef(cell->type);
	} अन्यथा अगर (max_inp_coef(cell->type)) {
		// linear with largest input width
		अचिन्हित पूर्णांक max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		वापस max * max_inp_coef(cell->type);
	} अन्यथा अगर (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		अचिन्हित पूर्णांक योग = port_width_योग(cell);
		अचिन्हित पूर्णांक coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, योग * योग);
		वापस coef * योग * योग;
	} अन्यथा अगर (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		अचिन्हित पूर्णांक cost_योग = 0;
		Macc macc;
		macc.from_cell(cell);
		अचिन्हित पूर्णांक y_width = cell->getParam(ID::Y_WIDTH).as_int();
		क्रम (स्वतः &term: macc.terms) {
			अगर (term.in_b.empty()) {
				// neglect addends
				जारी;
			}
			अचिन्हित a_width = term.in_a.आकार(), b_width = term.in_b.आकार();
			अचिन्हित पूर्णांक योग = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_योग += 3 * योग * योग;
		}
		वापस cost_योग;
	} अन्यथा अगर (cell->type == ID($lut)) {
		पूर्णांक width = cell->getParam(ID::WIDTH).as_int();
		अचिन्हित पूर्णांक cost = 1U << (अचिन्हित पूर्णांक)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		वापस cost;
	} अन्यथा अगर (cell->type == ID($sop)) {
		पूर्णांक width = cell->getParam(ID::WIDTH).as_int();
		पूर्णांक depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		वापस (2 * width + 1) * depth;
	} अन्यथा अगर (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		वापस 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_आकार(cell->parameters));
	वापस 1;
}
