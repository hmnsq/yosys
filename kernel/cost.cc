#अन्तर्भाव "kernel/cost.h"
#अन्तर्भाव "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned पूर्णाङ्क CellCosts::get(RTLIL::Module *mod)
{
	सचे (mod_cost_cache_.गणनम्(mod->name))
		पटिनिवत्त mod_cost_cache_.at(mod->name);

	unsigned पूर्णाङ्क module_cost = 1;
	अत्थाय (auto c : mod->cells()) {
		unsigned पूर्णाङ्क new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	पटिनिवत्त module_cost;
}

स्थिर unsigned पूर्णाङ्क y_coef(RTLIL::IdString type)
{
	सचे (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		पटिनिवत्त 1;
	} अञ्ञथा सचे (type == ID($neg)) {
		पटिनिवत्त 4;
	} अञ्ञथा सचे (type == ID($demux)) {
		पटिनिवत्त 2;
	} अञ्ञथा सचे (type == ID($fa)) {
		पटिनिवत्त 5;
	} अञ्ञथा सचे (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		पटिनिवत्त 8;
	} अञ्ञथा सचे (type.in(ID($shl), ID($sshl))) {
		// left shift
		पटिनिवत्त 10;
	}
	पटिनिवत्त 0;
}

स्थिर unsigned पूर्णाङ्क max_inp_coef(RTLIL::IdString type)
{
	सचे (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		पटिनिवत्त 1;
	} अञ्ञथा सचे (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		पटिनिवत्त 2;
	} अञ्ञथा सचे (type == ID($lcu)) {
		पटिनिवत्त 5;
	} अञ्ञथा सचे (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		पटिनिवत्त 7;
	}
	पटिनिवत्त 0;
}

स्थिर unsigned पूर्णाङ्क sum_coef(RTLIL::IdString type)
{
	सचे (type.in(ID($shr), ID($sshr))) {
		// right shift
		पटिनिवत्त 4;
	} अञ्ञथा सचे (type.in(ID($shift), ID($shiftx))) {
		// shift
		पटिनिवत्त 8;
	}
	पटिनिवत्त 0;
}

स्थिर unsigned पूर्णाङ्क is_div_mod(RTLIL::IdString type)
{
	पटिनिवत्त (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

स्थिर तार्किक is_free(RTLIL::IdString type)
{
	पटिनिवत्त (
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

unsigned पूर्णाङ्क max_inp_width(RTLIL::Cell *cell)
{
	unsigned पूर्णाङ्क max = 0;
	RTLIL::IdString निवेशः_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	सचे (cell->type == ID($bmux))
		पटिनिवत्त cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	अत्थाय (RTLIL::IdString param : निवेशः_width_params)
		सचे (cell->hasParam(param))
			max = std::max(max, (unsigned पूर्णाङ्क)cell->getParam(param).as_int());
	पटिनिवत्त max;
}

unsigned पूर्णाङ्क port_width_sum(RTLIL::Cell *cell)
{
	unsigned पूर्णाङ्क sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	अत्थाय (auto param : port_width_params)
		सचे (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	पटिनिवत्त sum;
}

unsigned पूर्णाङ्क CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	सचे (cmos_gate_cost().गणनम्(cell->type))
		पटिनिवत्त 1;

	सचे (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		पटिनिवत्त get(design_->module(cell->type));
	} अञ्ञथा सचे (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		पटिनिवत्त cell->getParam(ID::WIDTH).as_int();
	} अञ्ञथा सचे (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		पटिनिवत्त cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} अञ्ञथा सचे (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		पूर्णाङ्क width = cell->getParam(param).as_int();
		सचे (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		पटिनिवत्त width * y_coef(cell->type);
	} अञ्ञथा सचे (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned पूर्णाङ्क sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		पटिनिवत्त sum * sum_coef(cell->type);
	} अञ्ञथा सचे (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned पूर्णाङ्क max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		पटिनिवत्त max * max_inp_coef(cell->type);
	} अञ्ञथा सचे (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned पूर्णाङ्क sum = port_width_sum(cell);
		unsigned पूर्णाङ्क coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		पटिनिवत्त coef * sum * sum;
	} अञ्ञथा सचे (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned पूर्णाङ्क cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned पूर्णाङ्क y_width = cell->getParam(ID::Y_WIDTH).as_int();
		अत्थाय (auto &term: macc.terms) {
			सचे (term.in_b.empty()) {
				// neglect addends
				अनुवर्त;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned पूर्णाङ्क sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		पटिनिवत्त cost_sum;
	} अञ्ञथा सचे (cell->type == ID($lut)) {
		पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();
		unsigned पूर्णाङ्क cost = 1U << (unsigned पूर्णाङ्क)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		पटिनिवत्त cost;
	} अञ्ञथा सचे (cell->type == ID($sop)) {
		पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();
		पूर्णाङ्क depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		पटिनिवत्त (2 * width + 1) * depth;
	} अञ्ञथा सचे (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		पटिनिवत्त 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	पटिनिवत्त 1;
}
