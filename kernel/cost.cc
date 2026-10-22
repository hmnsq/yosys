#അന്തര്ഭാവ "kernel/cost.h"
#അന്തര്ഭാവ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned പൂർണ്ണസംഖ്യ CellCosts::get(RTLIL::Module *mod)
{
	എങ്കിൽ (mod_cost_cache_.എണ്ണം(mod->name))
		തിരികെ mod_cost_cache_.at(mod->name);

	unsigned പൂർണ്ണസംഖ്യ module_cost = 1;
	വേണ്ടി (auto c : mod->cells()) {
		unsigned പൂർണ്ണസംഖ്യ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	തിരികെ module_cost;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ y_coef(RTLIL::IdString type)
{
	എങ്കിൽ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		തിരികെ 1;
	} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($neg)) {
		തിരികെ 4;
	} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($demux)) {
		തിരികെ 2;
	} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($fa)) {
		തിരികെ 5;
	} അല്ലെങ്കിൽ എങ്കിൽ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		തിരികെ 8;
	} അല്ലെങ്കിൽ എങ്കിൽ (type.in(ID($shl), ID($sshl))) {
		// left shift
		തിരികെ 10;
	}
	തിരികെ 0;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ max_inp_coef(RTLIL::IdString type)
{
	എങ്കിൽ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		തിരികെ 1;
	} അല്ലെങ്കിൽ എങ്കിൽ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		തിരികെ 2;
	} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($lcu)) {
		തിരികെ 5;
	} അല്ലെങ്കിൽ എങ്കിൽ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		തിരികെ 7;
	}
	തിരികെ 0;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ sum_coef(RTLIL::IdString type)
{
	എങ്കിൽ (type.in(ID($shr), ID($sshr))) {
		// right shift
		തിരികെ 4;
	} അല്ലെങ്കിൽ എങ്കിൽ (type.in(ID($shift), ID($shiftx))) {
		// shift
		തിരികെ 8;
	}
	തിരികെ 0;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ is_div_mod(RTLIL::IdString type)
{
	തിരികെ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

സ്ഥിതം തർക്കം is_free(RTLIL::IdString type)
{
	തിരികെ (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($പ്രതിജ്ഞാ), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned പൂർണ്ണസംഖ്യ max_inp_width(RTLIL::Cell *cell)
{
	unsigned പൂർണ്ണസംഖ്യ max = 0;
	RTLIL::IdString നിവേശം_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	എങ്കിൽ (cell->type == ID($bmux))
		തിരികെ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	വേണ്ടി (RTLIL::IdString param : നിവേശം_width_params)
		എങ്കിൽ (cell->hasParam(param))
			max = std::max(max, (unsigned പൂർണ്ണസംഖ്യ)cell->getParam(param).as_int());
	തിരികെ max;
}

unsigned പൂർണ്ണസംഖ്യ port_width_sum(RTLIL::Cell *cell)
{
	unsigned പൂർണ്ണസംഖ്യ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	വേണ്ടി (auto param : port_width_params)
		എങ്കിൽ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	തിരികെ sum;
}

unsigned പൂർണ്ണസംഖ്യ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	എങ്കിൽ (cmos_gate_cost().എണ്ണം(cell->type))
		തിരികെ 1;

	എങ്കിൽ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		തിരികെ get(design_->module(cell->type));
	} അല്ലെങ്കിൽ എങ്കിൽ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		തിരികെ cell->getParam(ID::WIDTH).as_int();
	} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		തിരികെ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} അല്ലെങ്കിൽ എങ്കിൽ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		പൂർണ്ണസംഖ്യ width = cell->getParam(param).as_int();
		എങ്കിൽ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		തിരികെ width * y_coef(cell->type);
	} അല്ലെങ്കിൽ എങ്കിൽ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned പൂർണ്ണസംഖ്യ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		തിരികെ sum * sum_coef(cell->type);
	} അല്ലെങ്കിൽ എങ്കിൽ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned പൂർണ്ണസംഖ്യ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		തിരികെ max * max_inp_coef(cell->type);
	} അല്ലെങ്കിൽ എങ്കിൽ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned പൂർണ്ണസംഖ്യ sum = port_width_sum(cell);
		unsigned പൂർണ്ണസംഖ്യ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		തിരികെ coef * sum * sum;
	} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned പൂർണ്ണസംഖ്യ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned പൂർണ്ണസംഖ്യ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		വേണ്ടി (auto &term: macc.terms) {
			എങ്കിൽ (term.in_b.empty()) {
				// neglect addends
				തുടരുക;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned പൂർണ്ണസംഖ്യ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		തിരികെ cost_sum;
	} അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($lut)) {
		പൂർണ്ണസംഖ്യ width = cell->getParam(ID::WIDTH).as_int();
		unsigned പൂർണ്ണസംഖ്യ cost = 1U << (unsigned പൂർണ്ണസംഖ്യ)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		തിരികെ cost;
	} അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($sop)) {
		പൂർണ്ണസംഖ്യ width = cell->getParam(ID::WIDTH).as_int();
		പൂർണ്ണസംഖ്യ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		തിരികെ (2 * width + 1) * depth;
	} അല്ലെങ്കിൽ എങ്കിൽ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		തിരികെ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	തിരികെ 1;
}
