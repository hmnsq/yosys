#ضمن "kernel/cost.h"
#ضمن "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned صحيح CellCosts::get(RTLIL::Module *mod)
{
	إذا (mod_cost_cache_.تعداد_2(mod->name))
		إرجاع mod_cost_cache_.at(mod->name);

	unsigned صحيح module_cost = 1;
	لكل (auto c : mod->cells()) {
		unsigned صحيح new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	إرجاع module_cost;
}

ساكن unsigned صحيح y_coef(RTLIL::IdString type)
{
	إذا (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		إرجاع 1;
	} وإلا إذا (type == ID($neg)) {
		إرجاع 4;
	} وإلا إذا (type == ID($demux)) {
		إرجاع 2;
	} وإلا إذا (type == ID($fa)) {
		إرجاع 5;
	} وإلا إذا (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		إرجاع 8;
	} وإلا إذا (type.in(ID($shl), ID($sshl))) {
		// left shift
		إرجاع 10;
	}
	إرجاع 0;
}

ساكن unsigned صحيح max_inp_coef(RTLIL::IdString type)
{
	إذا (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		إرجاع 1;
	} وإلا إذا (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		إرجاع 2;
	} وإلا إذا (type == ID($lcu)) {
		إرجاع 5;
	} وإلا إذا (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		إرجاع 7;
	}
	إرجاع 0;
}

ساكن unsigned صحيح sum_coef(RTLIL::IdString type)
{
	إذا (type.in(ID($shr), ID($sshr))) {
		// right shift
		إرجاع 4;
	} وإلا إذا (type.in(ID($shift), ID($shiftx))) {
		// shift
		إرجاع 8;
	}
	إرجاع 0;
}

ساكن unsigned صحيح is_div_mod(RTLIL::IdString type)
{
	إرجاع (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ساكن منطقي is_free(RTLIL::IdString type)
{
	إرجاع (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($أكد), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned صحيح max_inp_width(RTLIL::Cell *cell)
{
	unsigned صحيح max = 0;
	RTLIL::IdString إدخال_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	إذا (cell->type == ID($bmux))
		إرجاع cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	لكل (RTLIL::IdString param : إدخال_width_params)
		إذا (cell->hasParam(param))
			max = std::max(max, (unsigned صحيح)cell->getParam(param).as_int());
	إرجاع max;
}

unsigned صحيح port_width_sum(RTLIL::Cell *cell)
{
	unsigned صحيح sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	لكل (auto param : port_width_params)
		إذا (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	إرجاع sum;
}

unsigned صحيح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	إذا (cmos_gate_cost().تعداد_2(cell->type))
		إرجاع 1;

	إذا (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		إرجاع get(design_->module(cell->type));
	} وإلا إذا (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		إرجاع cell->getParam(ID::WIDTH).as_int();
	} وإلا إذا (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		إرجاع cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} وإلا إذا (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		صحيح width = cell->getParam(param).as_int();
		إذا (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		إرجاع width * y_coef(cell->type);
	} وإلا إذا (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned صحيح sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		إرجاع sum * sum_coef(cell->type);
	} وإلا إذا (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned صحيح max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		إرجاع max * max_inp_coef(cell->type);
	} وإلا إذا (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned صحيح sum = port_width_sum(cell);
		unsigned صحيح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		إرجاع coef * sum * sum;
	} وإلا إذا (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned صحيح cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned صحيح y_width = cell->getParam(ID::Y_WIDTH).as_int();
		لكل (auto &term: macc.terms) {
			إذا (term.in_b.empty()) {
				// neglect addends
				استمر;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned صحيح sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		إرجاع cost_sum;
	} وإلا إذا (cell->type == ID($lut)) {
		صحيح width = cell->getParam(ID::WIDTH).as_int();
		unsigned صحيح cost = 1U << (unsigned صحيح)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		إرجاع cost;
	} وإلا إذا (cell->type == ID($sop)) {
		صحيح width = cell->getParam(ID::WIDTH).as_int();
		صحيح depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		إرجاع (2 * width + 1) * depth;
	} وإلا إذا (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		إرجاع 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	إرجاع 1;
}
