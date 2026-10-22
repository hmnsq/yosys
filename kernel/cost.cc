#شامل "kernel/cost.h"
#شامل "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned صحیح CellCosts::get(RTLIL::Module *mod)
{
	اگر (mod_cost_cache_.شمار(mod->name))
		بازگشت mod_cost_cache_.at(mod->name);

	unsigned صحیح module_cost = 1;
	برای (auto c : mod->cells()) {
		unsigned صحیح new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	بازگشت module_cost;
}

ایستا unsigned صحیح y_coef(RTLIL::IdString type)
{
	اگر (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		بازگشت 1;
	} وگرنه اگر (type == ID($neg)) {
		بازگشت 4;
	} وگرنه اگر (type == ID($demux)) {
		بازگشت 2;
	} وگرنه اگر (type == ID($fa)) {
		بازگشت 5;
	} وگرنه اگر (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		بازگشت 8;
	} وگرنه اگر (type.in(ID($shl), ID($sshl))) {
		// left shift
		بازگشت 10;
	}
	بازگشت 0;
}

ایستا unsigned صحیح max_inp_coef(RTLIL::IdString type)
{
	اگر (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		بازگشت 1;
	} وگرنه اگر (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		بازگشت 2;
	} وگرنه اگر (type == ID($lcu)) {
		بازگشت 5;
	} وگرنه اگر (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		بازگشت 7;
	}
	بازگشت 0;
}

ایستا unsigned صحیح sum_coef(RTLIL::IdString type)
{
	اگر (type.in(ID($shr), ID($sshr))) {
		// right shift
		بازگشت 4;
	} وگرنه اگر (type.in(ID($shift), ID($shiftx))) {
		// shift
		بازگشت 8;
	}
	بازگشت 0;
}

ایستا unsigned صحیح is_div_mod(RTLIL::IdString type)
{
	بازگشت (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ایستا منطقی is_free(RTLIL::IdString type)
{
	بازگشت (
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

unsigned صحیح max_inp_width(RTLIL::Cell *cell)
{
	unsigned صحیح max = 0;
	RTLIL::IdString درون_داد_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	اگر (cell->type == ID($bmux))
		بازگشت cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	برای (RTLIL::IdString param : درون_داد_width_params)
		اگر (cell->hasParam(param))
			max = std::max(max, (unsigned صحیح)cell->getParam(param).as_int());
	بازگشت max;
}

unsigned صحیح port_width_sum(RTLIL::Cell *cell)
{
	unsigned صحیح sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	برای (auto param : port_width_params)
		اگر (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	بازگشت sum;
}

unsigned صحیح CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	اگر (cmos_gate_cost().شمار(cell->type))
		بازگشت 1;

	اگر (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		بازگشت get(design_->module(cell->type));
	} وگرنه اگر (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		بازگشت cell->getParam(ID::WIDTH).as_int();
	} وگرنه اگر (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		بازگشت cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} وگرنه اگر (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		صحیح width = cell->getParam(param).as_int();
		اگر (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		بازگشت width * y_coef(cell->type);
	} وگرنه اگر (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned صحیح sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		بازگشت sum * sum_coef(cell->type);
	} وگرنه اگر (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned صحیح max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		بازگشت max * max_inp_coef(cell->type);
	} وگرنه اگر (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned صحیح sum = port_width_sum(cell);
		unsigned صحیح coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		بازگشت coef * sum * sum;
	} وگرنه اگر (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned صحیح cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned صحیح y_width = cell->getParam(ID::Y_WIDTH).as_int();
		برای (auto &term: macc.terms) {
			اگر (term.in_b.empty()) {
				// neglect addends
				ادامه;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned صحیح sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		بازگشت cost_sum;
	} وگرنه اگر (cell->type == ID($lut)) {
		صحیح width = cell->getParam(ID::WIDTH).as_int();
		unsigned صحیح cost = 1U << (unsigned صحیح)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		بازگشت cost;
	} وگرنه اگر (cell->type == ID($sop)) {
		صحیح width = cell->getParam(ID::WIDTH).as_int();
		صحیح depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		بازگشت (2 * width + 1) * depth;
	} وگرنه اگر (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		بازگشت 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	بازگشت 1;
}
