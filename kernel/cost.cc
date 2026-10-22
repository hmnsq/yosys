#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ཧྲིལ CellCosts::get(RTLIL::Module *mod)
{
	གལ་སྲིད (mod_cost_cache_.count(mod->name))
		ལོག mod_cost_cache_.at(mod->name);

	unsigned ཧྲིལ module_cost = 1;
	དོན་ལུ (auto c : mod->cells()) {
		unsigned ཧྲིལ new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	ལོག module_cost;
}

static unsigned ཧྲིལ y_coef(RTLIL::IdString type)
{
	གལ་སྲིད (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		ལོག 1;
	} དེ་མིན གལ་སྲིད (type == ID($neg)) {
		ལོག 4;
	} དེ་མིན གལ་སྲིད (type == ID($demux)) {
		ལོག 2;
	} དེ་མིན གལ་སྲིད (type == ID($fa)) {
		ལོག 5;
	} དེ་མིན གལ་སྲིད (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		ལོག 8;
	} དེ་མིན གལ་སྲིད (type.in(ID($shl), ID($sshl))) {
		// left shift
		ལོག 10;
	}
	ལོག 0;
}

static unsigned ཧྲིལ max_inp_coef(RTLIL::IdString type)
{
	གལ་སྲིད (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		ལོག 1;
	} དེ་མིན གལ་སྲིད (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		ལོག 2;
	} དེ་མིན གལ་སྲིད (type == ID($lcu)) {
		ལོག 5;
	} དེ་མིན གལ་སྲིད (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		ལོག 7;
	}
	ལོག 0;
}

static unsigned ཧྲིལ sum_coef(RTLIL::IdString type)
{
	གལ་སྲིད (type.in(ID($shr), ID($sshr))) {
		// right shift
		ལོག 4;
	} དེ་མིན གལ་སྲིད (type.in(ID($shift), ID($shiftx))) {
		// shift
		ལོག 8;
	}
	ལོག 0;
}

static unsigned ཧྲིལ is_div_mod(RTLIL::IdString type)
{
	ལོག (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	ལོག (
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

	གལ་སྲིད (cell->type == ID($bmux))
		ལོག cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	དོན་ལུ (RTLIL::IdString param : input_width_params)
		གལ་སྲིད (cell->hasParam(param))
			max = std::max(max, (unsigned ཧྲིལ)cell->getParam(param).as_int());
	ལོག max;
}

unsigned ཧྲིལ port_width_sum(RTLIL::Cell *cell)
{
	unsigned ཧྲིལ sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	དོན་ལུ (auto param : port_width_params)
		གལ་སྲིད (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	ལོག sum;
}

unsigned ཧྲིལ CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	གལ་སྲིད (cmos_gate_cost().count(cell->type))
		ལོག 1;

	གལ་སྲིད (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		ལོག get(design_->module(cell->type));
	} དེ་མིན གལ་སྲིད (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		ལོག cell->getParam(ID::WIDTH).as_int();
	} དེ་མིན གལ་སྲིད (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		ལོག cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} དེ་མིན གལ་སྲིད (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ཧྲིལ width = cell->getParam(param).as_int();
		གལ་སྲིད (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		ལོག width * y_coef(cell->type);
	} དེ་མིན གལ་སྲིད (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ཧྲིལ sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		ལོག sum * sum_coef(cell->type);
	} དེ་མིན གལ་སྲིད (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ཧྲིལ max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		ལོག max * max_inp_coef(cell->type);
	} དེ་མིན གལ་སྲིད (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ཧྲིལ sum = port_width_sum(cell);
		unsigned ཧྲིལ coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		ལོག coef * sum * sum;
	} དེ་མིན གལ་སྲིད (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ཧྲིལ cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ཧྲིལ y_width = cell->getParam(ID::Y_WIDTH).as_int();
		དོན་ལུ (auto &term: macc.terms) {
			གལ་སྲིད (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ཧྲིལ sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		ལོག cost_sum;
	} དེ་མིན གལ་སྲིད (cell->type == ID($lut)) {
		ཧྲིལ width = cell->getParam(ID::WIDTH).as_int();
		unsigned ཧྲིལ cost = 1U << (unsigned ཧྲིལ)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		ལོག cost;
	} དེ་མིན གལ་སྲིད (cell->type == ID($sop)) {
		ཧྲིལ width = cell->getParam(ID::WIDTH).as_int();
		ཧྲིལ depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		ལོག (2 * width + 1) * depth;
	} དེ་མིན གལ་སྲིད (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		ལོག 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	ལོག 1;
}
