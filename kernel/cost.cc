#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ཧྲིལ CellCosts::get(RTLIL::Module *mod)
{
	གལ་ཏེ (mod_cost_cache_.count(mod->name))
		ཕྱིར mod_cost_cache_.at(mod->name);

	unsigned ཧྲིལ module_cost = 1;
	ཆེད་དུ (auto c : mod->cells()) {
		unsigned ཧྲིལ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ཕྱིར module_cost;
}

static unsigned ཧྲིལ y_coef(RTLIL::IdString type)
{
	གལ་ཏེ (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ཕྱིར 1;
	} དེ་མིན གལ་ཏེ (type == ID($neg)) {
		ཕྱིར 4;
	} དེ་མིན གལ་ཏེ (type == ID($demux)) {
		ཕྱིར 2;
	} དེ་མིན གལ་ཏེ (type == ID($fa)) {
		ཕྱིར 5;
	} དེ་མིན གལ་ཏེ (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ཕྱིར 8;
	} དེ་མིན གལ་ཏེ (type.in(ID($shl), ID($sshl))) {
		// left shift
		ཕྱིར 10;
	}
	ཕྱིར 0;
}

static unsigned ཧྲིལ max_inp_coef(RTLIL::IdString type)
{
	གལ་ཏེ (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ཕྱིར 1;
	} དེ་མིན གལ་ཏེ (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ཕྱིར 2;
	} དེ་མིན གལ་ཏེ (type == ID($lcu)) {
		ཕྱིར 5;
	} དེ་མིན གལ་ཏེ (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ཕྱིར 7;
	}
	ཕྱིར 0;
}

static unsigned ཧྲིལ sum_coef(RTLIL::IdString type)
{
	གལ་ཏེ (type.in(ID($shr), ID($sshr))) {
		// right shift
		ཕྱིར 4;
	} དེ་མིན གལ་ཏེ (type.in(ID($shift), ID($shiftx))) {
		// shift
		ཕྱིར 8;
	}
	ཕྱིར 0;
}

static unsigned ཧྲིལ is_div_mod(RTLIL::IdString type)
{
	ཕྱིར (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ཕྱིར (
	  // tags
	  type.in(ID($overwrite_tag), ID($set_tag), ID($original_tag), ID($get_tag)) ||
	  // formal
	  type.in(ID($check), ID($equiv), ID($initstate), ID($assert), ID($assume), ID($live), ID($cover), ID($fair)) ||
	  type.in(ID($allseq), ID($allconst), ID($anyseq), ID($anyconst), ID($anyinit)) ||
	  // utilities
	  type.in(ID($scopeinfo), ID($print)) ||
	  // real but free
	  type.in(ID($concat), ID($slice), ID($pos)) ||
	  // specify
	  type.in(ID($specrule), ID($specify2), ID($specify3)));
}

unsigned ཧྲིལ max_inp_width(RTLIL::Cell *cell)
{
	unsigned ཧྲིལ max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	གལ་ཏེ (cell->type == ID($bmux))
		ཕྱིར cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	ཆེད་དུ (RTLIL::IdString param : input_width_params)
		གལ་ཏེ (cell->hasParam(param))
			max = std::max(max, (unsigned ཧྲིལ)cell->getParam(param).as_int());
	ཕྱིར max;
}

unsigned ཧྲིལ port_width_sum(RTLIL::Cell *cell)
{
	unsigned ཧྲིལ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	ཆེད་དུ (auto param : port_width_params)
		གལ་ཏེ (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ཕྱིར sum;
}

unsigned ཧྲིལ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	གལ་ཏེ (cmos_gate_cost().count(cell->type))
		ཕྱིར 1;

	གལ་ཏེ (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ཕྱིར get(design_->module(cell->type));
	} དེ་མིན གལ་ཏེ (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ཕྱིར cell->getParam(ID::WIDTH).as_int();
	} དེ་མིན གལ་ཏེ (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ཕྱིར cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} དེ་མིན གལ་ཏེ (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ཧྲིལ width = cell->getParam(param).as_int();
		གལ་ཏེ (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ཕྱིར width * y_coef(cell->type);
	} དེ་མིན གལ་ཏེ (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ཧྲིལ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ཕྱིར sum * sum_coef(cell->type);
	} དེ་མིན གལ་ཏེ (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ཧྲིལ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ཕྱིར max * max_inp_coef(cell->type);
	} དེ་མིན གལ་ཏེ (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ཧྲིལ sum = port_width_sum(cell);
		unsigned ཧྲིལ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ཕྱིར coef * sum * sum;
	} དེ་མིན གལ་ཏེ (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ཧྲིལ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ཧྲིལ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		ཆེད་དུ (auto &term: macc.terms) {
			གལ་ཏེ (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ཧྲིལ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ཕྱིར cost_sum;
	} དེ་མིན གལ་ཏེ (cell->type == ID($lut)) {
		ཧྲིལ width = cell->getParam(ID::WIDTH).as_int();
		unsigned ཧྲིལ cost = 1U << (unsigned ཧྲིལ)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ཕྱིར cost;
	} དེ་མིན གལ་ཏེ (cell->type == ID($sop)) {
		ཧྲིལ width = cell->getParam(ID::WIDTH).as_int();
		ཧྲིལ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ཕྱིར (2 * width + 1) * depth;
	} དེ་མིན གལ་ཏེ (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ཕྱིར 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ཕྱིར 1;
}
