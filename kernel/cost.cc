#অন্তর্ভাব "kernel/cost.h"
#অন্তর্ভাব "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned পূর্ণসংখ্যা CellCosts::get(RTLIL::Module *mod)
{
	যদি (mod_cost_cache_.গুনতি(mod->name))
		ফেরত mod_cost_cache_.at(mod->name);

	unsigned পূর্ণসংখ্যা module_cost = 1;
	জন্য (auto c : mod->cells()) {
		unsigned পূর্ণসংখ্যা new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ফেরত module_cost;
}

স্থির unsigned পূর্ণসংখ্যা y_coef(RTLIL::IdString type)
{
	যদি (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ফেরত 1;
	} অন্যথা যদি (type == ID($neg)) {
		ফেরত 4;
	} অন্যথা যদি (type == ID($demux)) {
		ফেরত 2;
	} অন্যথা যদি (type == ID($fa)) {
		ফেরত 5;
	} অন্যথা যদি (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ফেরত 8;
	} অন্যথা যদি (type.in(ID($shl), ID($sshl))) {
		// left shift
		ফেরত 10;
	}
	ফেরত 0;
}

স্থির unsigned পূর্ণসংখ্যা max_inp_coef(RTLIL::IdString type)
{
	যদি (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ফেরত 1;
	} অন্যথা যদি (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ফেরত 2;
	} অন্যথা যদি (type == ID($lcu)) {
		ফেরত 5;
	} অন্যথা যদি (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ফেরত 7;
	}
	ফেরত 0;
}

স্থির unsigned পূর্ণসংখ্যা যোগ_coef(RTLIL::IdString type)
{
	যদি (type.in(ID($shr), ID($sshr))) {
		// right shift
		ফেরত 4;
	} অন্যথা যদি (type.in(ID($shift), ID($shiftx))) {
		// shift
		ফেরত 8;
	}
	ফেরত 0;
}

স্থির unsigned পূর্ণসংখ্যা is_div_mod(RTLIL::IdString type)
{
	ফেরত (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

স্থির যৌক্তিক is_free(RTLIL::IdString type)
{
	ফেরত (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($প্রতিজ্ঞা), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned পূর্ণসংখ্যা max_inp_width(RTLIL::Cell *cell)
{
	unsigned পূর্ণসংখ্যা max = 0;
	RTLIL::IdString প্রবেশ_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	যদি (cell->type == ID($bmux))
		ফেরত cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	জন্য (RTLIL::IdString param : প্রবেশ_width_params)
		যদি (cell->hasParam(param))
			max = std::max(max, (unsigned পূর্ণসংখ্যা)cell->getParam(param).as_int());
	ফেরত max;
}

unsigned পূর্ণসংখ্যা port_width_যোগ(RTLIL::Cell *cell)
{
	unsigned পূর্ণসংখ্যা যোগ = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	জন্য (auto param : port_width_params)
		যদি (cell->hasParam(param))
			যোগ += cell->getParam(param).as_int();

	ফেরত যোগ;
}

unsigned পূর্ণসংখ্যা CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	যদি (cmos_gate_cost().গুনতি(cell->type))
		ফেরত 1;

	যদি (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ফেরত get(design_->module(cell->type));
	} অন্যথা যদি (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ফেরত cell->getParam(ID::WIDTH).as_int();
	} অন্যথা যদি (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ফেরত cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::আকার_3).as_int();
	} অন্যথা যদি (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		পূর্ণসংখ্যা width = cell->getParam(param).as_int();
		যদি (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ফেরত width * y_coef(cell->type);
	} অন্যথা যদি (যোগ_coef(cell->type)) {
		// linear with sum of port widths
		unsigned পূর্ণসংখ্যা যোগ = port_width_যোগ(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, যোগ, যোগ_coef(cell->type));
		ফেরত যোগ * যোগ_coef(cell->type);
	} অন্যথা যদি (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned পূর্ণসংখ্যা max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ফেরত max * max_inp_coef(cell->type);
	} অন্যথা যদি (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned পূর্ণসংখ্যা যোগ = port_width_যোগ(cell);
		unsigned পূর্ণসংখ্যা coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, যোগ * যোগ);
		ফেরত coef * যোগ * যোগ;
	} অন্যথা যদি (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned পূর্ণসংখ্যা cost_যোগ = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned পূর্ণসংখ্যা y_width = cell->getParam(ID::Y_WIDTH).as_int();
		জন্য (auto &term: macc.terms) {
			যদি (term.in_b.empty()) {
				// neglect addends
				চালু;
			}
			unsigned a_width = term.in_a.আকার_2(), b_width = term.in_b.আকার_2();
			unsigned পূর্ণসংখ্যা যোগ = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_যোগ += 3 * যোগ * যোগ;
		}
		ফেরত cost_যোগ;
	} অন্যথা যদি (cell->type == ID($lut)) {
		পূর্ণসংখ্যা width = cell->getParam(ID::WIDTH).as_int();
		unsigned পূর্ণসংখ্যা cost = 1U << (unsigned পূর্ণসংখ্যা)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ফেরত cost;
	} অন্যথা যদি (cell->type == ID($sop)) {
		পূর্ণসংখ্যা width = cell->getParam(ID::WIDTH).as_int();
		পূর্ণসংখ্যা depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ফেরত (2 * width + 1) * depth;
	} অন্যথা যদি (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ফেরত 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), Get_আকার(cell->parameters));
	ফেরত 1;
}
