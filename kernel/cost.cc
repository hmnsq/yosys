#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned ціле CellCosts::get(RTLIL::Module *mod)
{
	якщо (mod_cost_cache_.count(mod->name))
		повернути mod_cost_cache_.at(mod->name);

	unsigned ціле module_cost = 1;
	для (auto c : mod->cells()) {
		unsigned ціле new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	повернути module_cost;
}

static unsigned ціле y_coef(RTLIL::IdString type)
{
	якщо (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		повернути 1;
	} інакше якщо (type == ID($neg)) {
		повернути 4;
	} інакше якщо (type == ID($demux)) {
		повернути 2;
	} інакше якщо (type == ID($fa)) {
		повернути 5;
	} інакше якщо (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		повернути 8;
	} інакше якщо (type.in(ID($shl), ID($sshl))) {
		// left shift
		повернути 10;
	}
	повернути 0;
}

static unsigned ціле max_inp_coef(RTLIL::IdString type)
{
	якщо (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		повернути 1;
	} інакше якщо (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		повернути 2;
	} інакше якщо (type == ID($lcu)) {
		повернути 5;
	} інакше якщо (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		повернути 7;
	}
	повернути 0;
}

static unsigned ціле sum_coef(RTLIL::IdString type)
{
	якщо (type.in(ID($shr), ID($sshr))) {
		// right shift
		повернути 4;
	} інакше якщо (type.in(ID($shift), ID($shiftx))) {
		// shift
		повернути 8;
	}
	повернути 0;
}

static unsigned ціле is_div_mod(RTLIL::IdString type)
{
	повернути (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	повернути (
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

unsigned ціле max_inp_width(RTLIL::Cell *cell)
{
	unsigned ціле max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	якщо (cell->type == ID($bmux))
		повернути cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	для (RTLIL::IdString param : input_width_params)
		якщо (cell->hasParam(param))
			max = std::max(max, (unsigned ціле)cell->getParam(param).as_int());
	повернути max;
}

unsigned ціле port_width_sum(RTLIL::Cell *cell)
{
	unsigned ціле sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	для (auto param : port_width_params)
		якщо (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	повернути sum;
}

unsigned ціле CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	якщо (cmos_gate_cost().count(cell->type))
		повернути 1;

	якщо (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		повернути get(design_->module(cell->type));
	} інакше якщо (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		повернути cell->getParam(ID::WIDTH).as_int();
	} інакше якщо (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		повернути cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} інакше якщо (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		ціле width = cell->getParam(param).as_int();
		якщо (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		повернути width * y_coef(cell->type);
	} інакше якщо (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned ціле sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		повернути sum * sum_coef(cell->type);
	} інакше якщо (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned ціле max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		повернути max * max_inp_coef(cell->type);
	} інакше якщо (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned ціле sum = port_width_sum(cell);
		unsigned ціле coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		повернути coef * sum * sum;
	} інакше якщо (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned ціле cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned ціле y_width = cell->getParam(ID::Y_WIDTH).as_int();
		для (auto &term: macc.terms) {
			якщо (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned ціле sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		повернути cost_sum;
	} інакше якщо (cell->type == ID($lut)) {
		ціле width = cell->getParam(ID::WIDTH).as_int();
		unsigned ціле cost = 1U << (unsigned ціле)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		повернути cost;
	} інакше якщо (cell->type == ID($sop)) {
		ціле width = cell->getParam(ID::WIDTH).as_int();
		ціле depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		повернути (2 * width + 1) * depth;
	} інакше якщо (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		повернути 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	повернути 1;
}
