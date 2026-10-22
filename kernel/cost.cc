#include "kernel/cost.h"
#include "kernel/macc.h"

USING_YOSYS_NAMESPACE

unsigned 정수 CellCosts::get(RTLIL::Module *mod)
{
	만약 (mod_cost_cache_.count(mod->name))
		반환 mod_cost_cache_.at(mod->name);

	unsigned 정수 module_cost = 1;
	위해 (auto c : mod->cells()) {
		unsigned 정수 new_cost = module_cost + get(c);
		module_cost = new_cost >= module_cost ? new_cost : INT_MAX;
	}

	mod_cost_cache_[mod->name] = module_cost;
	반환 module_cost;
}

static unsigned 정수 y_coef(RTLIL::IdString type)
{
	만약 (
	  // equality
	  type.in(ID($bweqx), ID($nex), ID($eqx)) ||
	  // basic logic
	  type.in(ID($and), ID($or), ID($xor), ID($xnor), ID($not)) ||
	  // mux
	  type.in(ID($bwmux), ID($mux)) ||
	  // others
	  type == ID($tribuf)) {
		반환 1;
	} 아니면 만약 (type == ID($neg)) {
		반환 4;
	} 아니면 만약 (type == ID($demux)) {
		반환 2;
	} 아니면 만약 (type == ID($fa)) {
		반환 5;
	} 아니면 만약 (type.in(ID($add), ID($sub), ID($alu))) {
		// multi-bit adders
		반환 8;
	} 아니면 만약 (type.in(ID($shl), ID($sshl))) {
		// left shift
		반환 10;
	}
	반환 0;
}

static unsigned 정수 max_inp_coef(RTLIL::IdString type)
{
	만약 (
	  // binop reduce
	  type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)) ||
	  // others
	  type.in(ID($logic_not), ID($pmux), ID($bmux))) {
		반환 1;
	} 아니면 만약 (
	  // equality
	  type.in(ID($eq), ID($ne)) ||
	  // logic
	  type.in(ID($logic_and), ID($logic_or))) {
		반환 2;
	} 아니면 만약 (type == ID($lcu)) {
		반환 5;
	} 아니면 만약 (type.in(ID($lt), ID($le), ID($ge), ID($gt))) {
		// comparison
		반환 7;
	}
	반환 0;
}

static unsigned 정수 sum_coef(RTLIL::IdString type)
{
	만약 (type.in(ID($shr), ID($sshr))) {
		// right shift
		반환 4;
	} 아니면 만약 (type.in(ID($shift), ID($shiftx))) {
		// shift
		반환 8;
	}
	반환 0;
}

static unsigned 정수 is_div_mod(RTLIL::IdString type)
{
	반환 (type == ID($div) || type == ID($divfloor) || type == ID($mod) || type == ID($modfloor));
}

static bool is_free(RTLIL::IdString type)
{
	반환 (
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

unsigned 정수 max_inp_width(RTLIL::Cell *cell)
{
	unsigned 정수 max = 0;
	RTLIL::IdString input_width_params[] = {
	  ID::WIDTH,
	  ID::A_WIDTH,
	  ID::B_WIDTH,
	  ID::S_WIDTH,
	};

	만약 (cell->type == ID($bmux))
		반환 cell->getParam(ID::WIDTH).as_int() << cell->getParam(ID::S_WIDTH).as_int();

	위해 (RTLIL::IdString param : input_width_params)
		만약 (cell->hasParam(param))
			max = std::max(max, (unsigned 정수)cell->getParam(param).as_int());
	반환 max;
}

unsigned 정수 port_width_sum(RTLIL::Cell *cell)
{
	unsigned 정수 sum = 0;
	RTLIL::IdString port_width_params[] = {
	  ID::WIDTH, ID::A_WIDTH, ID::B_WIDTH, ID::S_WIDTH, ID::Y_WIDTH,
	};

	위해 (auto param : port_width_params)
		만약 (cell->hasParam(param))
			sum += cell->getParam(param).as_int();

	반환 sum;
}

unsigned 정수 CellCosts::get(RTLIL::Cell *cell)
{

	// simple 1-bit cells
	만약 (cmos_gate_cost().count(cell->type))
		반환 1;

	만약 (design_ && design_->module(cell->type) && cell->parameters.empty()) {
		log_debug("%s is a module, recurse\n", cell->name);
		반환 get(design_->module(cell->type));
	} 아니면 만약 (cell->is_builtin_ff()) {
		log_assert(cell->hasPort(ID::Q) && "Weird flip flop");
		log_debug("%s is ff\n", cell->name);
		반환 cell->getParam(ID::WIDTH).as_int();
	} 아니면 만약 (cell->type.in(ID($mem), ID($mem_v2))) {
		log_debug("%s is mem\n", cell->name);
		반환 cell->getParam(ID::WIDTH).as_int() * cell->getParam(ID::SIZE).as_int();
	} 아니면 만약 (y_coef(cell->type)) {
		// linear with Y_WIDTH or WIDTH
		log_assert((cell->hasParam(ID::Y_WIDTH) || cell->hasParam(ID::WIDTH)) && "Unknown width");
		auto param = cell->hasParam(ID::Y_WIDTH) ? ID::Y_WIDTH : ID::WIDTH;
		정수 width = cell->getParam(param).as_int();
		만약 (cell->type == ID($demux))
			width <<= cell->getParam(ID::S_WIDTH).as_int();
		log_debug("%s Y*coef %d * %d\n", cell->name, width, y_coef(cell->type));
		반환 width * y_coef(cell->type);
	} 아니면 만약 (sum_coef(cell->type)) {
		// linear with sum of port widths
		unsigned 정수 sum = port_width_sum(cell);
		log_debug("%s sum*coef %d * %d\n", cell->name, sum, sum_coef(cell->type));
		반환 sum * sum_coef(cell->type);
	} 아니면 만약 (max_inp_coef(cell->type)) {
		// linear with largest input width
		unsigned 정수 max = max_inp_width(cell);
		log_debug("%s max*coef %d * %d\n", cell->name, max, max_inp_coef(cell->type));
		반환 max * max_inp_coef(cell->type);
	} 아니면 만약 (is_div_mod(cell->type) || cell->type == ID($mul)) {
		// quadratic with sum of port widths
		unsigned 정수 sum = port_width_sum(cell);
		unsigned 정수 coef = cell->type == ID($mul) ? 3 : 5;
		log_debug("%s coef*(sum**2) %d * %d\n", cell->name, coef, sum * sum);
		반환 coef * sum * sum;
	} 아니면 만약 (cell->type.in(ID($macc), ID($macc_v2))) {
		// quadratic per term
		unsigned 정수 cost_sum = 0;
		Macc macc;
		macc.from_cell(cell);
		unsigned 정수 y_width = cell->getParam(ID::Y_WIDTH).as_int();
		위해 (auto &term: macc.terms) {
			만약 (term.in_b.empty()) {
				// neglect addends
				continue;
			}
			unsigned a_width = term.in_a.size(), b_width = term.in_b.size();
			unsigned 정수 sum = a_width + b_width + std::min(y_width, a_width + b_width);
			cost_sum += 3 * sum * sum;
		}
		반환 cost_sum;
	} 아니면 만약 (cell->type == ID($lut)) {
		정수 width = cell->getParam(ID::WIDTH).as_int();
		unsigned 정수 cost = 1U << (unsigned 정수)width;
		log_debug("%s is 2**%d\n", cell->name, width);
		반환 cost;
	} 아니면 만약 (cell->type == ID($sop)) {
		정수 width = cell->getParam(ID::WIDTH).as_int();
		정수 depth = cell->getParam(ID::DEPTH).as_int();
		log_debug("%s is (2*%d + 1)*%d\n", cell->name, width, depth);
		반환 (2 * width + 1) * depth;
	} 아니면 만약 (is_free(cell->type)) {
		log_debug("%s is free\n", cell->name);
		반환 0;
	}
	// TODO: $fsm
	// ignored: $pow $memrd $memwr $meminit (and v2 counterparts)

	log_warning("Can't determine cost of %s cell (%d parameters).\n", cell->type.unescape(), GetSize(cell->parameters));
	반환 1;
}
