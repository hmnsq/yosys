#অন্তর্ভাৱ "kernel/cost.h"
#অন্তর্ভাৱ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned পূর্ণাঙ্ক CellCosts::get(RTLIL::Module *mod)
{
	যদি (mod_cost_cache_.গুনতি(mod->name))
		ঘূৰাই mod_cost_cache_.at(mod->name);

	unsigned পূর্ণাঙ্ক module_cost = 1;
	বাবে (auto c : mod->cells()) {
		unsigned পূর্ণাঙ্ক new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ঘূৰাই module_cost;
}

স্থির unsigned পূর্ণাঙ্ক y_coef(RTLIL::IdString type)
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
		ঘূৰাই 1;
	} অন্যথা যদি (type == ID($neg)) {
		ঘূৰাই 4;
	} অন্যথা যদি (type == ID($demux)) {
		ঘূৰাই 2;
	} অন্যথা যদি (type == ID($fa)) {
		ঘূৰাই 5;
	} অন্যথা যদি (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ঘূৰাই 8;
	} অন্যথা যদি (type.in(ID($shl), ID($sshl))) {
		// left shift
		ঘূৰাই 10;
	}
	ঘূৰাই 0;
}

স্থির unsigned পূর্ণাঙ্ক max_inp_coef(RTLIL::IdString type)
{
	যদি (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ঘূৰাই 1;
	} অন্যথা যদি (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ঘূৰাই 2;
	} অন্যথা যদি (type == ID($lcu)) {
		ঘূৰাই 5;
	} অন্যথা যদি (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ঘূৰাই 7;
	}
	ঘূৰাই 0;
}

স্থির unsigned পূর্ণাঙ্ক sum_coef(RTLIL::IdString type)
{
	যদি (type.in(ID($shr), ID($sshr))) {
		// right shift
		ঘূৰাই 4;
	} অন্যথা যদি (type.in(ID($shift), ID($shiftx))) {
		// shift
		ঘূৰাই 8;
	}
	ঘূৰাই 0;
}

স্থির unsigned পূর্ণাঙ্ক is_div_mod(RTLIL::IdString type)
{
	ঘূৰাই (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

স্থির তার্কিক is_free(RTLIL::IdString type)
{
	ঘূৰাই (
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

unsigned পূর্ণাঙ্ক max_inp_width(RTLIL::Cell *cell)
{
	unsigned পূর্ণাঙ্ক max = 0;
	RTLIL::IdString প্ৰবেশ_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	যদি (cell->type == ID($bmux))
		ঘূৰাই cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	বাবে (RTLIL::IdString param : প্ৰবেশ_width_params)
		যদি (cell->hasParam(param))
			max = std::max(max, (unsigned পূর্ণাঙ্ক)cell->getParam(param).as_int());
	ঘূৰাই max;
}

unsigned পূর্ণাঙ্ক port_width_sum(RTLIL::Cell *cell)
{
	unsigned পূর্ণাঙ্ক sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	বাবে (auto param : port_width_params)
		যদি (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ঘূৰাই sum;
}

unsigned পূর্ণাঙ্ক CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	যদি (cmos_gate_cost().গুনতি(cell->type))
		ঘূৰাই 1;

	যদি (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ঘূৰাই get(design_->module(cell->type));
	} অন্যথা যদি (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ঘূৰাই cell->getParam(ID::WIDTH).as_int();
	} অন্যথা যদি (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ঘূৰাই cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} অন্যথা যদি (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		পূর্ণাঙ্ক width = cell->getParam(param).as_int();
		যদি (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ঘূৰাই width * y_coef(cell->type);
	} অন্যথা যদি (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned পূর্ণাঙ্ক sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ঘূৰাই sum * sum_coef(cell->type);
	} অন্যথা যদি (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned পূর্ণাঙ্ক max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ঘূৰাই max * max_inp_coef(cell->type);
	} অন্যথা যদি (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned পূর্ণাঙ্ক sum = port_width_sum(cell);
		unsigned পূর্ণাঙ্ক coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ঘূৰাই coef * sum * sum;
	} অন্যথা যদি (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned পূর্ণাঙ্ক cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned পূর্ণাঙ্ক y_width = cell->getParam(ID::Y_WIDTH).as_int();
		বাবে (auto &term: macc.terms) {
			যদি (term.in_b.empty()) {
				// neglect addends
				অনুৱর্ত;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned পূর্ণাঙ্ক sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ঘূৰাই cost_sum;
	} অন্যথা যদি (cell->type == ID($lut)) {
		পূর্ণাঙ্ক width = cell->getParam(ID::WIDTH).as_int();
		unsigned পূর্ণাঙ্ক cost = 1U << (unsigned পূর্ণাঙ্ক)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ঘূৰাই cost;
	} অন্যথা যদি (cell->type == ID($sop)) {
		পূর্ণাঙ্ক width = cell->getParam(ID::WIDTH).as_int();
		পূর্ণাঙ্ক depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ঘূৰাই (2 * width + 1) * depth;
	} অন্যথা যদি (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ঘূৰাই 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ঘূৰাই 1;
}
