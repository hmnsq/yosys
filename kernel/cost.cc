#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned цяло CellCosts::get(RTLIL::Module *mod)
{
	ако (mod_cost_cache_.count(mod->name))
		върни mod_cost_cache_.at(mod->name);

	unsigned цяло module_cost = 1;
	за (auto c : mod->cells()) {
		unsigned цяло new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	върни module_cost;
}

static unsigned цяло y_coef(RTLIL::IdString type)
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
		върни 1;
	} иначе ако (type == ID($neg)) {
		върни 4;
	} иначе ако (type == ID($demux)) {
		върни 2;
	} иначе ако (type == ID($fa)) {
		върни 5;
	} иначе ако (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		върни 8;
	} иначе ако (type.in(ID($shl), ID($sshl))) {
		// left shift
		върни 10;
	}
	върни 0;
}

static unsigned цяло max_inp_coef(RTLIL::IdString type)
{
	ако (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		върни 1;
	} иначе ако (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		върни 2;
	} иначе ако (type == ID($lcu)) {
		върни 5;
	} иначе ако (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		върни 7;
	}
	върни 0;
}

static unsigned цяло sum_coef(RTLIL::IdString type)
{
	ако (type.in(ID($shr), ID($sshr))) {
		// right shift
		върни 4;
	} иначе ако (type.in(ID($shift), ID($shiftx))) {
		// shift
		върни 8;
	}
	върни 0;
}

static unsigned цяло is_div_mod(RTLIL::IdString type)
{
	върни (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	върни (
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

unsigned цяло max_inp_width(RTLIL::Cell *cell)
{
	unsigned цяло max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	ако (cell->type == ID($bmux))
		върни cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	за (RTLIL::IdString param : input_width_params)
		ако (cell->hasParam(param))
			max = std::max(max, (unsigned цяло)cell->getParam(param).as_int());
	върни max;
}

unsigned цяло port_width_sum(RTLIL::Cell *cell)
{
	unsigned цяло sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	за (auto param : port_width_params)
		ако (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	върни sum;
}

unsigned цяло CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	ако (cmos_gate_cost().count(cell->type))
		върни 1;

	ако (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		върни get(design_->module(cell->type));
	} иначе ако (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		върни cell->getParam(ID::WIDTH).as_int();
	} иначе ако (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		върни cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} иначе ако (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		цяло width = cell->getParam(param).as_int();
		ако (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		върни width * y_coef(cell->type);
	} иначе ако (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned цяло sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		върни sum * sum_coef(cell->type);
	} иначе ако (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned цяло max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		върни max * max_inp_coef(cell->type);
	} иначе ако (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned цяло sum = port_width_sum(cell);
		unsigned цяло coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		върни coef * sum * sum;
	} иначе ако (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned цяло cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned цяло y_width = cell->getParam(ID::Y_WIDTH).as_int();
		за (auto &term: macc.terms) {
			ако (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned цяло sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		върни cost_sum;
	} иначе ако (cell->type == ID($lut)) {
		цяло width = cell->getParam(ID::WIDTH).as_int();
		unsigned цяло cost = 1U << (unsigned цяло)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		върни cost;
	} иначе ако (cell->type == ID($sop)) {
		цяло width = cell->getParam(ID::WIDTH).as_int();
		цяло depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		върни (2 * width + 1) * depth;
	} иначе ако (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		върни 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	върни 1;
}
