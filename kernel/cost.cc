#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned цео CellCosts::get(RTLIL::Module *mod)
{
	ако (mod_cost_cache_.count(mod->name))
		врати mod_cost_cache_.at(mod->name);

	unsigned цео module_cost = 1;
	за (auto c : mod->cells()) {
		unsigned цео new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	врати module_cost;
}

static unsigned цео y_coef(RTLIL::IdString type)
{
	ако (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		врати 1;
	} инако ако (type == ID($neg)) {
		врати 4;
	} инако ако (type == ID($demux)) {
		врати 2;
	} инако ако (type == ID($fa)) {
		врати 5;
	} инако ако (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		врати 8;
	} инако ако (type.in(ID($shl), ID($sshl))) {
		// left shift
		врати 10;
	}
	врати 0;
}

static unsigned цео max_inp_coef(RTLIL::IdString type)
{
	ако (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		врати 1;
	} инако ако (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		врати 2;
	} инако ако (type == ID($lcu)) {
		врати 5;
	} инако ако (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		врати 7;
	}
	врати 0;
}

static unsigned цео sum_coef(RTLIL::IdString type)
{
	ако (type.in(ID($shr), ID($sshr))) {
		// right shift
		врати 4;
	} инако ако (type.in(ID($shift), ID($shiftx))) {
		// shift
		врати 8;
	}
	врати 0;
}

static unsigned цео is_div_mod(RTLIL::IdString type)
{
	врати (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	врати (
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

unsigned цео max_inp_width(RTLIL::Cell *cell)
{
	unsigned цео max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ако (cell->type == ID($bmux))
		врати cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	за (RTLIL::IdString param : input_width_params)
		ако (cell->hasParam(param))
			max = std::max(max, (unsigned цео)cell->getParam(param).as_int());
	врати max;
}

unsigned цео port_width_sum(RTLIL::Cell *cell)
{
	unsigned цео sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	за (auto param : port_width_params)
		ако (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	врати sum;
}

unsigned цео CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ако (cmos_gate_cost().count(cell->type))
		врати 1;

	ако (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		врати get(design_->module(cell->type));
	} инако ако (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		врати cell->getParam(ID::WIDTH).as_int();
	} инако ако (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		врати cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} инако ако (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		цео width = cell->getParam(param).as_int();
		ако (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		врати width * y_coef(cell->type);
	} инако ако (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned цео sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		врати sum * sum_coef(cell->type);
	} инако ако (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned цео max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		врати max * max_inp_coef(cell->type);
	} инако ако (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned цео sum = port_width_sum(cell);
		unsigned цео coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		врати coef * sum * sum;
	} инако ако (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned цео cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned цео y_width = cell->getParam(ID::Y_WIDTH).as_int();
		за (auto &term: macc.terms) {
			ако (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned цео sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		врати cost_sum;
	} инако ако (cell->type == ID($lut)) {
		цео width = cell->getParam(ID::WIDTH).as_int();
		unsigned цео cost = 1U << (unsigned цео)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		врати cost;
	} инако ако (cell->type == ID($sop)) {
		цео width = cell->getParam(ID::WIDTH).as_int();
		цео depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		врати (2 * width + 1) * depth;
	} инако ако (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		врати 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	врати 1;
}
