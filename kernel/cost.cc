#ಅನ್ತರ್ಭಾವ "kernel/cost.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ಪೂರ್ಣಾಙ್ಕ CellCosts::get(RTLIL::Module *mod)
{
	ಯದಿ (mod_cost_cache_.ಎಣಿಕೆ(mod->name))
		ಪ್ರತ್ಯಾಗಮ mod_cost_cache_.at(mod->name);

	unsigned ಪೂರ್ಣಾಙ್ಕ module_cost = 1;
	ಪ್ರತಿ (auto c : mod->cells()) {
		unsigned ಪೂರ್ಣಾಙ್ಕ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ಪ್ರತ್ಯಾಗಮ module_cost;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ y_coef(RTLIL::IdString type)
{
	ಯದಿ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ಪ್ರತ್ಯಾಗಮ 1;
	} ಅನ್ಯಥಾ ಯದಿ (type == ID($neg)) {
		ಪ್ರತ್ಯಾಗಮ 4;
	} ಅನ್ಯಥಾ ಯದಿ (type == ID($demux)) {
		ಪ್ರತ್ಯಾಗಮ 2;
	} ಅನ್ಯಥಾ ಯದಿ (type == ID($fa)) {
		ಪ್ರತ್ಯಾಗಮ 5;
	} ಅನ್ಯಥಾ ಯದಿ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ಪ್ರತ್ಯಾಗಮ 8;
	} ಅನ್ಯಥಾ ಯದಿ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ಪ್ರತ್ಯಾಗಮ 10;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ max_inp_coef(RTLIL::IdString type)
{
	ಯದಿ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ಪ್ರತ್ಯಾಗಮ 1;
	} ಅನ್ಯಥಾ ಯದಿ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ಪ್ರತ್ಯಾಗಮ 2;
	} ಅನ್ಯಥಾ ಯದಿ (type == ID($lcu)) {
		ಪ್ರತ್ಯಾಗಮ 5;
	} ಅನ್ಯಥಾ ಯದಿ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ಪ್ರತ್ಯಾಗಮ 7;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ sum_coef(RTLIL::IdString type)
{
	ಯದಿ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ಪ್ರತ್ಯಾಗಮ 4;
	} ಅನ್ಯಥಾ ಯದಿ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ಪ್ರತ್ಯಾಗಮ 8;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ is_div_mod(RTLIL::IdString type)
{
	ಪ್ರತ್ಯಾಗಮ (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ is_free(RTLIL::IdString type)
{
	ಪ್ರತ್ಯಾಗಮ (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned ಪೂರ್ಣಾಙ್ಕ max_inp_width(RTLIL::Cell *cell)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ max = 0;
	RTLIL::IdString ಒಳಹರಿವು_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ಯದಿ (cell->type == ID($bmux))
		ಪ್ರತ್ಯಾಗಮ cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	ಪ್ರತಿ (RTLIL::IdString param : ಒಳಹರಿವು_width_params)
		ಯದಿ (cell->hasParam(param))
			max = std::max(max, (unsigned ಪೂರ್ಣಾಙ್ಕ)cell->getParam(param).as_int());
	ಪ್ರತ್ಯಾಗಮ max;
}

unsigned ಪೂರ್ಣಾಙ್ಕ port_width_sum(RTLIL::Cell *cell)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	ಪ್ರತಿ (auto param : port_width_params)
		ಯದಿ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ಪ್ರತ್ಯಾಗಮ sum;
}

unsigned ಪೂರ್ಣಾಙ್ಕ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ಯದಿ (cmos_gate_cost().ಎಣಿಕೆ(cell->type))
		ಪ್ರತ್ಯಾಗಮ 1;

	ಯದಿ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ಪ್ರತ್ಯಾಗಮ get(design_->module(cell->type));
	} ಅನ್ಯಥಾ ಯದಿ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ಪ್ರತ್ಯಾಗಮ cell->getParam(ID::WIDTH).as_int();
	} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ಪ್ರತ್ಯಾಗಮ cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} ಅನ್ಯಥಾ ಯದಿ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ಪೂರ್ಣಾಙ್ಕ width = cell->getParam(param).as_int();
		ಯದಿ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ಪ್ರತ್ಯಾಗಮ width * y_coef(cell->type);
	} ಅನ್ಯಥಾ ಯದಿ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ಪೂರ್ಣಾಙ್ಕ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ಪ್ರತ್ಯಾಗಮ sum * sum_coef(cell->type);
	} ಅನ್ಯಥಾ ಯದಿ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ಪೂರ್ಣಾಙ್ಕ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ಪ್ರತ್ಯಾಗಮ max * max_inp_coef(cell->type);
	} ಅನ್ಯಥಾ ಯದಿ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ಪೂರ್ಣಾಙ್ಕ sum = port_width_sum(cell);
		unsigned ಪೂರ್ಣಾಙ್ಕ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ಪ್ರತ್ಯಾಗಮ coef * sum * sum;
	} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ಪೂರ್ಣಾಙ್ಕ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ಪೂರ್ಣಾಙ್ಕ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		ಪ್ರತಿ (auto &term: macc.terms) {
			ಯದಿ (term.in_b.empty()) {
				// neglect addends
				ಅನುವರ್ತ;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ಪೂರ್ಣಾಙ್ಕ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ಪ್ರತ್ಯಾಗಮ cost_sum;
	} ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($lut)) {
		ಪೂರ್ಣಾಙ್ಕ width = cell->getParam(ID::WIDTH).as_int();
		unsigned ಪೂರ್ಣಾಙ್ಕ cost = 1U << (unsigned ಪೂರ್ಣಾಙ್ಕ)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ಪ್ರತ್ಯಾಗಮ cost;
	} ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($sop)) {
		ಪೂರ್ಣಾಙ್ಕ width = cell->getParam(ID::WIDTH).as_int();
		ಪೂರ್ಣಾಙ್ಕ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ಪ್ರತ್ಯಾಗಮ (2 * width + 1) * depth;
	} ಅನ್ಯಥಾ ಯದಿ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ಪ್ರತ್ಯಾಗಮ 1;
}
