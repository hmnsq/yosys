#ଅନ୍ତର୍ଭାଵ "kernel/cost.h"
#ଅନ୍ତର୍ଭାଵ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା CellCosts::get(RTLIL::Module *mod)
{
	ଯଦି (mod_cost_cache_.ଗଣନା_2(mod->name))
		ଫେରନ୍ତୁ mod_cost_cache_.at(mod->name);

	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା module_cost = 1;
	ପାଇଁ (auto c : mod->cells()) {
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ଫେରନ୍ତୁ module_cost;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_coef(RTLIL::IdString type)
{
	ଯଦି (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ଫେରନ୍ତୁ 1;
	} ଅନ୍ୟଥା ଯଦି (type == ID($neg)) {
		ଫେରନ୍ତୁ 4;
	} ଅନ୍ୟଥା ଯଦି (type == ID($demux)) {
		ଫେରନ୍ତୁ 2;
	} ଅନ୍ୟଥା ଯଦି (type == ID($fa)) {
		ଫେରନ୍ତୁ 5;
	} ଅନ୍ୟଥା ଯଦି (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ଫେରନ୍ତୁ 8;
	} ଅନ୍ୟଥା ଯଦି (type.in(ID($shl), ID($sshl))) {
		// left shift
		ଫେରନ୍ତୁ 10;
	}
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_inp_coef(RTLIL::IdString type)
{
	ଯଦି (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ଫେରନ୍ତୁ 1;
	} ଅନ୍ୟଥା ଯଦି (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ଫେରନ୍ତୁ 2;
	} ଅନ୍ୟଥା ଯଦି (type == ID($lcu)) {
		ଫେରନ୍ତୁ 5;
	} ଅନ୍ୟଥା ଯଦି (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ଫେରନ୍ତୁ 7;
	}
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum_coef(RTLIL::IdString type)
{
	ଯଦି (type.in(ID($shr), ID($sshr))) {
		// right shift
		ଫେରନ୍ତୁ 4;
	} ଅନ୍ୟଥା ଯଦି (type.in(ID($shift), ID($shiftx))) {
		// shift
		ଫେରନ୍ତୁ 8;
	}
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_div_mod(RTLIL::IdString type)
{
	ଫେରନ୍ତୁ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ସ୍ଥିର ତାର୍କିକ is_free(RTLIL::IdString type)
{
	ଫେରନ୍ତୁ (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($ପ୍ରତିଜ୍ଞା), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_inp_width(RTLIL::Cell *cell)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = 0;
	RTLIL::IdString ନିବେଶ_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ଯଦି (cell->type == ID($bmux))
		ଫେରନ୍ତୁ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	ପାଇଁ (RTLIL::IdString param : ନିବେଶ_width_params)
		ଯଦି (cell->hasParam(param))
			max = std::max(max, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cell->getParam(param).as_int());
	ଫେରନ୍ତୁ max;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା port_width_sum(RTLIL::Cell *cell)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	ପାଇଁ (auto param : port_width_params)
		ଯଦି (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ଫେରନ୍ତୁ sum;
}

unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ଯଦି (cmos_gate_cost().ଗଣନା_2(cell->type))
		ଫେରନ୍ତୁ 1;

	ଯଦି (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ଫେରନ୍ତୁ get(design_->module(cell->type));
	} ଅନ୍ୟଥା ଯଦି (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ଫେରନ୍ତୁ cell->getParam(ID::WIDTH).as_int();
	} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ଫେରନ୍ତୁ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} ଅନ୍ୟଥା ଯଦି (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getParam(param).as_int();
		ଯଦି (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ଫେରନ୍ତୁ width * y_coef(cell->type);
	} ଅନ୍ୟଥା ଯଦି (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ଫେରନ୍ତୁ sum * sum_coef(cell->type);
	} ଅନ୍ୟଥା ଯଦି (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ଫେରନ୍ତୁ max * max_inp_coef(cell->type);
	} ଅନ୍ୟଥା ଯଦି (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = port_width_sum(cell);
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ଫେରନ୍ତୁ coef * sum * sum;
	} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = cell->getParam(ID::Y_WIDTH).as_int();
		ପାଇଁ (auto &term: macc.terms) {
			ଯଦି (term.in_b.empty()) {
				// neglect addends
				ଜାରି;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ଫେରନ୍ତୁ cost_sum;
	} ଅନ୍ୟଥା ଯଦି (cell->type == ID($lut)) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getParam(ID::WIDTH).as_int();
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cost = 1U << (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ଫେରନ୍ତୁ cost;
	} ଅନ୍ୟଥା ଯଦି (cell->type == ID($sop)) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getParam(ID::WIDTH).as_int();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ଫେରନ୍ତୁ (2 * width + 1) * depth;
	} ଅନ୍ୟଥା ଯଦି (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ଫେରନ୍ତୁ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ଫେରନ୍ତୁ 1;
}
