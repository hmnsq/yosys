#અન્તર્ભાવ "kernel/cost.h"
#અન્તર્ભાવ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned પૂર્ણાંક CellCosts::get(RTLIL::Module *mod)
{
	જો (mod_cost_cache_.ગણતરી(mod->name))
		પાછા mod_cost_cache_.at(mod->name);

	unsigned પૂર્ણાંક module_cost = 1;
	માટે (auto c : mod->cells()) {
		unsigned પૂર્ણાંક new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	પાછા module_cost;
}

સ્થિર unsigned પૂર્ણાંક y_coef(RTLIL::IdString type)
{
	જો (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		પાછા 1;
	} નહીંતર જો (type == ID($neg)) {
		પાછા 4;
	} નહીંતર જો (type == ID($demux)) {
		પાછા 2;
	} નહીંતર જો (type == ID($fa)) {
		પાછા 5;
	} નહીંતર જો (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		પાછા 8;
	} નહીંતર જો (type.in(ID($shl), ID($sshl))) {
		// left shift
		પાછા 10;
	}
	પાછા 0;
}

સ્થિર unsigned પૂર્ણાંક max_inp_coef(RTLIL::IdString type)
{
	જો (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		પાછા 1;
	} નહીંતર જો (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		પાછા 2;
	} નહીંતર જો (type == ID($lcu)) {
		પાછા 5;
	} નહીંતર જો (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		પાછા 7;
	}
	પાછા 0;
}

સ્થિર unsigned પૂર્ણાંક sum_coef(RTLIL::IdString type)
{
	જો (type.in(ID($shr), ID($sshr))) {
		// right shift
		પાછા 4;
	} નહીંતર જો (type.in(ID($shift), ID($shiftx))) {
		// shift
		પાછા 8;
	}
	પાછા 0;
}

સ્થિર unsigned પૂર્ણાંક is_div_mod(RTLIL::IdString type)
{
	પાછા (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

સ્થિર તાર્કિક is_free(RTLIL::IdString type)
{
	પાછા (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($પ્રતિજ્ઞા), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned પૂર્ણાંક max_inp_width(RTLIL::Cell *cell)
{
	unsigned પૂર્ણાંક max = 0;
	RTLIL::IdString આગત_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	જો (cell->type == ID($bmux))
		પાછા cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	માટે (RTLIL::IdString param : આગત_width_params)
		જો (cell->hasParam(param))
			max = std::max(max, (unsigned પૂર્ણાંક)cell->getParam(param).as_int());
	પાછા max;
}

unsigned પૂર્ણાંક port_width_sum(RTLIL::Cell *cell)
{
	unsigned પૂર્ણાંક sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	માટે (auto param : port_width_params)
		જો (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	પાછા sum;
}

unsigned પૂર્ણાંક CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	જો (cmos_gate_cost().ગણતરી(cell->type))
		પાછા 1;

	જો (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		પાછા get(design_->module(cell->type));
	} નહીંતર જો (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		પાછા cell->getParam(ID::WIDTH).as_int();
	} નહીંતર જો (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		પાછા cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} નહીંતર જો (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		પૂર્ણાંક width = cell->getParam(param).as_int();
		જો (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		પાછા width * y_coef(cell->type);
	} નહીંતર જો (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned પૂર્ણાંક sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		પાછા sum * sum_coef(cell->type);
	} નહીંતર જો (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned પૂર્ણાંક max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		પાછા max * max_inp_coef(cell->type);
	} નહીંતર જો (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned પૂર્ણાંક sum = port_width_sum(cell);
		unsigned પૂર્ણાંક coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		પાછા coef * sum * sum;
	} નહીંતર જો (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned પૂર્ણાંક cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned પૂર્ણાંક y_width = cell->getParam(ID::Y_WIDTH).as_int();
		માટે (auto &term: macc.terms) {
			જો (term.in_b.empty()) {
				// neglect addends
				ચાલુ;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned પૂર્ણાંક sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		પાછા cost_sum;
	} નહીંતર જો (cell->type == ID($lut)) {
		પૂર્ણાંક width = cell->getParam(ID::WIDTH).as_int();
		unsigned પૂર્ણાંક cost = 1U << (unsigned પૂર્ણાંક)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		પાછા cost;
	} નહીંતર જો (cell->type == ID($sop)) {
		પૂર્ણાંક width = cell->getParam(ID::WIDTH).as_int();
		પૂર્ણાંક depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		પાછા (2 * width + 1) * depth;
	} નહીંતર જો (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		પાછા 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	પાછા 1;
}
